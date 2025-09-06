#ifndef CHICKEN_NET_SB_RAW_PACKET
#define CHICKEN_NET_SB_RAW_PACKET

#include <stddef.h>

typedef struct {
	size_t id;
	char *buffer;
	size_t len;
} net_sb_raw_packet_t;

int net_sb_read_raw_packet(int fd, net_sb_raw_packet_t *raw_packet);

#endif
