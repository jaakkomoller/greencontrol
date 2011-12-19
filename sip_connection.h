#ifndef SIP_CONNECTION_H
#define SIP_CONNECTION_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct {
	char* Req;
	char* Via;
	char*	From;
	char* To;
	char* Call_ID;
	char* CSeq;
	char* Accept;
	char* Cnt_type;
	char* Allow;
	char* Max_FWD;
	char* UA;
	char* Subject;
	char* Expires;
	char* Cnt_len;
	char* Msg_body;
}Sip_in;

struct connection {
	Sip_in sip_conn;
	int port, is_connected;
	pid_t trans_pid, mp3_fetch_pid, rtp_serv_pid;
	int mp3_fetcher_control; // This pipes control data to mp3 fetcher
	int transcoder_control; // This pipes control data to transcoder
	int rtp_server_control; // This pipes control data to rtp server
};

struct node
{
	struct connection data;
	struct node *link;
};

void append(struct node **, struct connection *);
void in_begin(struct node **, struct connection *);
void del(struct node **, char *);
void in_middle(struct node **, int, struct connection *);
int count(struct node *);
void display(struct node *);

#endif /*SIP_CONNECTION_H*/
