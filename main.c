#include <stdio.h>
#include <stdlib.h>
#include "rtp_connection.h"
#include "mp3fetcher.h"
#include "util.h"

int main(int argc, char *argv[]) {

	int err = 0;
	int rtp_server_pipe[2]; // This pipes data from converter to rto server
	struct rtp_connection rtp_connection; // The RTP connection object

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
		char *dests[2];
		char *ports[2];
		char destsarray[][100] = {{"localhost"}, {"localhost"}};
		char portsarray[][100] = {{"1500"}, {"2000"}};
		
		dests[0] = &destsarray[0][0];
		ports[0] = &portsarray[0][0];
		dests[1] = &destsarray[1][0];
		ports[1] = &portsarray[1][0];

		int sock = 0;
		struct sockaddr_in dest;

		/* TODO Handle errors.. */

		/* Open the soundfile. The sound data should not contain
		 * HTTP headers. */
		if((soundfile = fopen(fname, "r")) == NULL) {
			err = -1;
			goto exit_system_err;
		}
		
		set_destinations(dests, ports, &rtp_connection, 2);
		init_rtp_connection(&rtp_connection, RTP_SEND_INTERVAL_SEC,
			RTP_SEND_INTERVAL_USEC, RTP_SAMPLING_FREQ,
			SAMPLE_SIZE, fileno(soundfile));

		// TODO fork within this function..
		rtp_connection_kick(&rtp_connection);
		
		free_rtp_connection(&rtp_connection);

		fclose(soundfile);
	}

	if(argc == 2 && strcmp(argv[1], "fetcher") == 0) {
	fetch_station_info();

	}


exit_system_err:
	if(err != 0)
		perror("Error with syscalls: ");
exit:
	return 0;
}
