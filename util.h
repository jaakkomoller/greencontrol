#ifndef UTIL_H
#define UTIL_H

// Sound sample size in bytes
#define SAMPLE_SIZE 1
#define RTP_SAMPLING_FREQ 8000
#define RTP_SEND_INTERVAL_SEC 0 
#define RTP_SEND_INTERVAL_USEC 20000 

// Addressing limits
#define MAX_IPV4_ADDRS 100
#define MAX_IPV4_ADDR_LEN 100

/* Error codes */
#define SOCK_CREATE_ERROR 1
#define UNKNOWN_HOST_ERROR 2
#define UDP_SEND_ERROR 3
#define SELECT_ERROR 4
#define RTP_PACKET_ALLOC_ERROR 5
#define RTP_CONNECTION_ALLOC_ERROR 6
#define RTP_DATA_READ_ERROR 7
#define COMMAND_LINE_PARSING_ERROR 8

enum state {RUNNING, PAUSE, STOP};

struct cl_options {
	int addresses;
	char destsarray[MAX_IPV4_ADDRS][MAX_IPV4_ADDR_LEN];
	char portsarray[MAX_IPV4_ADDRS][MAX_IPV4_ADDR_LEN];

	/* Debug flags, remove from final.. */
	int rtptest;
};

unsigned long random32();
int parse_opts(int argc, char **argv, struct cl_options *opt);


#endif /* RTP_PACKET_H */

