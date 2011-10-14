#include <sys/socket.h> /* socket creation */
#include <sys/types.h>
#include <netinet/in.h> /* socket address */
#include <stdio.h> /* FILE streams, input/output*/
#include <strings.h> /* bzero */
#include <errno.h> /* errors */
#include <unistd.h> /* write&read */
#include <arpa/inet.h> /* inet_pton */

#include "mp3fetcher.h"

#define MAXLINE 128
#define SA struct sockaddr

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
