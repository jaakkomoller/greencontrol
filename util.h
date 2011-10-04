#ifndef UTIL_H
#define UTIL_H

// Sound sample size in bytes
#define SAMPLE_SIZE 1
#define RTP_SAMPLING_FREQ 8000
#define RTP_SEND_INTERVAL_SEC 0 
#define RTP_SEND_INTERVAL_USEC 20000 

/* Error codes */
#define SOCK_CREATE_ERROR 1
#define UNKNOWN_HOST_ERROR 2
#define UDP_SEND_ERROR 3
#define SELECT_ERROR 4
#define RTP_PACKET_ALLOC_ERROR 5
#define RTP_CONNECTION_ALLOC_ERROR 6
#define RTP_DATA_READ_ERROR 7


unsigned long random32();



#endif /* RTP_PACKET_H */

