#include <stdio.h>
#include <stdlib.h>
#include "rtp_connection.h"
#include "util.h"

int main(int argc, char *argv[]) {

	int err = 0;
	int rtp_server_pipe[2]; // This pipes data from converter to rto server
	struct rtp_connection rtp_output; // The RTP connection object

	int fd[2];
	err = pipe(fd);
	if(err != 0) {
		goto exit_system_err;
	} 

	/*
	 * A hack to start rtptest with "./RadioStreamer rtptest".
	 * 
	 * Aim and Tomi: you can add your own test functions in the same
	 * manner (, or then in an other way, what ever is your thing...).
	 */
	if(argc == 2 && strcmp(argv[1], "rtptest") == 0) {
		FILE *soundfile;
		char fname[] = "test_data/original.au";
		struct rtp_connection connection;

		int sock = 0;
		struct sockaddr_in dest;

		/* TODO Handle errors.. */

		/* Open the soundfile. The sound data should not contain
		 * HTTP headers. */
		if((soundfile = fopen(fname, "r")) == NULL) {
			err = -1;
			goto exit_system_err;
		}
		/* TODO Add support for multiple destinations */
		init_rtp_connection("localhost", "1500", 1,
			&connection, RTP_SEND_INTERVAL_SEC,
			RTP_SEND_INTERVAL_USEC, RTP_SAMPLING_FREQ,
			SAMPLE_SIZE, fileno(soundfile));

		rtp_connection_kick(&connection);
		
		free_rtp_connection(&connection);

		fclose(soundfile);
	}



exit_system_err:
	if(err != 0)
		perror("Error with syscalls: ");
exit:
	return 0;
}
