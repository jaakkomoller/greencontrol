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
#include "mp3fetcher.h"

#define SA struct sockaddr
#define REQUEST "GET /radio/soundtracks HTTP/1.0\r\nHOST:www.shoutcast.com\r\n\r\n"
#define MAXLINE 128
#define MAXBUFFER 1024
#define FRAMESIZE 417


/*
* Fetches radio station information and generates menu items based on this info
*/

int fetch_station_info()
{
printf("Fetching information from shoutcast.com and generating menu items\n");

char page[50000]="";
int status, i;

for (i=0;i<10;i++)
{
	status = fetch_page("www.shoutcast.com","80",REQUEST,page);

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
//char *string = &html;
char       *pattern = "on playimage\" name=\"\\(.*\\)\"></a>";
int        rc;
size_t     nmatch = 2;
regmatch_t pmatch[2];


   if (0 != (rc = regcomp(&preg, pattern, REG_NEWLINE))) {
      exit(EXIT_FAILURE);
   }

char station1[100]="";
char station2[100]="";
char station3[100]="";
char station4[100]="";

int j;
for(j=1;j<5;j++)
{
   if (0 != (rc = regexec(&preg, page, nmatch, pmatch, 0))) {
      printf("Failed to match\n");
   }
   else{

if (j==1)
{
	sprintf(station1,"%.*s", pmatch[1].rm_eo - pmatch[1].rm_so, &page[pmatch[1].rm_so]);
}
if (j==2)
{
	sprintf(station2,"%.*s", pmatch[1].rm_eo - pmatch[1].rm_so, &page[pmatch[1].rm_so]);
}
if (j==3)
{
	sprintf(station3,"%.*s", pmatch[1].rm_eo - pmatch[1].rm_so, &page[pmatch[1].rm_so]);
}
if (j==4)
{
	sprintf(station4,"%.*s", pmatch[1].rm_eo - pmatch[1].rm_so, &page[pmatch[1].rm_so]);
}

//printf("%.*s\n",  pmatch[1].rm_eo - pmatch[1].rm_so, &page[pmatch[1].rm_so]);


page[pmatch[1].rm_so-1]="0"; //reset one bit of the string so regexec does not match the same line again.

  }
}

//freeing of memory
regfree(&preg);

//Generate a menu

char menu;
char selection=0;
int selected=0;


do {

if (selection!=0)
goto loop;

	printf("\n##############################################################################");
  	printf("\n                              RadioStreamer");
	printf("\n##############################################################################");
  	printf("\n[1] \"%s\"",station1);
  	printf("\n[2] \"%s\"",station2);
  	printf("\n[3] \"%s\"",station3);
   	printf("\n[4] \"%s\"",station4);
        printf("\n##############################################################################");
	printf("\n[N]ext, [P]ause, [C]ontinue, [E]xit");
	printf("\n##############################################################################");
	printf("\nChoose a channel: ");

scanf("%c", &menu);
getchar();
selection=toupper(menu);

loop:
switch(selection) {

	case '1':
	printf("\nChannel [1] was chosen\n");
	selected=1;
	selection=fetch_playlist(station1);
	break;

      	case '2':
        printf("\nChannel [2] was chosen\n");
	selected=2;
	selection=fetch_playlist(station2);
        break;

	case '3':
	printf("\nChannel [3] was chosen\n");
	selected=3;
	selection=fetch_playlist(station3);
        break;

  	case '4':
	printf("\nChannel [4] was chosen\n");
	selected=4;
	selection=fetch_playlist(station4);
        break;

	case 'N':
	selected=selected+1;

	if (selected==5)
	selected=1;

 	if (selected==1)
	{
	printf("\nChannel [1] was chosen\n");
	selection=fetch_playlist(station1);
	}
        if (selected==2)
        {
	printf("\nChannel [2] was chosen\n");
        selection=fetch_playlist(station2);
	}
	if (selected==3)
        {
	printf("\nChannel [3] was chosen\n");
        selection=fetch_playlist(station3);
	}
	if (selected==4)
        {
	printf("\nChannel [4] was chosen\n");
        selection=fetch_playlist(station4);
	}

        break;

	case 'E':
		exit(0);

	default:
		selection=0;
		break;

}
} while(selection != 'E');




printf("\nIt's over\n");

exit(0);
}


/*
* Fetches and parses a playlist html page and tries to call fetch_file() function (possibly several times)
*/
int fetch_playlist(char* station)
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
ipandport[100]="";
sprintf(ipandport,"%.*s", pmatch[1].rm_eo - pmatch[1].rm_so, &page[pmatch[1].rm_so]);

char * ip="";
char * port="";

ip=strtok(ipandport,":");
port=strtok(NULL,":");

page[pmatch[1].rm_so-1]="0"; //reset one bit of the string so regexec does not match the same line again.


//fetch_file
int fetch=fetch_file(ip,port);

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



/*
* Parses a string by delimeter
*/
char* parseString(char* MESSAGE)
{
char id[15]="";
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
//printf("socket:%d\n",sockfd);

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
if (snprintf(RECEIVED_PAGE+strlen(RECEIVED_PAGE),n,buffer)==EOF) /* write current buffer to string */
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

int fetch_file(char* IP,char* PORT)
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

FD_ZERO(&readsetfds2); /* clear all bits in the set */
FD_ZERO(&readsetfds);

FD_SET(sockfd, &readsetfds); /* Turn on bit for fd in the set */
FD_SET(0, &readsetfds);


int pipefd[2];

if (pipe(pipefd) == -1)
{
        perror("Pipe");
	return -1;
}



int read_bytes=0;
int read_bytes_prev=0;
int sum;

int count=0;

//Wait for some input
for(;;)
{
readsetfds2 = readsetfds;
selectid=select(sockfd+1, &readsetfds2, NULL, NULL,NULL);

	if (selectid <= 0)
	{
	perror("Select");
	return -1;
	}
	int ispaused=0;

	if (selectid > 0) /* something to read */
	{
		if (FD_ISSET(sockfd, &readsetfds2)) //MP3 stream
  		{

		if ((read_bytes = recv(sockfd, buffer, 417,0))<0)
  		{
  		perror("recv");
		return -1;
  		}

		sum=read_bytes+read_bytes_prev; //Calculate the maximum amount of data that can be written to the buffer. It consists of the data that was just read and the older data, which is in the buffer already, that was not needed before to achieve a full frame.


		if (sum >= FRAMESIZE) // A full mp3 frame fetched so write it to the buffer
		{

//			write(1,buffer,FRAMESIZE-read_bytes_prev);
			count++;

			if (count >=5) // Call the transcoder function when there are 5 full frames in the buffer
			{
				goto call_transcoder;
			}

			extra_bytes:  // Calculate the extra bytes and write them to the buffer too

			read_bytes_prev=sum-(FRAMESIZE-read_bytes_prev);

			if (read_bytes-read_bytes_prev<-1)
			{
			}
			else
			{
				int j;
				for(j=0;j<read_bytes_prev;j++)
                		{
                        		new_buffer[j] = buffer[read_bytes-read_bytes_prev+1+j];
                		}
//				write(1,new_buffer,read_bytes_prev);
			}

			continue;

		}


		else // (sum <FRAMESIZE)
                {
//	       	        write(1,buffer,read_bytes); // Add these bytes to the buffer. It's not a full frame though
                	read_bytes_prev=sum;
			continue;
    		}

		call_transcoder:
                {

			if (ispaused==1) // pause is pressed so do not call the tanscoder funtion
			{
//				TODO:  clear the pipe, pipefd[0] & 417*count
			}
			else // pause is not pressed so call the transcoder funtion
			{
//				call_transcoder(pipefd,417*count); // In the other end just read from pipefd[0]
				count=0;
				goto extra_bytes;  // Check if there were some more bytes available
                	}

		}

		}
		if (FD_ISSET(0, &readsetfds2)) //USER INPUT
                {

			char menu;
			scanf("%c", &menu);
			menu=toupper(menu);
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



	}


}



if ((x=close(sockfd))<0)
perror("error when closing the socket");


}
