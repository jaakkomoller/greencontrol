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
		printf("\n\nEmpty Link List.Can't Display The Data");
		goto last;
	}
	while(q!=NULL)
	{
//		printf("\n%d",q->data);
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
		printf("Empty Link List.\n");
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
		printf("Link List Is Empty.Can't Insert.");
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
		printf("\n\nLink List Is Empty.Can't Insert.");
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
		printf("\n\nNode Specified Doesn't Exist.Cant Enter The Data");
	}
	else
	{
		printf("Data Inserted");
	}
last:
	return;
}

void del(struct node**q, char *call_id)
{    
	if(*q==NULL)
	{
		printf("\n\nEmpty Linked List.Cant Delete The Data.");
		goto last;
	}
	else
	{
		struct node *old,*temp;
		int flag=0;
		temp=*q;
		while(temp!=NULL)
		{
			if(strcmp(temp->data.sip_conn.Call_ID, call_id))
			{
				if(temp==*q)         /* First Node case */
					*q=temp->link;  /* shifted the header node */
				else
					old->link=temp->link;

				free(temp);
				flag=1;
			}
			else
			{
				old=temp;
				temp=temp->link;
			}
		}
		if(flag==0)
			printf("\nData Not Found...");
		else
			printf("\nData Deleted...Tap a key to continue");
	}
last:
	return;
}
