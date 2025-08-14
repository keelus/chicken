#ifndef CHICKEN_NET_SB_PACKETS_HANDSHAKE
#define CHICKEN_NET_SB_PACKETS_HANDSHAKE

#include "../../../../common.h"

#include "../../fields/varint.h"
#include "../../fields/string.h"
#include "../../fields/ushort.h"
#include <stdio.h>

typedef struct {
	usize protocol_version;
	char *address;
	u16 port;
	usize next_state;
} net_sb_packets_handshake_handshake;

net_sb_packets_handshake_handshake *
net_sb_packets_handshake_handshake_parse(raw_packet_t *raw_packet) {
	usize index = 0;
	usize protocol_version = net_sb_varint_parse(raw_packet, &index);
	char *address = net_sb_string_parse(raw_packet, &index);
	u16 port = net_sb_ushort_parse(raw_packet, &index);
	usize next_state = net_sb_varint_parse(raw_packet, &index);

	net_sb_packets_handshake_handshake *packet =
		(net_sb_packets_handshake_handshake *)malloc(
			sizeof(net_sb_packets_handshake_handshake));
	*packet = (net_sb_packets_handshake_handshake){.protocol_version =
													   protocol_version,
												   .address = address,
												   .port = port,
												   .next_state = next_state};

	return packet;
}

#endif
