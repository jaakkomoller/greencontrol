#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "rtp_server.h"
 
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

	/* This must be the last field! */
	unsigned char payload[];
};

/*
 * Allocates a variable size packet
 * 
 * @arg struct rtp_packet **packet: double pointer to point into allocated  
 * 		packet
 * @arg size_t payload_length: length of the payload in bytes
 * @ret: -1 if allocation fails, payload_length otherwise
 */

static int create_rtp_packet(struct rtp_packet **packet, size_t payload_length) {
	*packet = (struct rtp_packet *) malloc(RTP_HEADER_SIZE + payload_length * sizeof(char));
	return (*packet != NULL) ? payload_length : -1; 
}


static int init_rtp_packet(struct rtp_packet *packet, unsigned seq_no, unsigned long timestamp, unsigned long ssrc) {
	
	packet->version = 2;
	packet->padding = 0;
	packet->extension = 0;
	packet->csrc_counter = 0;
	packet->marker = 0;

	/* PCMU type code = 0, see http://tools.ietf.org/html/rfc3551#page-32 */
	packet->payload_type = 0;
	packet->seq_no = seq_no;

	packet->timestamp = timestamp;
	packet->ssrc_id = ssrc;
	return 0;
}

/*
 * TODO: proper random, see:
 * http://tools.ietf.org/html/rfc3550#appendix-A.6
 */

static unsigned long get_ssrc() {
	unsigned long ret;
	int *random = (int *)&ret;
	
	srand(time(NULL));
	random[0] = rand();
	random[1] = rand();

	return ret;
}

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

