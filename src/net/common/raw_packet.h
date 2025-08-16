#ifndef CHICKEN_NET_COMMON_RAW_PACKET
#define CHICKEN_NET_COMMON_RAW_PACKET

#include "aliases.h"

typedef struct {
	usize id;
	char *buffer;
	usize len;
} net_sb_raw_packet_t;

#endif
