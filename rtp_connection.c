#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "rtp_connection.h"
#include "rtp_packet.h"
#include "util.h"


static int bind_udp(int *sock);
static int free_udp(int sock);
int parse_destination(char *addr, char *port, struct destination *dest);
static int get_payload_size(struct rtp_connection *connection);
static unsigned long get_timestamp_per_packet_inc(
		struct rtp_connection *connection);
 
int init_rtp_connection(struct rtp_connection *connection,
		unsigned long send_interval_sec,
		unsigned long send_interval_usec,
		unsigned int sampling_freq, unsigned int sample_size,
		int data_input) {
	int err = 0, i = 0;
	
	err = bind_udp(&connection->bind_sk);
	if(err)
		goto exit;
	
	connection->send_interval.tv_sec = send_interval_sec;
	connection->send_interval.tv_usec = send_interval_usec;
	connection->sampling_freq = sampling_freq;
	connection->sample_size = sample_size;

	/* A magical constant at the moment... */
	connection->header_size = get_rtp_header_size();
	
	connection->seq_no = 0;
	connection->ssrc = random32();
	connection->timestamp = 0;
	
	connection->data_input = data_input;

exit:
	return err;

exit_free_bind_sk:
	close(connection->bind_sk);
	return err;
}

int rtp_connection_kick(struct rtp_connection *connection) {
	
	int n, err = 0, retval = 0, i = 0;
	struct hostent *hp;
	char buffer[256];
	struct rtp_packet *packet;
	fd_set rfds;
	int readchars = 0;
	struct timeval tv;

	packet = create_rtp_packet(get_payload_size(connection));
	if(packet == NULL) {
		err = RTP_PACKET_ALLOC_ERROR;
		goto exit_no_packet;
	}


	/* Initialize rtp packet */
	init_rtp_packet(packet, connection->seq_no, connection->timestamp,
		connection->ssrc);


	/* Initialize test data */
	
	/* Skip file headers TODO remove this when fetching real data */ 
	readchars = read(connection->data_input, buffer, 24);

	/* Initializing select */
	
	/* Watch stdin (fd 0) to see when it has input. */
	FD_ZERO(&rfds);
	//FD_SET(0, &rfds);

	/* Wait up to five seconds. */
	tv.tv_sec = connection->send_interval.tv_sec;
	tv.tv_usec = connection->send_interval.tv_usec;

	readchars = read(connection->data_input, packet->payload, packet->payload_size);

	while(1) {
		if(readchars <= 0) {
			if(readchars < 0)
				err = RTP_DATA_READ_ERROR;
			goto exit;
		}

		retval = select(1, &rfds, NULL, NULL, &tv);
		
		if(retval == -1) {
			err = SELECT_ERROR;
			goto exit;
		}			
		else if (retval) /* TODO Here we should read user input */
			goto exit;
		else {
			/* TODO Here we should read the other file descriptors
			 * as well. */
			for(i = 0; i < connection->howmany; i++) {
				n = sendto(connection->bind_sk, packet->start,
					packet->packet_size, 0,
					(const struct sockaddr *)
					&connection->destinations[i].addr.
					addr_in,
					connection->destinations[i].length);
				if (n < 0) {
					err = UDP_SEND_ERROR;
					goto exit;
				}
			}
			
//			printf(".");
//			fflush(stdout);

			connection->seq_no++;
			connection->timestamp +=
				get_timestamp_per_packet_inc(connection);
			/* TODO packet no as seq no might wrap */
			init_rtp_packet(packet, htons(connection->seq_no),
				htonl(connection->timestamp), connection->ssrc);
			readchars = read(connection->data_input, packet->payload, packet->payload_size);

			/* TODO Here we should take into account the
			 * processing time taken to send the packets,
			 * so that we don't introduce extra latency
			 * between samples */
			tv.tv_sec = connection->send_interval.tv_sec;
			tv.tv_usec = connection->send_interval.tv_usec;
		}
	}
	
exit:	
	free(packet);
exit_no_packet:
	return 0;
}

static int bind_udp(int *sock) {
	int err = 0;

	/* TODO take socket args rom user */

	*sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (*sock < 0) {
		err = SOCK_CREATE_ERROR;
		goto exit;
	}

exit:	
	return err;
}

int free_rtp_connection(struct rtp_connection *connection) {
	int err = 0;

	free(connection->destinations);
	close(connection->bind_sk);
	return err;
}

int parse_destination(char *addr, char *port, struct destination *dest) {
	int err = 0;
	struct hostent *hp;

printf("parsing %s, %s\n", addr, port);

	dest->addr_family = AF_INET;
	dest->length = sizeof(struct sockaddr_in);

	dest->addr.addr_in.sin_family = AF_INET;
	hp = gethostbyname(addr);
	if (hp == 0) {
		err = UNKNOWN_HOST_ERROR;
		goto exit;
	}

	bcopy((char *)hp->h_addr, (char *)&dest->addr.addr_in.sin_addr,
		hp->h_length);
	dest->addr.addr_in.sin_port = htons(atoi(port));

exit:	
	return err;
}

int set_destinations(char addr[][MAX_IPV4_ADDR_LEN], char ports[][MAX_IPV4_ADDR_LEN],
		struct rtp_connection *connection, int howmany) {
	
	int err = 0, i = 0;

	connection->destinations = malloc(howmany * sizeof(struct destination));
	if(connection->destinations == NULL) {
		err = RTP_CONNECTION_ALLOC_ERROR;
		goto exit;
	}

	connection->howmany = howmany;

	for(i = 0; i < howmany && err == 0; i++)
		err = parse_destination(addr[i], ports[i],
			&connection->destinations[i]);
	if(err != 0)
		goto exit_free_dests;

exit:
	return err;

exit_free_dests:
	free(connection->destinations);
	return err;
}

static int get_payload_size(struct rtp_connection *connection) {
	return (int)(((float)connection->sample_size) *
		((float)connection->sampling_freq) *
		(((float)connection->send_interval.tv_usec) / 1000000 +
		((float)connection->send_interval.tv_sec)));
}

static unsigned long get_timestamp_per_packet_inc(
		struct rtp_connection *connection) {
	return (long)((((float)connection->send_interval.tv_usec) / 1000000 +
		((float)connection->send_interval.tv_sec)) *
		(float)connection->sampling_freq);
}

