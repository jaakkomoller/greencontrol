#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/file.h> /* O_CREAT jne*/
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
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
	opt->family = AF_INET;

	while ((c = getopt (argc, argv, "6")) != -1 && error == 0) {
		switch (c)
		{
			case '6':
				printf("ipv6\n");
				opt->family = AF_INET6;
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

int flush_file(int file) {
	int bytes_available, err = 0;
	char buf[100];
	
	// Check amount of data in pipe
	err = ioctl(file, FIONREAD, &bytes_available);
	if(err != 0) {
		perror("Error while checking size of pipe: ");
		goto exit;
	}

	while(bytes_available != 0) {
		read(file, buf, (100 < bytes_available) ? 100 : bytes_available);
		// Check amount of data in pipe
		err = ioctl(file, FIONREAD, &bytes_available);
		if(err != 0) {
			perror("Error while checking size of pipe: ");
			goto exit;
		}
	}
	exit:
	return err;
}
