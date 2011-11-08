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

#define MAXLINE 128
#define SA struct sockaddr

#define SHOUTCAST "www.shoutcast.com/radio/soundtracks"
#define REQUEST "GET /radio/soundtracks HTTP/1.0\r\nHOST:www.shoutcast.com\r\n\r\n"
#define MAXBUFFER 1024

int fetch_station_info()
{

struct addrinfo hints, *i ,*retaddr;
int n,m,sockfd;
char buffer[MAXBUFFER];
int status;

printf("Fetching information from shoutcast.com and generating menu items\n");

//define hints
memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC; // IPv6 or IPv4 protocol independent
hints.ai_socktype = SOCK_STREAM; //TCP

//hostname resolving
if (status=getaddrinfo("www.shoutcast.com","80", &hints, &retaddr) != 0)
{
	//printf("an error occured with getaddrinfo\n");
	fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
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

;

if ((m = send(sockfd,REQUEST,strlen(REQUEST),0))<0) /* HTTP GET */
perror("http GET error");
char html[50000];

while ( (n = recv(sockfd, buffer, MAXBUFFER-1,0)) > 0)  /* read until n is set to 0 */
{

buffer[n] = 0; /* null terminate */
if (snprintf(html+strlen(html),n,buffer)==EOF) /* write current buffer to string */
perror("sprintf error");

if (n < 0)
perror("read error");

}

//printf("%s",html);


regex_t    preg;
//   char       *string = "http://yp.shoutcast.com/sbin/tunein-station.pls?id=1283288\" title=\"Radio Rivendell - The Fantasy Station\" class=\"playbutton playimage\" name=\"Radio Rivendell - The Fantasy Station\" id=\"1283288\"></a>";
char *string = &html;

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
   if (0 != (rc = regexec(&preg, string, nmatch, pmatch, 0))) {
      printf("Failed to match\n");
   }
   else{

if (j==1)
{
	sprintf(station1,"%.*s", pmatch[1].rm_eo - pmatch[1].rm_so, &string[pmatch[1].rm_so]);
}
if (j==2)
{
	sprintf(station2,"%.*s", pmatch[1].rm_eo - pmatch[1].rm_so, &string[pmatch[1].rm_so]);
}
if (j==3)
{
	sprintf(station3,"%.*s", pmatch[1].rm_eo - pmatch[1].rm_so, &string[pmatch[1].rm_so]);
}
if (j==4)
{
	sprintf(station4,"%.*s", pmatch[1].rm_eo - pmatch[1].rm_so, &string[pmatch[1].rm_so]);
}

//printf("%.*s\n",  pmatch[1].rm_eo - pmatch[1].rm_so, &string[pmatch[1].rm_so]);


string[pmatch[1].rm_so-1]="0"; //reset one bit of the string so regexec does not match the same line again.

  }
}

//freeing of memory
regfree(&preg);
freeaddrinfo(retaddr); // Release the storage allocated by getaddrinfo() call



char menu;
char selection;
do {

	printf("\n##############################################################################");
  	printf("\n                              RadioStreamer");
	printf("\n##############################################################################");
  	printf("\n[1] \"%s\"",station1);
  	printf("\n[2] \2%s\"",station2);
  	printf("\n[3] \"%s\"",station3);
   	printf("\n[4] \"%s\"",station4);
        printf("\n##############################################################################");
	printf("\n[N]ext, [P]ause, [C]ontinue, [E]xit");
	printf("\n##############################################################################");
	printf("\nChoose a channel: ");

scanf("%c", &menu);
getchar();
selection=toupper(menu);

switch(selection) {

	case '1':
	printf("\nChannel 1 was chosen\n");
	break;

      	case '2':
        printf("\nChannel 2 was chosen\n");
        break;

	case '3':

        break;

  	case '4':

        break;

	case 'N':

        break;

  	case 'P':

        break;

  	case 'C':
	printf("\ncontinue");
        break;

}
  } while(selection != 'E');



printf("\nIt's over\n");

return 0;
}



void fetch_file()
{

char buffer_orig[MAXLINE + 1];
struct sockaddr_in servaddr;
int sockfd, n,m,x,numbytes;


if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) /* create a socket, SOCK_STREAM -> TCP */
perror("socket error");

bzero(&servaddr, sizeof(servaddr));
servaddr.sin_family = AF_INET;
servaddr.sin_port = htons(9000); /* http server */

if (inet_pton(AF_INET, "83.145.128.37", &servaddr.sin_addr) <= 0) /* Convert an address from ASCII string format to binary */
perror("inet_pton error");

if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0) /* connect to server */
perror("connect error");




if ((m = send(sockfd, "GET / HTTP/1.1\r\nHost: 83.145.128.37:9000\r\nUser-Agent: uberclient/10\r\nRange: bytes=0-\r\n\r\n", MAXLINE,0))<0) /* HTTP GET */
perror("http GET error");


fd_set readsetfds;
fd_set readsetfds2; /* temp*/

char buffer[1024];


int selectid;

FD_ZERO(&readsetfds2); /* clears all bits in the set */
FD_ZERO(&readsetfds);

FD_SET(sockfd, &readsetfds); /* Turn on bit for fd in the set */




for(;;)
{
readsetfds2 = readsetfds;
selectid=select(sockfd+1, &readsetfds2, NULL, NULL,NULL);

	if (selectid <= 0)
	{
	perror("Select");

	}


	if (selectid > 0) /* something to read */
	{
		if (FD_ISSET(sockfd, &readsetfds2))
  		{

			if ((  n = recv(sockfd, buffer, 1023,0))<0)
  			{
  			perror("recv");
  			}

			if (write(1, buffer, n) <0) break;

		}
	}


}



//if ((x=close(sockfd))<0)
//perror("error when closing the socket");




}
