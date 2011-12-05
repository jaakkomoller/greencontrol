#ifndef RTP_CONNECTION_H
#define RTP_CONNECTION_H

#include "util.h"
#include <netinet/in.h>

struct destination {
	int addr_family;
	int length;
	union {
		struct sockaddr_in addr_in;
		struct sockaddr_in6 addr_in6;
	} addr;
};

struct rtp_connection {
	int bind_sk;
	int data_input;

	struct timeval send_interval;
	unsigned int sampling_freq; 
	unsigned int sample_size; 
	unsigned int header_size;

	unsigned int seq_no;
	unsigned long ssrc, timestamp;

	struct destination *destinations;
	int howmany;
};

int init_rtp_connection(struct rtp_connection *connection,
		unsigned long send_interval_sec,
		unsigned long send_interval_usec,
		unsigned int sampling_freq, unsigned int sample_size,
		int data_input);
int set_destinations(char addr[][MAX_IPV4_ADDR_LEN], char ports[][MAX_IPV4_ADDR_LEN],
		struct rtp_connection *connection, int howmany);

/*
 * Reads the U-law encoded data and sends to clients. The soundfile is
 * expected to include HTTP headers, so we need a function that parses
 * those out. Only IPv4 support initially. Control data will be taken
 * in UDP.
 *
 * @arg soundfile: File pointer to read incoming mp3 from HTTP connected sk
 * @arg input: stdin
 * @arg control_port: Port to bind to get control packets (extra)
 * @arg dest_addr: list of destination addresses
 * @arg count: amount of destination addresses
 * @arg af_family: To which address family are we sending to
 *
 */
int rtp_connection_kick(struct rtp_connection *connection);

int free_rtp_connection(struct rtp_connection *connection);
#endif /* RTP_CONNECTION_H */
