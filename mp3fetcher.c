#include <sys/socket.h> /* for socket creation */
#include <sys/types.h>
#include <netinet/in.h> /* for socket address */
#include <stdio.h> /* for FILE streams, input/output*/
#include <strings.h> /* for bzero */
#include <errno.h> /* for  errors */
#include <unistd.h> /* for write&read */
#include <arpa/inet.h> /* for inet_pton */
#include <string.h> /* for memset */
#include <netdb.h> /* for hints*/
#include <regex.h>
#include <stdlib.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include "mp3fetcher.h"

#define SA struct sockaddr
#define REQUEST "GET /radio/Original%20Score HTTP/1.0\r\nHOST:www.shoutcast.com\r\n\r\n"
#define MAXLINE 128
#define MAXBUFFER 1024
#define FRAMESIZE 417

/*
 * Fetches radio station information and generates menu items based on this info
 */

int fetch_station_info(char stations[][100], int max_stations)
{
	printf("Fetching information from shoutcast.com and generating menu items\n");

	char page[50000] = "";
	int status, i;

	for(i = 0; i < max_stations; i++)
		sprintf(stations[i], "");

	for (i = 0; i < 10; i++)
	{
		status = fetch_page("www.shoutcast.com", "80", REQUEST, page);

		if (status==0)
		{
			break;
		}

		if (i==9)
		{
			printf("Cannot fetch the channel info...\n");
			exit(1);
		}

		sleep(1);
	}

	//printf("%s",page);


	//parse radio stations from html page

	regex_t    preg;
	char       *pattern = "on playimage\" name=\"\\(.*\\)\"></a>";
	int        rc;
	size_t     nmatch = 2;
	regmatch_t pmatch[2];

	if (0 != (rc = regcomp(&preg, pattern, REG_NEWLINE))) {
		exit(EXIT_FAILURE);
	}

	// for bitrate checking
	regex_t    preg2;
	char       *pattern2 = "<div class=\"dirbitrate\">([^<]+)</div>";
	int        rc2;
	size_t     nmatch2 = 2;
	regmatch_t pmatch2[2];

	if (0 != (rc2 = regcomp(&preg2, pattern2, REG_EXTENDED))) {
		exit(EXIT_FAILURE);
	}

	// for audio type checking
	regex_t    preg3;
	char       *pattern3 = "<div class=\"dirtype\">([^<]+)</div>";
	int        rc3;
	size_t     nmatch3 = 2;
	regmatch_t pmatch3[2];

	if (0 != (rc3 = regcomp(&preg3, pattern3, REG_EXTENDED))) {
		exit(EXIT_FAILURE);
	}

	char bitrate[50]="";
	char * parsed;
	char type[50]="";
	char * type_parsed;

	int j,station_count=0, match=1;

	for(j=0;1;j++)
	{
		if (0 != (rc = regexec(&preg, page, nmatch, pmatch, 0))) { //Get a station
//			printf("Failed to match\n");
			break;
		}
		else{
			// Check that the bitrate of the stream of selected radio station is 128 kbps
			if (0 != (rc2 = regexec(&preg2, page, nmatch2, pmatch2, 0))) {
				printf("Failed to match bitrate\n");
			}

			sprintf(bitrate,"%.*s", pmatch2[1].rm_eo - pmatch2[1].rm_so, &page[pmatch2[1].rm_so]);
			parsed = strtok (bitrate," \n");// the actual bitrate of the stream
			page[pmatch2[1].rm_so-1]='0'; //reset one bit of the string so regexec does not match the same line again.

			// Check that the audio type of the stream of selected radio station is mp3
			if (0 != (rc3 = regexec(&preg3, page, nmatch3, pmatch3, 0))) {
				printf("Failed to match audio type\n");
			}

			sprintf(type,"%.*s", pmatch3[1].rm_eo - pmatch3[1].rm_so, &page[pmatch3[1].rm_so]);
			type_parsed = strtok (type," \n");// mp3 or aac+
			page[pmatch3[1].rm_so-1]='0'; //reset one bit of the string so regexec does not match the same line again.

			if (strncmp(parsed,"128",3)==0 && strncmp(type_parsed,"MP3",3)==0)
//			if (strncmp(type_parsed,"MP3",3)==0)
			{
				station_count++; //128 kbps mp3 stream
				match=0;
			}

			if (station_count > 0 && station_count <= max_stations && match == 0) {
				sprintf(stations[station_count - 1],"%.*s", pmatch[1].rm_eo - pmatch[1].rm_so, &page[pmatch[1].rm_so]);
//				printf("added %s, bitrate: %s\n", stations[station_count-1], parsed);
			}
			if (station_count == max_stations) {
				printf("breaking, %d, %d\n", station_count, max_stations);
				break;
			}

			if (strncmp(type_parsed,"MP3",3)==0)
			{
				page[pmatch[1].rm_so-1]='0'; //reset one bit of the string so regexec does not match the same line again
			}

			match=1;

		}
	}

	//freeing of memory
	regfree(&preg);
	regfree(&preg2);
	regfree(&preg3);

	return station_count;
}


int start_gui(int outfile, int *state, char stations[][100], int station_count) {

	//Generate a menu

	char menu[100];
	char selection = 0;
	int int_selection = 0;
	int selected = 0;
	int i = 0;

	do {

		if (selection!=0)
			goto loop;

		printf("\n##############################################################################");
		printf("\n                              RadioStreamer");
		printf("\n##############################################################################");
		for(i = 0; i < station_count; i++)
		printf("\n[%d] \"%s\"", i+1, stations[i]);
		printf("\n##############################################################################");
		printf("\n[N]ext, [P]ause, [C]ontinue, [E]xit");
		printf("\n##############################################################################");
		printf("\nChoose a channel: ");

		scanf("%s", menu);
		getchar();

loop:
		selection = toupper(menu[0]);
		int_selection = atoi(menu);

		// Channel number selected
		if (isdigit(menu[0]) && int_selection > 0 && int_selection <= station_count) {
			printf("\nChannel [%d] was chosen\n", int_selection);
			selected = int_selection;
			fetch_playlist(outfile, state, stations[int_selection - 1], menu);
			goto loop;
		}

		else if (int_selection > station_count)
			printf("\nPlease select a channel between 1 and %d\n", station_count);
		
		switch(selection) {

			case 'N':
				selected = selected + 1;

				if (selected == station_count +1)
					selected = 1;
				
				sprintf(menu, "%d", selected);

				break;

			case 'E':
				break;

			default:
				selection = 0 ;
				break;

		}
	} while(selection != 'E');

	return 0;
}


/*
 * Fetches and parses a playlist html page and tries to call fetch_file() function (possibly several times)
 */
int fetch_playlist(int outfile, int *state, char* station, char *buf)
{
	char page[10000]="";
	char *parsed="";

	parsed = parseString(station);

	//create http get request
	char get[150]="";
	sprintf(get,"GET http://yp.shoutcast.com/sbin/tunein-station.pls?id=%s HTTP/1.0\r\n\r\n",parsed);

	fetch_page("yp.shoutcast.com","80",get,page);
	//printf("%s\n",page);



	//parse playlist file to get IP and PORT

	regex_t    preg;
	char       *pattern = "=http://\\(.*\\)";
	int        rc;
	size_t     nmatch = 2;
	regmatch_t pmatch[2];


	if (0 != (rc = regcomp(&preg, pattern, REG_NEWLINE))) {
		exit(EXIT_FAILURE);
	}

	char ipandport[100]="";

	//First parse ip and port from playlist, then try to connect (and repeat with different ip+port if necessary)
	while(regexec(&preg, page, nmatch, pmatch, 0)==0 )
	{
		ipandport[100]=0;
		sprintf(ipandport,"%.*s", pmatch[1].rm_eo - pmatch[1].rm_so, &page[pmatch[1].rm_so]);

		char * ip="";
		char * port="";

		ip=strtok(ipandport,":");
		port=strtok(NULL,":");

		page[pmatch[1].rm_so-1]='0'; //reset one bit of the string so regexec does not match the same line again.


		//fetch_file
		int fetch = fetch_file(outfile, state, ip, port, buf);

		if (fetch==-1)
		{
			continue;
		}

		if (fetch>=0)
		{
			//freeing of memory
			regfree(&preg);
			return fetch;
		}

	}

	//freeing of memory
	regfree(&preg);


}

char id[15]="";

/*
 * Parses a string by delimeter
 */
char* parseString(char* MESSAGE)
{
	//char id[15]="";
	regex_t    preg;
	char       *pattern ="\" id=\"\\(.*\\)";
	int        rc;
	size_t     nmatch = 2;
	regmatch_t pmatch[2];


	if (0 != (rc = regcomp(&preg, pattern, REG_NEWLINE))) {
		exit(EXIT_FAILURE);
	}

	if (0 != (rc = regexec(&preg, MESSAGE, nmatch, pmatch, 0))) {
		printf("Failed to match\n");
	}

	sprintf(id,"%.*s", pmatch[1].rm_eo - pmatch[1].rm_so, &MESSAGE[pmatch[1].rm_so]);
	regfree(&preg);

	return id;
}


/*
 * Fetches an html page (HTTP GET) and creates a string variable from the page
 */
int fetch_page(char* URL, char* PORT,char* HTTP_GET,char* RECEIVED_PAGE)
{
	struct addrinfo hints, *i ,*retaddr;
	int n,m,sockfd;
	char buffer[MAXBUFFER];
	int status;

	//define hints
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // IPv6 or IPv4 protocol independent
	hints.ai_socktype = SOCK_STREAM; //TCP

	//hostname resolving
	if (status=getaddrinfo(URL,PORT, &hints, &retaddr) != 0)
	{
		//printf("an error occured with getaddrinfo\n");
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		return 1;
	}

	//loop through the resuls and create a socket and connect()
	for(i = retaddr; i != NULL; i = i->ai_next)
	{
		if ((sockfd = socket(i->ai_family, i->ai_socktype,i->ai_protocol)) == -1)
		{
			perror("error when creating a socket");
			continue;
		}

		if (connect(sockfd, i->ai_addr, i->ai_addrlen) == -1) {
			close(sockfd);
			perror("connect");
			continue;
		}

		break;
	}

	//freeing of memory
	freeaddrinfo(retaddr); // Release the storage allocated by getaddrinfo() call

	if ((m = send(sockfd,HTTP_GET,strlen(HTTP_GET),0))<0) /* HTTP GET */
	{
		perror("http GET error");
		return 1;
	}

	while ( (n = recv(sockfd, buffer, MAXBUFFER-1,0)) > 0)  /* read until n is set to 0 */
	{
		buffer[n] = 0; /* null terminate */
		if (sprintf(RECEIVED_PAGE+strlen(RECEIVED_PAGE),"%s",buffer)==EOF) /* write current buffer to RECEIVED_PAGE */
		{
			//perror("sprintf error");
			//return 1;
		}
		if (n < 0)
		{
			perror("read error");
			return 1;
		}
	}
	return 0;
}



/*
 * Fetches an mp3 file stream
 */

int fetch_file(int outfile, int *state, char* IP, char* PORT, char *buf)
{
	//Try to connect to a certain IP:PORT

	struct sockaddr_in servaddr;
	int sockfd, n,m,x,numbytes;


	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) /* create a socket, SOCK_STREAM -> TCP */
	{
		perror("socket error");
		return -1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(PORT)); /* http server */

	if (inet_pton(AF_INET, IP, &servaddr.sin_addr) <= 0) /* Convert an address from ASCII string format to binary */
	{
		perror("inet_pton error");
		return -1;
	}

	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0) /* connect to server */
	{
		perror("connect error");
		return -1;
	}

	//create http get request
	char get[150]="";
	sprintf(get,"GET / HTTP/1.1\r\nHost: %s:%s\r\nUser-Agent: uberclient/10\r\nRange: bytes=0-\r\n\r\n",IP,PORT);

	if ((m = send(sockfd, get, MAXLINE,0))<0) /* HTTP GET */
	{
		perror("http GET error");
		return -1;
	}

	fd_set readsetfds;
	fd_set readsetfds2; /* temp*/

	char buffer[FRAMESIZE+1];//[1024];
	char new_buffer[FRAMESIZE+1];

	int selectid;

	FD_ZERO(&readsetfds);

	int read_bytes = 0;
	int read_bytes_prev = 0;
	int sum;
	int ispaused = 0;

	int count=0;
	int flags = 0;

	if (-1 == (flags = fcntl(sockfd, F_GETFL, 0)))
		flags = 0;
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	//Wait for some input
	for(;;)
	{
		FD_SET(sockfd, &readsetfds); /* Turn on bit for fd in the set */
		FD_SET(0, &readsetfds);

		selectid = select(sockfd+1, &readsetfds, NULL, NULL,NULL);

		if (selectid <= 0)
		{
			perror("Select");
			return -1;
		}

		if (selectid > 0) /* something to read */
		{
			if (FD_ISSET(0, &readsetfds)) //USER INPUT (needs to be read first)
			{
				char menu;
				scanf("%s", buf);
				menu = toupper(buf[0]);
				getchar();

				if (menu=='P')
				{
					printf("Paused\n");
					ispaused=1;
					continue;
				}

				if (menu=='C')
				{
					printf("Continue\n");
					ispaused=0;
					continue;
				}

				return menu;
			}
			if (FD_ISSET(sockfd, &readsetfds)) //MP3 stream
			{

				if ((read_bytes = recv(sockfd, buffer, 417,0))<0)
				{
					perror("recv");
					return -1;
				}

				if (!ispaused)
					write(outfile,buffer,read_bytes);

			}
		}
	}

	if ((x=close(sockfd))<0)
		perror("error when closing the socket");

}
