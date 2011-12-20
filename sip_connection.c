#include "sip_connection.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Linked list */

void append(struct node **q, struct connection *conn)
{
	struct node *temp,*r;
	temp = *q;
	if(*q==NULL)
	{
		temp = (struct node *) malloc( sizeof( struct node));
		memcpy(&temp->data, conn, sizeof(struct connection));
		temp->link = NULL;
		*q = temp;
	}
	else
	{
		temp = *q;
		while(temp->link !=NULL)
		{
			temp = temp->link;
		}
		r = (struct node *) malloc( sizeof(struct node));
		memcpy(&r->data, conn, sizeof(struct connection));
		r->link=NULL;
		temp->link=r;
	}
}

void display(struct node *q)
{
	if(q==NULL)
	{
		goto last;
	}
	while(q!=NULL)
	{
		printf("%s\n",q->data.sip_conn->Call_ID);
		q=q->link;
	}
last:
	return;
}

int count(struct node *q)
{
	int c=0;
	if(q==NULL)
	{
		goto last;
	}
	while(q!=NULL)
	{
		c++;
		q=q->link;
	}
last:
	return c;
}

void in_begin(struct node **q, struct connection *conn)
{
	struct node *temp;
	if(*q==NULL)
	{
		goto last;
	}
	else
	{   temp=(struct node *)malloc(sizeof(struct node));
		memcpy(&temp->data, conn, sizeof(struct connection));
		temp->link=*q;
		*q=temp;  /* pointing to the first node */
	}
last:
	return;
}

void in_middle(struct node **q, int loc, struct connection *conn)
{  
	struct node *temp,*n;
	int c=1,flag=0;
	temp=*q;
	if(*q==NULL)
	{
		goto last;
	}
	else
		while(temp!=NULL)
		{
			if(c==loc)
			{
				n = (struct node *)malloc(sizeof(struct node));
				memcpy(&n->data, conn, sizeof(struct connection));
				n->link=temp->link;
				temp->link=n;
				flag=1;
			}
			c++;
			temp=temp->link;
		}
	if(flag==0)
	{
		//printf("\n\nNode Specified Doesn't Exist.Cant Enter The Data");
	}
	else
	{
		//printf("Data Inserted");
	}
last:
	return;
}

void del(struct node**q, char *call_id)
{    
	if(*q==NULL)
	{
		goto last;
	}
	else
	{
		struct node *old,*temp;
		int flag=0;
		temp=*q;
		while(temp!=NULL)
		{
			if(strcmp(temp->data.sip_conn->Call_ID, call_id) == 0)
			{
				if(temp==*q)         /* First Node case */
					*q=temp->link;  /* shifted the header node */
				else
					old->link=temp->link;

				free_in(temp->data.sip_conn);
				free(temp);
				temp = NULL;
				flag=1;
			}
			else
			{
				old=temp;
				temp=temp->link;
			}
		}
		//if(flag==0)
			//printf("Data Not Found...\n");
		//else
			//printf("Data Deleted...Tap a key to continue\n");
	}
last:
	return;
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

