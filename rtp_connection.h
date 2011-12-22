#ifndef RTP_CONNECTION_H
#define RTP_CONNECTION_H

#include "util.h"
#include <netinet/in.h>

struct destination {
	int length;
	struct sockaddr_storage addr;
};

struct rtp_connection {
	int bind_sk4, bind_sk6;
	int data_input;

	struct timeval send_interval;
	unsigned int sampling_freq; 
	unsigned int sample_size; 
	unsigned int header_size;

	unsigned int seq_no;
	unsigned long ssrc, timestamp;

	struct destination *destinations;
	int howmany;
	int have_ipv6, have_ipv4;
};

int init_rtp_connection(struct rtp_connection *connection,
		unsigned long send_interval_sec,
		unsigned long send_interval_usec,
		unsigned int sampling_freq, unsigned int sample_size,
		int data_input);
int set_destinations(char addr[][MAX_IPV4_ADDR_LEN], char ports[][MAX_IPV4_ADDR_LEN],
		struct rtp_connection *connection, int howmany);

int rtp_connection_kick(struct rtp_connection *connection);

int free_rtp_connection(struct rtp_connection *connection);
#endif /* RTP_CONNECTION_H */
