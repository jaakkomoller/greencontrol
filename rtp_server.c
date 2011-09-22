#include <stdio.h>
#include <stdlib.h>
 
int rtp_server(FILE *infile, void *dest_addr, int count, int af_family) {
	char fname[] = "test_data/original.au";
	char line[100];
	int readchars = 0;
 
	/* Open the file.  If NULL is returned there was an error */
	if((infile = fopen(fname, "r")) == NULL) {
		printf("Error Opening File.\n");
		exit(1);
	}
  
	readchars = fread (line, 1, 100, infile);
	while(readchars != 0) {
		printf("read line with %d chars\n", readchars);  
		readchars = fread (line, 1, 100, infile);
	}
 
	fclose(infile);  /* Close the file */
	return 0;
}


