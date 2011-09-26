#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "rtp_server.h"
 
struct rtp_packet {
	
	/* Each byte has to be put in reverse order */
	unsigned csrc_counter :4;
	unsigned extension :1;
	unsigned padding :1;
	unsigned version :2;

	unsigned payload_type :7;
	unsigned marker :1;

	unsigned seq_no :16;

	unsigned timestamp :32;
	unsigned ssrc_id :32;

	/* This must be the last field! */
	unsigned char payload[];
};

static struct rtp_packet *create_rtp_packet(size_t payload_length);
static int init_rtp_packet(struct rtp_packet *packet, unsigned seq_no, unsigned long timestamp, unsigned long ssrc);
static unsigned long get_ssrc();

int rtp_server(FILE *soundfile, FILE *input, int control_port, struct sockaddr_in *dest4, int count4, struct sockaddr_in6 *dest6, int count6) {
	
	int sock, n, err = 0, retval = 0;
	unsigned int length;
	unsigned long ssrc = get_ssrc(), packet_no = 0;
	struct sockaddr_in server, from;
	struct hostent *hp;
	char buffer[256];
	struct rtp_packet *packet;
	fd_set rfds;
	struct timeval tv;
	int readchars = 0;

	packet = create_rtp_packet(U_CODE_SAMPLE_SIZE);
	if(packet == NULL) {
		err = RTP_PACKET_ALLOC_ERROR;
		goto exit_no_packet;
	}

	/* TODO take socket args from user */

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		err = SOCK_CREATE_ERROR;
		goto exit_no_sock;
	}
	server.sin_family = AF_INET;
	hp = gethostbyname("localhost");
	if (hp == 0) {
		err = UNKNOWN_HOST_ERROR;
		goto exit;
	}

	bcopy((char *)hp->h_addr, (char *)&server.sin_addr,
		hp->h_length);
	server.sin_port = htons(1500);
	length=sizeof(struct sockaddr_in);

	/* Initialize rtp packet */
	init_rtp_packet(packet, 0, packet_no, ssrc);

	/* Initialize test data */
	
	/* Skip file headers TODO remove this when fetching http */ 
	readchars = fread (buffer, 1, 24, soundfile);
	/*readchars = fread (buffer, 1, 15, soundfile);
	readchars = fread (buffer, 1, 1, soundfile);
	printf("Encoding is %d\n", buffer[0]);
	goto exit;*/

	/* Initializing select */
	
	/* Watch stdin (fd 0) to see when it has input. */
	FD_ZERO(&rfds);
	FD_SET(0, &rfds);

	/* Wait up to five seconds. */
	tv.tv_sec = 0;
	tv.tv_usec = RTP_SAMPLE_INTERVAL_USEC;

	readchars = fread (packet->payload, 1, 1, soundfile);

	while(1) {
		if(readchars == 0)
			goto exit;

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
			n = sendto(sock, packet, RTP_HEADER_SIZE +
				U_CODE_SAMPLE_SIZE, 0,
				(const struct sockaddr *)&server,length);
			if (n < 0) {
				err = UDP_SEND_ERROR;
				goto exit;
			}
			
			packet_no++;
			/* TODO packet no as seq no might wrap */
			init_rtp_packet(packet, packet_no, packet_no, ssrc);
			readchars = fread(packet->payload, 1, U_CODE_SAMPLE_SIZE, soundfile);
			tv.tv_usec = RTP_SAMPLE_INTERVAL_USEC;
		}
	}
	
exit:	
	close(sock);
exit_no_sock:
	free(packet);
exit_no_packet:
	return 0;
}

/*
 * Allocates a variable size packet
 * 
 * @arg struct rtp_packet **packet: double pointer to point into allocated  
 * 		packet
 * @arg size_t payload_length: length of the payload in bytes
 * @ret: -1 if allocation fails, payload_length otherwise
 */

static struct rtp_packet *create_rtp_packet(size_t payload_length) {
	return (struct rtp_packet *) malloc(RTP_HEADER_SIZE + payload_length * sizeof(char));
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


