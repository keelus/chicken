#include "packet_definitions/handshake/handshake.h"
#include "packet_definitions/login/login_start.h"
#include "packet_handlers/handshake/handshake.h"
#include "packet_handlers/login/login_start.h"
#include <stdio.h>

void handle_handshake_packet(net_sb_raw_packet_t raw_packet,
							 net_common_client_t *client) {
	switch(raw_packet.id) {
	case 0x00: {
		net_sb_packets_handshake_handshake *packet =
			net_sb_packets_handshake_handshake_parse(&raw_packet);
		net_sb_packet_handlers_handshake_handshake(client, packet);
	} break;
	default:
		printf("Unknown handshake packet with ID %zu.\n", raw_packet.id);
		exit(1);
		break;
	}
}
void handle_login_packet(net_sb_raw_packet_t raw_packet,
						 net_common_client_t *client) {
	switch(raw_packet.id) {
	case 0x00: {
		net_sb_packets_login_login_start *packet =
			net_sb_packets_login_login_start_parse(&raw_packet);
		net_sb_packet_handlers_login_login_start(client, packet);
	} break;
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
