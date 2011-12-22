/*
 *  time.c
 *  
 *
 *  Created by Sarantorn Bisalbutra on 12/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "timing.h"


void print_time(const char* message)
{
	char buffer[30];
	struct timeval tv;
	
	time_t curtime;
	
	
	gettimeofday(&tv, NULL); 
	curtime=tv.tv_sec;
	
	strftime(buffer,30,"%m-%d-%Y  %T.",localtime(&curtime));
	fprintf(stderr, "%s:%s%ld\n",message,buffer,tv.tv_usec);
	
	
}
