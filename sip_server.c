#include "sip_server.h"
 /*gcc -std=c99 -g -W -Wall -o ./Sip_server sip_server.c*/


int main(int argc, char *argv[])
{
	int sockfd; /*File descriptor for socket*/
	int portno; /*port number*/
	socklen_t clilen; /*stores the size of the address of the client. This is needed for the accept system call.*/
	char buffer[BUFLEN]; /*Buffer for reading each datagram*/
	struct sockaddr_in serv_addr, cli_addr;
	/*struct sockaddr_in
	{
	  short   sin_family; // must be AF_INET 
	  u_short sin_port;
	  struct  in_addr sin_addr;
	  char    sin_zero[8]; // Not used, must be zero 
	};*/
	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sockfd < 0) 
        error("ERROR opening socket");
	memset((char *) &serv_addr,'\0',sizeof(serv_addr)); /*set all addresses to 0*/
	portno = 50000; /*fundamentally set to 50000*/
	serv_addr.sin_family = AF_INET; /*Address family*/
	serv_addr.sin_addr.s_addr = INADDR_ANY; /* Gets IP address of the host*/
	serv_addr.sin_port = htons(portno); /*convert to network byte order*/
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) /*binds a socket to an address*/
		error("ERROR on binding");

	char* result = NULL;
	int Unsupportflag = FALSE;

	for (int i=0; i<NPACK; i++) {
		memset(buffer,'\0',BUFLEN);
		if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &cli_addr, &clilen)==-1)
			error("recvfrom()");
		Sip_in *Sip_cli = in_init();
		Sip_body *body = body_init();
		result = strtok(buffer,"\n");
		int ind = 0;
			
		int bodyflag = 0;
		int DTMFflag = 0;
		int RTPflag = 0;
		char DTMF_signal;
		char *server_msg = malloc(BUFLEN);
		memset(server_msg,'\0',BUFLEN);
		char *server_msg2 = malloc(BUFLEN);
		memset(server_msg,'\0',BUFLEN);
		/*Chop the stream into sub message and store each of them into Sip_in struct*/
		while(result != NULL){
			if(ind == 0){
				if((Sip_cli->Req = realloc(Sip_cli->Req,strlen(result) + 1)) == NULL) {
    					error("realloc");
				}	
				strcpy(Sip_cli->Req,result);
			}
			else{
				if(strncmp(result,"Via:",strlen("Via:")) == 0){
					if((Sip_cli->Via = realloc(Sip_cli->Via,strlen(result) + 1)) == NULL) {
	    					error("realloc");
					}	
					strcpy(Sip_cli->Via,result);
				}
				else if(strncmp(result,"From:",strlen("From:")) == 0){
					if((Sip_cli->From = realloc(Sip_cli->From,strlen(result) + 1)) == NULL) {
	    					error("realloc");
					}	
					strcpy(Sip_cli->From,result);
				}
				else if(strncmp(result,"To:",strlen("To:")) == 0){
					if((Sip_cli->To = realloc(Sip_cli->To,strlen(result) + 1)) == NULL) {
	    					error("realloc");
					}	
					strcpy(Sip_cli->To,result);
				}
				else if(strncmp(result,"Call-ID:",strlen("Call-ID:")) == 0){
					if((Sip_cli->Call_ID = realloc(Sip_cli->Call_ID, strlen(result) + 1)) == NULL) {
	    					error("realloc");
					}	
					strcpy(Sip_cli->Call_ID,result);
				}
				else if(strncmp(result,"CSeq:",strlen("CSeq:")) == 0){
					if((Sip_cli->CSeq = realloc(Sip_cli->CSeq, strlen(result) + 1)) == NULL) {
	    					error("realloc");
					}	
					strcpy(Sip_cli->CSeq,result);
				}
				else if(strncmp(result,"Accept:",strlen("Accept:")) == 0){
					if((Sip_cli->Accept = realloc(Sip_cli->Accept, strlen(result) + 1)) == NULL) {
	    					error("realloc");
					}	
					strcpy(Sip_cli->Accept,result);
				}
				else if(strncmp(result,"Content-Type:",strlen("Content-Type:")) == 0){
					if((Sip_cli->Cnt_type = realloc(Sip_cli->Cnt_type, strlen(result) + 1)) == NULL) {
	    					error("realloc");
					}	
					strcpy(Sip_cli->Cnt_type,result);
				}
				else if(strncmp(result,"Allow:",strlen("Allow:")) == 0){
					if((Sip_cli->Allow = realloc(Sip_cli->Allow, strlen(result) + 1)) == NULL) {
	    					error("realloc");
					}	
					strcpy(Sip_cli->Allow,result);
				}
				else if(strncmp(result,"Max-Forwards:",strlen("Max-Forwards:")) == 0){
					if((Sip_cli->Max_FWD = realloc(Sip_cli->Max_FWD, strlen(result) + 1)) == NULL) {
	    					error("realloc");
					}	
					strcpy(Sip_cli->Max_FWD,result);
				}
				else if(strncmp(result,"User-Agent:",strlen("User-Agent:")) == 0){
					if((Sip_cli->UA = realloc(Sip_cli->UA, strlen(result) + 1)) == NULL) {
	    					error("realloc");
					}	
					strcpy(Sip_cli->UA,result);
				}
				else if(strncmp(result,"Subject:",strlen("Subject:")) == 0){
					if((Sip_cli->Subject = realloc(Sip_cli->Subject, strlen(result) + 1)) == NULL) {
	    					error("realloc");
					}	
					strcpy(Sip_cli->Subject,result);
				}
				else if(strncmp(result,"Expires:",strlen("Expires:")) == 0){
					if((Sip_cli->Expires = realloc(Sip_cli->Expires, strlen(result) + 1)) == NULL) {
	    					error("realloc");
					}	
					strcpy(Sip_cli->Expires,result);
				}
				else if(strncmp(result,"Content-Length:",strlen("Content-Length:")) == 0){
					if((Sip_cli->Cnt_len = realloc(Sip_cli->Cnt_len, strlen(result) + 1)) == NULL) {
	    					error("realloc");
					}	
					strcpy(Sip_cli->Cnt_len,result);
					if(strncmp(Sip_cli->Cnt_len,"Content-Length: 0",strlen("Content-Length: 0"))!= 0){
						printf("BODY FOUND\n");
						bodyflag = SUPPORT;
					}
				} /*In the case where body message is not empty, we will store them in Sip_body struct*/
				if(bodyflag == SUPPORT){
					if(strncmp(result,"v=",2) == 0){
						if((body->v = realloc(body->v, strlen(result) + 1)) == NULL) {
		    					error("realloc");
						}	
						strcpy(body->v,result);
					}
					else if(strncmp(result,"o=",2) == 0){
						if((body->o = realloc(body->o, strlen(result) + 1)) == NULL) {
		    					error("realloc");
						}	
						strcpy(body->o,result);
					}
					else if(strncmp(result,"a=rtpmap:0 PCMU/8000",strlen("a=rtpmap:0 PCMU/8000")) == 0){
						if((body->a1 = realloc(body->a1, strlen(result) + 1)) == NULL) {
		    					error("realloc");
						}	
						strcpy(body->a1,result);
						RTPflag = SUPPORT;
					}
					else if(strncmp(result,"a=rtpmap:101 telephone-event/8000",strlen("a=rtpmap:101 telephone-event/8000")) == 0){
						if((body->a2 = realloc(body->a2, strlen(result) + 1)) == NULL) {
		    					error("realloc");
						}	
						strcpy(body->a2,result);
					}
					else if(strncmp(result,"a=fmtp",strlen("a=fmtp")) == 0){
						if((body->a3 = realloc(body->a3, strlen(result) + 1)) == NULL) {
		    					error("realloc");
						}	
						strcpy(body->a3,result);
					}
					else if(strncmp(result,"Signal=",strlen("Signal=")) == 0){
						/*SOME OPRATION TO GET SIGNAL VALUE NEEDS TO BE DONE HERE*/
						
						result = strstr(result,"=");
						result = strtok(result,"=");
						DTMF_signal = result[0];
						if(DTMF_signal < '0' || DTMF_signal > '9'){
							printf("Unsupported");	
							DTMFflag = UNSUPPORT;
						}			
						printf("%c\n",DTMF_signal);
											
					}
				}

			}
			result = strtok(NULL,"\r\n");
			ind++;
		}
		/*Handle different message types, then send response out*/
		if(strncmp(Sip_cli->Req,"OPTIONS",7) == 0){
			strcpy(server_msg,OPTIONS_Handle(Sip_cli, cli_addr,server_msg2));
			if (sendto(sockfd, server_msg,strlen(server_msg), 0, (struct sockaddr *) &cli_addr, clilen)==-1)
				error("sendto");
		}
		else if(strncmp(Sip_cli->Req,"INVITE",6) == 0){
			/*If the codec is supported*/
			if(RTPflag == SUPPORT){
				strcpy(server_msg,INVITE_Handle(Sip_cli, body, cli_addr, server_msg2));
			}/*If PCMU is not supported by the client*/
			else if(RTPflag != SUPPORT){
				strcpy(server_msg,UNSUPPORT_Handle(Sip_cli, cli_addr, server_msg2));
				Unsupportflag = TRUE;
			}
			if (sendto(sockfd, server_msg,strlen(server_msg), 0, (struct sockaddr *) &cli_addr, clilen)==-1)
				error("sendto");	
		}
		else if(strncmp(Sip_cli->Req,"ACK",3) == 0 && Unsupportflag == FALSE){
			printf("ACK REC\n");
			/*THIS IS THE CASE WHEN ACK FROM 200 OK MSG IS RECEIVED, This is where we start the streaming*/
		}
		else if(strncmp(Sip_cli->Req,"ACK",3) == 0 && Unsupportflag == TRUE){
			printf("ACK Unsupported REC\n");
			/*THIS IS THE CASE WHEN ACK FROM UNSUPPORTED MSG IS RECEIVED*/
			Unsupportflag = FALSE;
		}
		else if(strncmp(Sip_cli->Req,"INFO",4) == 0){
			strcpy(server_msg,INFO_Handle(Sip_cli, cli_addr, server_msg2));
			if (sendto(sockfd, server_msg,strlen(server_msg), 0, (struct sockaddr *) &cli_addr, clilen)==-1)
				error("sendto");
			if(DTMFflag == UNSUPPORT){
				strcpy(server_msg,UNSUPPORT_Handle(Sip_cli, cli_addr, server_msg2));
				if (sendto(sockfd, server_msg,strlen(server_msg), 0, (struct sockaddr *) &cli_addr, clilen)==-1)
					error("sendto");
			}
		}
		else if(strncmp(Sip_cli->Req,"BYE",3) == 0){
			strcpy(server_msg,BYE_Handle(Sip_cli, cli_addr, server_msg2));
			if (sendto(sockfd, server_msg,strlen(server_msg), 0, (struct sockaddr *) &cli_addr, clilen)==-1)
				error("sendto");
		}
		
		free_in(Sip_cli);
		free_body(body);
		free(server_msg);
		free(server_msg2);
	}
     close(sockfd);
     return 0; 
}

char* OPTIONS_Handle(Sip_in *client, struct sockaddr_in client_addr, char* msg){

	char* result = NULL;
	char buffer[256];
	memset(buffer,'\0',256);
	Sip_out *Sip_ser = out_init();

	if((Sip_ser->Status = realloc(Sip_ser->Status, strlen("SIP/2.0 200 OK") + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->Status,"SIP/2.0 200 OK");
	
	char delims[] = "rport";
	result = strtok(client->Via,delims);
	int i = 0;
	while(result != NULL){
		if(i == 0){
			strcpy(buffer,result);
			strcat(buffer,"rport=");
			char port[6];
			memset(port,'\0',6);
			int portnum = ntohs(client_addr.sin_port);
			sprintf(port,"%d",portnum);
			port[strlen(port)] = '\0';
			strcat(buffer,port);
		}
		else if(i == 1){
			strcat(buffer,result);
			strcat(buffer,"r");
		}
		else if(i == 2){
			strcat(buffer,result);
			
		}
		result = strtok(NULL,delims);
		i++;
	}
	if((Sip_ser->Via = realloc(Sip_ser->Via, strlen(buffer) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->Via,buffer);

	if((Sip_ser->From = realloc(Sip_ser->From, strlen(client->From) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->From,client->From);

	char tag[16];
	memset(tag,'\0',16);
	sprintf(tag,";tag=%d",rand());
	if((Sip_ser->To = realloc(Sip_ser->To, strlen(client->To)+ 15 + 1)) == NULL) {
		error("realloc");
	}	
	//printf("RAND = %d",random());
	strcpy(Sip_ser->To,client->To);
	strcat(Sip_ser->To,tag);
	
	if((Sip_ser->Call_ID = realloc(Sip_ser->Call_ID, strlen(client->Call_ID) + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(Sip_ser->Call_ID,client->Call_ID);
	
	if((Sip_ser->CSeq = realloc(Sip_ser->CSeq, strlen(client->CSeq) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->CSeq,client->CSeq);
		
	if((Sip_ser->Allow = realloc(Sip_ser->Allow, strlen("Allow: INVITE,ACK,BYE,OPTIONS,INFO") + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(Sip_ser->Allow,"Allow: INVITE,ACK,BYE,OPTIONS,INFO");
	if((Sip_ser->Accept = realloc(Sip_ser->Accept, strlen(client->Accept) + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(Sip_ser->Accept,client->Accept);
	if((Sip_ser->UA = realloc(Sip_ser->UA, strlen(client->UA) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->UA,client->UA);
	if((Sip_ser->Cnt_len = realloc(Sip_ser->Cnt_len, strlen(client->Cnt_len) + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(Sip_ser->Cnt_len,client->Cnt_len);
	/*Combine all messages to 1 stream*/
	if(Sip_ser->Status != NULL){
		strcpy(msg,Sip_ser->Status);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->Via != NULL){
		strcat(msg,Sip_ser->Via);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->From != NULL){
		strcat(msg,Sip_ser->From);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->To != NULL){
		strcat(msg,Sip_ser->To);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->Call_ID != NULL){
		strcat(msg,Sip_ser->Call_ID);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->CSeq != NULL){
		strcat(msg,Sip_ser->CSeq);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->Allow != NULL){
		strcat(msg,Sip_ser->Allow);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->Accept != NULL){
		strcat(msg,Sip_ser->Accept);
		strcat(msg,"\r\n");
	}	
	if(Sip_ser->UA != NULL){
		strcat(msg,Sip_ser->UA);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->Cnt_len != NULL){
		strcat(msg,Sip_ser->Cnt_len);
		strcat(msg,"\r\n");
	}	
	strcat(msg,"\r\n");
	free_out(Sip_ser);
	return(msg);
}


char* INVITE_Handle(Sip_in *client, Sip_body *body,struct sockaddr_in client_addr,char* msg){
	//printf("IN INVITE Handles\n");

	char* result = NULL;
	char buffer[256];
	memset(buffer,'\0',256);
	Sip_out *Sip_ser = out_init();
	Sip_body *serv_body = body_init();

	if((Sip_ser->Status = realloc(Sip_ser->Status, strlen("SIP/2.0 200 OK") + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->Status,"SIP/2.0 200 OK");
	
	char delims[] = "rport";
	result = strtok(client->Via,delims);
	int i = 0;
	while(result != NULL){
		if(i == 0){
			strcpy(buffer,result);
			strcat(buffer,"rport=");
			char port[6];
			memset(port,'\0',6);
			int portnum = ntohs(client_addr.sin_port);
			sprintf(port,"%d",portnum);
			port[strlen(port)] = '\0';
			strcat(buffer,port);
		}
		else if(i == 1){
			strcat(buffer,result);
			strcat(buffer,"r");
		}
		else if(i == 2){
			strcat(buffer,result);
			
		}
		result = strtok(NULL,delims);
		i++;
	}
	if((Sip_ser->Via = realloc(Sip_ser->Via, strlen(buffer) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->Via,buffer);
	if((Sip_ser->From = realloc(Sip_ser->From, strlen(client->From) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->From,client->From);
	char tag[16];
	memset(tag,'\0',16);
	sprintf(tag,";tag=%d",rand());
	
	if((Sip_ser->To = realloc(Sip_ser->To, strlen(client->To)+ 15 + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(Sip_ser->To,client->To);
	strcat(Sip_ser->To,tag);
	
	if((Sip_ser->Call_ID = realloc(Sip_ser->Call_ID, strlen(client->Call_ID) + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(Sip_ser->Call_ID,client->Call_ID);
	
	if((Sip_ser->CSeq = realloc(Sip_ser->CSeq, strlen(client->CSeq) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->CSeq,client->CSeq);

	memset(buffer,'\0',256);
	strcpy(buffer,"Contact");
	strcat(buffer,strstr(client->To,":"));

	if((Sip_ser->Contact = realloc(Sip_ser->Contact, strlen(buffer) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->Contact,buffer);
	if((Sip_ser->Cnt_type = realloc(Sip_ser->Cnt_type, strlen(client->Cnt_type) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->Cnt_type,client->Cnt_type);

	if((Sip_ser->UA = realloc(Sip_ser->UA, strlen(client->UA) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->UA,client->UA);
	result = strtok(client->To,"@");
	result = strtok(result,":");
	result = strtok(NULL,":");
	result = strtok(NULL,":");

	memset(buffer,'\0',256);
	sprintf(buffer,"o=%s 123456 654321 IN IP4 %d",result, ntohs(client_addr.sin_port));

	if((serv_body->o = realloc(serv_body->o, strlen(buffer) + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(serv_body->o,buffer);
	
	if((serv_body->v = realloc(serv_body->v, strlen("v=0") + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(serv_body->v,"v=0");
	

	if((serv_body->s = realloc(serv_body->s, strlen("s=A conversation") + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(serv_body->s,"s=A conversation");

	memset(buffer,'\0',256);
	sprintf(buffer,"c=IN IP4 %s",inet_ntoa(client_addr.sin_addr));
	//printf("%s\n",inet_ntoa(server_addr.sin_addr));
	if((serv_body->c = realloc(serv_body->c, strlen(buffer) + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(serv_body->c,buffer);

	if((serv_body->t = realloc(serv_body->t, strlen("t=0 0") + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(serv_body->t,"t=0 0");

	if((serv_body->m = realloc(serv_body->m, strlen("m=audio 8078 RTP/AVP 0 101") + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(serv_body->m,"m=audio 8078 RTP/AVP 0 101");

	if((serv_body->a1 = realloc(serv_body->a1, strlen(body->a1) + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(serv_body->a1,body->a1);
	
	if((serv_body->a2 = realloc(serv_body->a2, strlen(body->a2) + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(serv_body->a2,body->a2);

	if((serv_body->a3 = realloc(serv_body->a3, strlen("a=fmtp:101 0-9") + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(serv_body->a3,"a=fmtp:101 0-9");

	int length = strlen(serv_body->v) + strlen(serv_body->o) + strlen(serv_body->s) + strlen(serv_body->c) + strlen(serv_body->t) +
			 strlen(serv_body->m) + strlen(serv_body->a1) + strlen(serv_body->a2) + strlen(serv_body->a3)+9*2;
	memset(buffer,'\0',256);
	sprintf(buffer,"Content-Length: %d",length);
	if((Sip_ser->Cnt_len = realloc(Sip_ser->Cnt_len, strlen(buffer) + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(Sip_ser->Cnt_len,buffer);
	/*Combine all messages to 1 stream*/
	if(Sip_ser->Status != NULL){
		strcpy(msg,Sip_ser->Status);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->Via != NULL){
		strcat(msg,Sip_ser->Via);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->From != NULL){
		strcat(msg,Sip_ser->From);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->To != NULL){
		strcat(msg,Sip_ser->To);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->Call_ID != NULL){
		strcat(msg,Sip_ser->Call_ID);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->CSeq != NULL){
		strcat(msg,Sip_ser->CSeq);
		strcat(msg,"\r\n");
	}	
	if(Sip_ser->Contact != NULL){
		strcat(msg,Sip_ser->Contact);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->Cnt_type != NULL){
		strcat(msg,Sip_ser->Cnt_type);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->UA != NULL){
		strcat(msg,Sip_ser->UA);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->Cnt_len != NULL){
		strcat(msg,Sip_ser->Cnt_len);
		strcat(msg,"\r\n");
	}	
	strcat(msg,"\r\n");
	if(serv_body->v != NULL){
		strcat(msg,serv_body->v);
		strcat(msg,"\r\n");
	}
	if(serv_body->o != NULL){
		strcat(msg,serv_body->o);
		strcat(msg,"\r\n");
	}
	if(serv_body->s != NULL){
		strcat(msg,serv_body->s);
		strcat(msg,"\r\n");
	}
	if(serv_body->c != NULL){
		strcat(msg,serv_body->c);
		strcat(msg,"\r\n");
	}
	if(serv_body->t != NULL){
		strcat(msg,serv_body->t);
		strcat(msg,"\r\n");
	}
	if(serv_body->m != NULL){
		strcat(msg,serv_body->m);
		strcat(msg,"\r\n");
	}
	if(serv_body->a1 != NULL){
		strcat(msg,serv_body->a1);
		strcat(msg,"\r\n");
	}
	if(serv_body->a2 != NULL){
		strcat(msg,serv_body->a2);
		strcat(msg,"\r\n");
	}
	if(serv_body->a3 != NULL){
		strcat(msg,serv_body->a3);
		strcat(msg,"\r\n");
	}
	strcat(msg,"\r\n");
	free_out(Sip_ser);
	free_body(serv_body);
	return(msg);
}
char* UNSUPPORT_Handle(Sip_in *client, struct sockaddr_in client_addr,char* msg){
	char* result = NULL;
	char buffer[256];
	memset(buffer,'\0',256);
	Sip_out *Sip_ser = out_init();

	if((Sip_ser->Status = realloc(Sip_ser->Status, strlen("SIP/2.0 415 Unsupported Media Type") + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->Status,"SIP/2.0 415 Unsupported Media Type");
	
	char delims[] = "rport";
	result = strtok(client->Via,delims);
	int i = 0;
	while(result != NULL){
		if(i == 0){
			strcpy(buffer,result);
			strcat(buffer,"rport=");
			char port[6];
			memset(port,'\0',6);
			int portnum = ntohs(client_addr.sin_port);
			sprintf(port,"%d",portnum);
			port[strlen(port)] = '\0';
			strcat(buffer,port);
		}
		else if(i == 1){
			strcat(buffer,result);
			strcat(buffer,"r");
		}
		else if(i == 2){
			strcat(buffer,result);
			
		}
		result = strtok(NULL,delims);
		i++;
	}
	if((Sip_ser->Via = realloc(Sip_ser->Via, strlen(buffer) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->Via,buffer);
	if((Sip_ser->From = realloc(Sip_ser->From, strlen(client->From) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->From,client->From);
	char tag[16];
	memset(tag,'\0',16);
	sprintf(tag,";tag=%d",rand());
	if((Sip_ser->To = realloc(Sip_ser->To, strlen(client->To)+ 15 + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(Sip_ser->To,client->To);
	strcat(Sip_ser->To,tag);
	
	if((Sip_ser->Call_ID = realloc(Sip_ser->Call_ID, strlen(client->Call_ID) + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(Sip_ser->Call_ID,client->Call_ID);
	
	if((Sip_ser->CSeq = realloc(Sip_ser->CSeq, strlen(client->CSeq) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->CSeq,client->CSeq);
		
	if((Sip_ser->UA = realloc(Sip_ser->UA, strlen(client->UA) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->UA,client->UA);
	if((Sip_ser->Cnt_len = realloc(Sip_ser->Cnt_len, strlen("Content-Length: 0") + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(Sip_ser->Cnt_len,"Content-Length: 0");
	/*Combine all messages to 1 stream*/
	if(Sip_ser->Status != NULL){
		strcpy(msg,Sip_ser->Status);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->Via != NULL){
		strcat(msg,Sip_ser->Via);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->From != NULL){
		strcat(msg,Sip_ser->From);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->To != NULL){
		strcat(msg,Sip_ser->To);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->Call_ID != NULL){
		strcat(msg,Sip_ser->Call_ID);
	}
	if(Sip_ser->CSeq != NULL){
		strcat(msg,Sip_ser->CSeq);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->UA != NULL){
		strcat(msg,Sip_ser->UA);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->Cnt_len != NULL){
		strcat(msg,Sip_ser->Cnt_len);
		strcat(msg,"\r\n");
	}	
	strcat(msg,"\r\n");
	free_out(Sip_ser);
	return(msg);
}

/*int ACK_Handle();*/
char* INFO_Handle(Sip_in *client, struct sockaddr_in client_addr,char* msg){

	char* result = NULL;
	char buffer[256];
	memset(buffer,'\0',256);
	Sip_out *Sip_ser = out_init();

	if((Sip_ser->Status = realloc(Sip_ser->Status, strlen("SIP/2.0 200 OK") + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->Status,"SIP/2.0 200 OK");
	
	char delims[] = "rport";
	result = strtok(client->Via,delims);
	int i = 0;
	while(result != NULL){
		if(i == 0){
			strcpy(buffer,result);
			strcat(buffer,"rport=");
			char port[6];
			memset(port,'\0',6);
			int portnum = ntohs(client_addr.sin_port);
			sprintf(port,"%d",portnum);
			port[strlen(port)] = '\0';
			strcat(buffer,port);
		}
		else if(i == 1){
			strcat(buffer,result);
			strcat(buffer,"r");
		}
		else if(i == 2){
			strcat(buffer,result);
			
		}
		result = strtok(NULL,delims);
		i++;
	}
	if((Sip_ser->Via = realloc(Sip_ser->Via, strlen(buffer) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->Via,buffer);
	if((Sip_ser->From = realloc(Sip_ser->From, strlen(client->From) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->From,client->From);
	
	if((Sip_ser->To = realloc(Sip_ser->To, strlen(client->To)+ 1)) == NULL) {
		error("realloc");
	}	
	strcpy(Sip_ser->To,client->To);
	
	if((Sip_ser->Call_ID = realloc(Sip_ser->Call_ID, strlen(client->Call_ID) + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(Sip_ser->Call_ID,client->Call_ID);
	
	if((Sip_ser->CSeq = realloc(Sip_ser->CSeq, strlen(client->CSeq) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->CSeq,client->CSeq);

	memset(buffer,'\0',256);
	strcpy(buffer,"Contact");
	strcat(buffer,strtok(strstr(client->To,":"),";"));

	if((Sip_ser->Contact = realloc(Sip_ser->Contact, strlen(buffer) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->Contact,buffer);

	if((Sip_ser->UA = realloc(Sip_ser->UA, strlen(client->UA) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->UA,client->UA);
	if((Sip_ser->Cnt_len = realloc(Sip_ser->Cnt_len, strlen("Content-Length: 0") + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(Sip_ser->Cnt_len,"Content-Length: 0");
	/*Combine all messages to 1 stream*/
	if(Sip_ser->Status != NULL){
		strcpy(msg,Sip_ser->Status);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->Via != NULL){
		strcat(msg,Sip_ser->Via);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->From != NULL){
		strcat(msg,Sip_ser->From);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->To != NULL){
		strcat(msg,Sip_ser->To);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->Call_ID != NULL){
		strcat(msg,Sip_ser->Call_ID);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->CSeq != NULL){
		strcat(msg,Sip_ser->CSeq);
		strcat(msg,"\r\n");
	}	
	if(Sip_ser->Contact != NULL){
		strcat(msg,Sip_ser->Contact);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->UA != NULL){
		strcat(msg,Sip_ser->UA);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->Cnt_len != NULL){
		strcat(msg,Sip_ser->Cnt_len);
		strcat(msg,"\r\n");
	}	
	strcat(msg,"\r\n");
	free_out(Sip_ser);
	return(msg);
}
char* BYE_Handle(Sip_in *client, struct sockaddr_in client_addr,char* msg){
	printf("IN BYE Handle\n");
	char* result = NULL;
	char buffer[256];

	memset(buffer,'\0',256);
	Sip_out *Sip_ser = out_init();

	if((Sip_ser->Status = realloc(Sip_ser->Status, strlen("SIP/2.0 200 OK") + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->Status,"SIP/2.0 200 OK");
	
	char delims[] = "rport";
	result = strtok(client->Via,delims);
	int i = 0;
	while(result != NULL){
		if(i == 0){
			strcpy(buffer,result);
			strcat(buffer,"rport=");
			char port[6];
			memset(port,'\0',6);
			int portnum = ntohs(client_addr.sin_port);
			sprintf(port,"%d",portnum);
			port[strlen(port)] = '\0';
			strcat(buffer,port);
		}
		else if(i == 1){
			strcat(buffer,result);
			strcat(buffer,"r");
		}
		else if(i == 2){
			strcat(buffer,result);
			
		}
		result = strtok(NULL,delims);
		i++;
	}
	if((Sip_ser->Via = realloc(Sip_ser->Via, strlen(buffer) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->Via,buffer);
	if((Sip_ser->From = realloc(Sip_ser->From, strlen(client->From) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->From,client->From);

	
	if((Sip_ser->To = realloc(Sip_ser->To, strlen(client->To) + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(Sip_ser->To,client->To);
	if((Sip_ser->Call_ID = realloc(Sip_ser->Call_ID, strlen(client->Call_ID) + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(Sip_ser->Call_ID,client->Call_ID);
	
	if((Sip_ser->CSeq = realloc(Sip_ser->CSeq, strlen(client->CSeq) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->CSeq,client->CSeq);

	if((Sip_ser->UA = realloc(Sip_ser->UA, strlen(client->UA) + 1)) == NULL) {
		error("realloc");
	}
	strcpy(Sip_ser->UA,client->UA);
	if((Sip_ser->Cnt_len = realloc(Sip_ser->Cnt_len, strlen(client->Cnt_len) + 1)) == NULL) {
		error("realloc");
	}	
	strcpy(Sip_ser->Cnt_len,client->Cnt_len);
	/*Combine all messages to 1 stream*/
	if(Sip_ser->Status != NULL){
		strcpy(msg,Sip_ser->Status);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->Via != NULL){
		strcat(msg,Sip_ser->Via);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->From != NULL){
		strcat(msg,Sip_ser->From);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->To != NULL){
		strcat(msg,Sip_ser->To);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->Call_ID != NULL){
		strcat(msg,Sip_ser->Call_ID);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->CSeq != NULL){
		strcat(msg,Sip_ser->CSeq);
		strcat(msg,"\r\n");
	}	
	if(Sip_ser->UA != NULL){
		strcat(msg,Sip_ser->UA);
		strcat(msg,"\r\n");
	}
	if(Sip_ser->Cnt_len != NULL){
		strcat(msg,Sip_ser->Cnt_len);
		strcat(msg,"\r\n");
	}	
	strcat(msg,"\r\n");
	free_out(Sip_ser);
	return(msg);
}

/*Initialize struct Sip_in*/
Sip_in *in_init(void){
	Sip_in *stream;
	if((stream = malloc(BUFLEN)) == NULL) {
		error("realloc");
	}
	stream->Req = NULL;
	stream->Via = NULL;
	stream->From = NULL;
	stream->To = NULL;
	stream->Call_ID = NULL;
	stream->CSeq = NULL;
	stream->Accept = NULL;
	stream->Cnt_type = NULL;
	stream->Allow = NULL;
	stream->Max_FWD = NULL;
	stream->UA = NULL;
	stream->Subject = NULL;
	stream->Expires = NULL;
	stream->Cnt_len = NULL;
	stream->Msg_body = NULL;
	return(stream);
}

/*Initialize struct Sip_out*/
Sip_out *out_init(void){
	Sip_out *stream;
	if((stream = malloc(BUFLEN)) == NULL) {
		error("realloc");
	}
	stream->Status = NULL;
	stream->Via = NULL;
	stream->From = NULL;
	stream->To = NULL;
	stream->Call_ID = NULL;
	stream->CSeq = NULL;
	stream->Allow = NULL;
	stream->Accept = NULL;
	stream->Contact = NULL;
	stream->Cnt_type = NULL;
	stream->UA = NULL;
	stream->Cnt_len = NULL;
	return(stream);
}

/*Initialize struct Sip_body*/
Sip_body *body_init(void){
	Sip_body *stream;
	if((stream = malloc(BUFLEN)) == NULL) {
		error("realloc");
	}
	stream->v = NULL;
	stream->o = NULL;
	stream->s = NULL;
	stream->c = NULL;
	stream->t = NULL;
	stream->m = NULL;
	stream->a1 = NULL;
	stream->a2 = NULL;
	stream->a3 = NULL;
	return(stream);
}

/*free struct Sip_in*/
void free_in(Sip_in* stream){
	free(stream->Req);
	free(stream->Via);
	free(stream->From);
	free(stream->To);
	free(stream->Call_ID);
	free(stream->CSeq);
	free(stream->Accept);
	free(stream->Cnt_type);
	free(stream->Allow);
	free(stream->Max_FWD);
	free(stream->UA);
	free(stream->Subject);
	free(stream->Expires);
	free(stream->Cnt_len);
	free(stream->Msg_body);
	free(stream);
}

/*free struct Sip_out*/
void free_out(Sip_out* stream){

	free(stream->Status);
	free(stream->Via);
	free(stream->From);
	free(stream->To);
	free(stream->Call_ID);
	free(stream->CSeq);
	free(stream->Allow);
	free(stream->Accept);
	free(stream->Contact);
	free(stream->Cnt_type);
	free(stream->UA);
	free(stream->Cnt_len);
	free(stream);
}

/*free struct Sip_body*/
void free_body(Sip_body* stream){

	free(stream->v);
	free(stream->o);
	free(stream->s);
	free(stream->c);
	free(stream->t);
	free(stream->m);
	free(stream->a1);
	free(stream->a2);
	free(stream->a3);
	free(stream);
}
