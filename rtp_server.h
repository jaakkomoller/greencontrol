#ifndef RTP_SERVER_H
#define RTP_SERVER_H

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
int rtp_server(FILE *soundfile, FILE *input, int control_port, void *dest_addr, int count, int af_family);


#endif /* RTP_SERVER_H */
