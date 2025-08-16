#ifndef CHICKEN_NET_CB_PACKET_BUILDER
#define CHICKEN_NET_CB_PACKET_BUILDER

#include "../common/aliases.h"
#include <cstdlib>
#include <string.h>

typedef struct {
	char *buffer;
	usize len;
} net_cb_packet;

usize net_cb_packet_builder_add_varint(net_cb_packet *packet,
									   const usize value) {
	return 0;
}

usize net_cb_packet_builder_add_string(net_cb_packet *packet,
									   const char *string) {
	usize added_bytes =
		net_cb_packet_builder_add_varint(packet, strlen(string)) +
		strlen(string);
	packet->len += added_bytes;
	packet->buffer =
		(char *)realloc(packet->buffer, sizeof(char) * packet->len);
	return added_bytes;
}

#endif
