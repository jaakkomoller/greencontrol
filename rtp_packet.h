#ifndef RTP_PACKET_H
#define RTP_PACKET_H


struct rtp_packet {

	int packet_size, payload_size;

	char start[0];
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

struct rtp_packet *create_rtp_packet(size_t payload_length);
int init_rtp_packet(struct rtp_packet *packet, unsigned seq_no,
	unsigned long timestamp, unsigned long ssrc);
int get_rtp_header_size();


#endif /* RTP_PACKET_H */
