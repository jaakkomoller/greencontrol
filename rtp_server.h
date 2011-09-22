#ifndef RTP_SERVER_H
#define RTP_SERVER_H

/* Reads the U-law encoded data and sends to clients */
int rtp_server(FILE *infile, void *dest_addr, int count, int af_family);


#endif /* RTP_SERVER_H */
