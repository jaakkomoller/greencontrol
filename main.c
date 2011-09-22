#include <stdio.h>
#include <stdlib.h>
#include "rtp_server.h"

int main(int argc, char *argv[]) {


	/* A hack to start rtptest with "./RadioStreamer rtptest" */
	if(argc == 2 && strcmp(argv[1], "rtptest") == 0)
		rtp_server(NULL, NULL, 0, 4);
		

	return 0;
}
