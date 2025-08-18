#ifndef CHICKEN_NET_SB_PACKET_HANDLERS_HANDSHAKE
#define CHICKEN_NET_SB_PACKET_HANDLERS_HANDSHAKE

#include "../../common/client.h"
#include "../packet_definitions/handshake.h"

static inline void net_sb_packet_handlers_handshake_handshake(
	net_common_client_t *client, net_sb_packets_handshake_handshake *packet) {
	client->state = CLIENT_STATE_LOGIN;
}

#endif
