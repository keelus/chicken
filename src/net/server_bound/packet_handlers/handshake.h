#ifndef CHICKEN_NET_SB_PACKET_HANDLERS_HANDSHAKE
#define CHICKEN_NET_SB_PACKET_HANDLERS_HANDSHAKE

#include <stdio.h>

#include "../packet_definitions/handshake.h"
#include "../../client/client.h"

static inline void net_sb_packet_handlers_handshake_handshake(
	net_client_t *client, net_sb_packets_handshake_handshake *packet) {
	if(packet->next_state != 2) {
		printf("[ERROR] Next state at handshake was not login. Unimplemented.");
		exit(1);
	}

	client->state = CLIENT_STATE_LOGIN;
}

#endif
