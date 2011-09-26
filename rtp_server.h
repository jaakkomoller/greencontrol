#ifndef RTP_SERVER_H
#define RTP_SERVER_H

#define RTP_HEADER_SIZE 12
#define U_CODE_SAMPLE_SIZE 1
#define RTP_SAMPLE_INTERVAL_USEC 125

/* Error codes */
#define SOCK_CREATE_ERROR 1
#define UNKNOWN_HOST_ERROR 2
#define UDP_SEND_ERROR 3
#define SELECT_ERROR 4
#define RTP_PACKET_ALLOC_ERROR 5


#include <netinet/in.h>
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
int rtp_server(FILE *soundfile, FILE *input, int control_port, struct sockaddr_in *dest4, int count4, struct sockaddr_in6 *dest6, int count6);

#endif /* RTP_SERVER_H */
