#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "util.h"



/*
 * TODO: proper random, see:
 * http://tools.ietf.org/html/rfc3550#appendix-A.6
 */

unsigned long random32() {
	unsigned long ret;
	int *random = (int *)&ret;
	
	srand(time(NULL));
	random[0] = rand();
	random[1] = rand();

	return ret;
}

int parse_opts(int argc, char **argv, struct cl_options *opt) {
	
	int error = 0;
	int c;
	char *temp;

	opt->rtptest = 0;
	opt->addresses = 0;

	while ((c = getopt (argc, argv, "u:p:c:r")) != -1 && error == 0) {
printf("parsing %i\n", c);
printf("print %i\n", 'r');
		switch (c)
		{
		case 'r':
;printf("Setting rtptest\n");
			;opt->rtptest = 1;
			break;
		case 'u':
			printf("Option not implemented\n");
			break;
		case 'p':
			printf("Option not implemented\n");
			break;
		case 'c':
;printf("Setting IP-address\n");
			;char *ipaddr, *port, *temp;
			int i = 0;
			
			temp = optarg;
			temp = strtok(temp, ",");
			for(i = 0; temp != NULL; temp = strtok(NULL, ","), i++) {
				ipaddr = temp;
				port = strtok(NULL, ",");

				if(port == NULL) {
					fprintf (stderr, "Error while parsing ip address.\n");
					error = 1;
					goto exit;
				}
			
				printf("inserting address %s:%s\n", ipaddr, port);	
				strcpy(opt->destsarray[i], ipaddr);
				strcpy(opt->portsarray[i], port);
			}
			opt->addresses = i;
			break;
		case '?':
			if (optopt == 'u' || optopt == 'p' || optopt == 'c')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint (optopt))
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf (stderr,
					"Unknown option character `\\x%x'.\n",
					optopt);
				return 1;
		default:
			fprintf(stderr, "Should not reach here.\n");
			break;
		}
	}
exit:
	return error;
}
