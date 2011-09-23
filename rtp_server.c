#include <stdio.h>
#include <stdlib.h>
 
int rtp_server(FILE *soundfile, FILE *input, int control_port, void *dest_addr, int count, int af_family) {
	char line[100];
	int readchars = 0;
 
	readchars = fread (line, 1, 100, soundfile);
	while(readchars != 0) {
		printf("read line with %d chars\n", readchars);  
		readchars = fread (line, 1, 100, soundfile);
	}
 
	return 0;
}


