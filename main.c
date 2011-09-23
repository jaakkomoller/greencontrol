#include <stdio.h>
#include <stdlib.h>
#include "rtp_server.h"

int main(int argc, char *argv[]) {

	FILE *soundfile;
	char fname[] = "test_data/original.au";

	/* Open the soundfile. Now the sound data passed to rtp_server does
	 * not include HTTP frames, as it will on at some point... */
	if((soundfile = fopen(fname, "r")) == NULL) {
		printf("Error Opening File.\n");
		exit(1);
	}
  
	/*
	 * A hack to start rtptest with "./RadioStreamer rtptest".
	 * 
	 * Aim and Tomi: you can add your own test functions in the same
	 * manner (, or then in an other way, what ever is your thing...).
	 */
	if(argc == 2 && strcmp(argv[1], "rtptest") == 0)
		rtp_server(soundfile, stdin, 1500, NULL, 0, 4);
		

	fclose(soundfile);

	return 0;
}
