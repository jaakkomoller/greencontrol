#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "rtp_packet.h"

#define RTP_HEADER_SIZE 12

struct rtp_packet *create_rtp_packet(size_t payload_size) {
	struct rtp_packet *ret = NULL;

	ret = (struct rtp_packet *) malloc(RTP_HEADER_SIZE + payload_size
		* sizeof(char));
	if(ret == NULL)
		goto exit;
	
	ret->packet_size = RTP_HEADER_SIZE + payload_size;
	ret->payload_size = payload_size;
	
exit:
	return ret;
}



int init_rtp_packet(struct rtp_packet *packet, unsigned seq_no,
		unsigned long timestamp, unsigned long ssrc) {
	
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


int get_rtp_header_size() {
	return RTP_HEADER_SIZE;
}
