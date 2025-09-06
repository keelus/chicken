#include <stdio.h>

#include "packet_definitions/handshake.h"
#include "packet_definitions/login.h"
#include "packet_definitions/play.h"
#include "packet_handlers/handshake.h"
#include "packet_handlers/login.h"
#include "packet_handlers/play.h"
#include "../common/raw_packet.h"


#define HANDLE_PACKET(packet_category, packet_name)                            \
	do {                                                                       \
		net_sb_packets_##packet_category##_##packet_name *packet =             \
			net_sb_packets_##packet_category##_##packet_name##_parse(          \
				&raw_packet);                                                  \
		net_sb_packet_handlers_##packet_category##_##packet_name(client,       \
																 packet);      \
	} while(0);

void handle_handshake_packet(net_sb_raw_packet_t raw_packet,
							 net_client_t *client) {
	switch(raw_packet.id) {
	case 0x00: HANDLE_PACKET(handshake, handshake); break;
	default:
		printf("[ERROR] Unknown handshake packet with ID 0x%02lx.\n", raw_packet.id);
		exit(1);
		break;
	}
}

void handle_login_packet(net_sb_raw_packet_t raw_packet, net_client_t *client) {
	switch(raw_packet.id) {
	case 0x00: HANDLE_PACKET(login, login_start); break;
	default:
		printf("[ERROR] Unknown login packet with ID 0x%02lx.\n", raw_packet.id);
		exit(1);
		break;
	}
}

void handle_play_packet(net_sb_raw_packet_t raw_packet, net_client_t *client) {
	switch(raw_packet.id) {
	case 0x01: HANDLE_PACKET(play, chat_message); break;
	case 0x03:		  // Ignore player
	case 0x04:		  // Ignore player position
	case 0x05:		  // Ignore player look
	case 0x06:		  // Ignore player position and look
	case 0x13:		  // Ignore player fly
	case 0x15:		  // Ignore player settings
	case 0x17: break; // Ignore plugin message
	default:
		printf("[ERROR] Unknown play packet with ID 0x%02lx.\n", raw_packet.id);
		exit(1);
		break;
	}
}

void net_sb_packet_handle(net_sb_raw_packet_t raw_packet,
						  net_client_t *client) {
	switch(client->state) {
	case CLIENT_STATE_HANDSHAKE:
		handle_handshake_packet(raw_packet, client);
		break;
	case CLIENT_STATE_LOGIN: handle_login_packet(raw_packet, client); break;
	case CLIENT_STATE_PLAY: handle_play_packet(raw_packet, client); break;
	default:
		printf("[ERROR] Cannot handle packet for client state %d.\n", client->state);
		break;
	}
}
