#include "packet_definitions/handshake.h"
#include "packet_definitions/login.h"
#include "packet_handlers/handshake.h"
#include "packet_handlers/login.h"

#include "../common/raw_packet.h"
#include "../common/client.h"

#include <stdio.h>

#define HANDLE_PACKET(packet_category, packet_name)                            \
	do {                                                                       \
		net_sb_packets_##packet_category##_##packet_name *packet =             \
			net_sb_packets_##packet_category##_##packet_name##_parse(          \
				&raw_packet);                                                  \
		net_sb_packet_handlers_##packet_category##_##packet_name(client,       \
																 packet);      \
	} while(0);

void handle_handshake_packet(net_sb_raw_packet_t raw_packet,
							 net_common_client_t *client) {
	switch(raw_packet.id) {
	case 0x00: HANDLE_PACKET(handshake, handshake); break;
	default:
		printf("Unknown handshake packet with ID %zu.\n", raw_packet.id);
		exit(1);
		break;
	}
}

void handle_login_packet(net_sb_raw_packet_t raw_packet,
						 net_common_client_t *client) {
	switch(raw_packet.id) {
	case 0x00: HANDLE_PACKET(login, login_start); break;
	default:
		printf("Unknown login packet with ID %zu.\n", raw_packet.id);
		exit(1);
		break;
	}
}

void handle_play_packet(net_sb_raw_packet_t raw_packet,
						net_common_client_t *client) {
	switch(raw_packet.id) {
	default:
		printf("Unknown play packet with ID %zu.\n", raw_packet.id);
		exit(1);
		break;
	}
}

void net_sb_packet_handle(net_sb_raw_packet_t raw_packet,
						  net_common_client_t *client) {
	switch(client->state) {
	case CLIENT_STATE_HANDSHAKE:
		handle_handshake_packet(raw_packet, client);
		break;
	case CLIENT_STATE_LOGIN: handle_login_packet(raw_packet, client); break;
	case CLIENT_STATE_PLAY: handle_play_packet(raw_packet, client); break;
	default:
		printf("Cannot handle packet for client state %d.\n", client->state);
		break;
	}
}
