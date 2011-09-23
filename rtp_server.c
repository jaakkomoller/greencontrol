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

struct rtp_packet {
	unsigned version :2;
	unsigned padding :1;
	unsigned extension :1;
	unsigned csrc_counter :4;
	unsigned marker :1;
	unsigned payload_type :7;
	unsigned seq_no :16;

	unsigned timestamp :32;
	unsigned ssrc_id :32;
};

static int init_rtp_packet(struct rtp_packet *packet) {
	return 0;
}
