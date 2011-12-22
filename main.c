#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include "rtp_connection.h"
#include "mp3fetcher.h"
#include "Transcoder.h"

int main(int argc, char *argv[]) {

	char stations[MAX_STATIONS][100];
	int station_count = fetch_station_info(stations, MAX_STATIONS);
	int err = 0;
	struct cl_options opt;


	err = parse_opts(argc, argv, &opt);
	if(err != 0) {
		goto exit_err;
	}


	sip_server_kick(stations, station_count, 50000, opt.family);

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
