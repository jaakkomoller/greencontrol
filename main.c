#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include "rtp_connection.h"
#include "mp3fetcher.h"
#include "Transcoder.h"

int main(int argc, char *argv[]) {

	int err = 0;
	int rtp_server_pipe[2]; // This pipes data from transcoder to rtp server
	int transcoder_pipe[2]; // This pipes data from mp3fetcher to transcoder
	int transcoder_control_pipe[2]; // This pipes control data to transcoder
	int rtp_server_control_pipe[2]; // This pipes control data to rtp server
	int mp3_fetcher_control_pipe[2]; // This pipes control data to mp3 fetcher
	struct rtp_connection rtp_connection; // The RTP connection object
	struct cl_options opt; // Command line options
	int state = RUNNING; 

	err = parse_opts(argc, argv, &opt);
	if(err != 0) {
		goto exit_err;
	}

	err = pipe(mp3_fetcher_control_pipe);
	if(err != 0) {
		goto exit_system_err;
	} 

	err = pipe(rtp_server_pipe);
	if(err != 0) {
		goto exit_system_err;
	} 

	err = pipe(transcoder_pipe);
	if(err != 0) {
		goto exit_system_err;
	} 

	err = pipe(transcoder_control_pipe);
	if(err != 0) {
	goto exit_system_err;
	}

	err = pipe(rtp_server_control_pipe);
	if(err != 0) {
	goto exit_system_err;
	} 

	if(fork() == 0) {

		// Transcoder's thread
		int fd;
		fd = open("warning.txt", O_TRUNC | O_WRONLY | O_CREAT, S_IRWXU);
		dup2(fd, 2); // Output warning data to warning.txt

		dup2(transcoder_control_pipe[0], fileno(stdin)); // Control data from stdin

		struct transcoder_data coder;
		init_transcoder();
		init_transcoder_data(transcoder_pipe[0], rtp_server_pipe[1], mp3_fetcher_control_pipe[1], &coder);
		audio_transcode(&coder, &state);
		close(fd);

		printf("Transcoder quitting\n");
		char *temp = "E\n";
		write(rtp_server_control_pipe[1], temp, 2);

		exit(0);
	}
	if(fork() == 0) {
		// RTP server's thread
		set_destinations(opt.destsarray, opt.portsarray, &rtp_connection, opt.addresses);
		init_rtp_connection(&rtp_connection, RTP_SEND_INTERVAL_SEC,
				RTP_SEND_INTERVAL_USEC, RTP_SAMPLING_FREQ,
				SAMPLE_SIZE, rtp_server_pipe[0]);
		
		dup2(rtp_server_control_pipe[0], fileno(stdin)); // Control data from stdin

		rtp_connection_kick(&rtp_connection, &state);

		free_rtp_connection(&rtp_connection);
	
		printf("RTP server quitting\n");
		exit(0);
	}

	// MP3 fetcher's thread (Also UI)
	char stations[MAX_STATIONS][100];
	int station_count = fetch_station_info(stations, MAX_STATIONS);

	start_gui(transcoder_pipe[1], transcoder_control_pipe[1], mp3_fetcher_control_pipe[0], &state, stations, station_count);

	fprintf(stderr, "MP3 fetcher quitting\n");
	char *temp = "E\n";
	write(transcoder_control_pipe[1], temp, 2);
	wait();
	wait();

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
