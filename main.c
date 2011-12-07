#include <stdio.h>
#include <stdlib.h>
#include "rtp_connection.h"
#include "mp3fetcher.h"
#include "Transcoder.h"

int main(int argc, char *argv[]) {

	int err = 0;
	int rtp_server_pipe[2]; // This pipes data from converter to rto server
	struct rtp_connection rtp_connection; // The RTP connection object
	struct cl_options opt;

	err = parse_opts(argc, argv, &opt);
	if(err != 0) {
		goto exit_err;
	}

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
	if(opt.rtptest) {
		FILE *soundfile;
//		char fname[] = "test_data/original.au";
		char fname[] = "test_data/test.mp3";

		int sock = 0;
		struct sockaddr_in dest;


		/* TODO Handle errors.. */

		/* Open the soundfile. The sound data should not contain
		 * HTTP headers. */
		if((soundfile = fopen(fname, "r")) == NULL) {
			err = -1;
			goto exit_system_err;
		}
		
		init_transcoder(fileno(soundfile));
		
		/*
	int addresses;
	char destsarray[MAX_IPV4_ADDRS][MAX_IPV4_ADDRS];
	char portsarray[MAX_IPV4_ADDRS][MAX_IPV4_ADDRS];
*/
		
		audio_transcode(rtp_server_pipe[0]);
		set_destinations(opt.destsarray, opt.portsarray, &rtp_connection, opt.addresses);
		init_rtp_connection(&rtp_connection, RTP_SEND_INTERVAL_SEC,
			RTP_SEND_INTERVAL_USEC, RTP_SAMPLING_FREQ,
			SAMPLE_SIZE, rtp_server_pipe[1]);

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

exit_err:
	if(err != 0)
		perror("Error: ");
	return 0;
}
