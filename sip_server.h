#ifndef SIP_SERVER_H
#define SIP_SERVER_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> /*constants and structures needed for internet domain addresses.*/
#include <arpa/inet.h>
#include "sip_connection.h"

#define SUPPORT 1
#define UNSUPPORT 4
#define TRUE 2
#define FALSE 3

char* UserAgent = "User-Agent: Radio-Streamer";

void error(const char *msg)
{
    perror(msg);
    exit(1);
}


typedef struct {
	char* Status;
	char* Via;
	char*	From;
	char* To;
	char* Call_ID;
	char* CSeq;
	char* Allow;
	char* Accept;
	char* Contact;
	char* Cnt_type;
	char* UA;
	char* Cnt_len;
}Sip_out;

typedef struct {
	char* v;
	char* o;
	char* s;
	char* c;
	char* t;
	char* m;
	char* a1;
	char* a2;
	char* a3;

}Sip_body;
Sip_out *out_init(void);
Sip_body *body_init(void);

int sip_server_kick(char stations[][100], int station_count, int portno, int *state);
char* OPTIONS_Handle(Sip_in *client, struct sockaddr_in client_addr,char *msg);
char* INVITE_Handle(Sip_in *client, Sip_body *body,struct sockaddr_in client_addr,char* msg);
char* UNSUPPORT_Handle(Sip_in *client, struct sockaddr_in client_addr,char* msg);
char* UNSUPPORTINFO_Handle(Sip_in *client, struct sockaddr_in client_addr,char* msg);
char* INFO_Handle(Sip_in *client, struct sockaddr_in client_addr,char* msg);
char* BYE_Handle(Sip_in *client, struct sockaddr_in client_addr,char *msg);

void free_out(Sip_out* stream);
void free_body(Sip_body* stream);

#endif /*SIP_SERVER_H*/
