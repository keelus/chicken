#ifndef CHICKEN_NET_SB_PACKET_HANDLERS_LOGIN_LOGIN_START
#define CHICKEN_NET_SB_PACKET_HANDLERS_LOGIN_LOGIN_START

#include "../../../common/client.h"
#include "../../packet_definitions/login/login_start.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void send_login_success(net_common_client_t *client) {
	// Length: 1 byte
	// ID: 1 byte
	// String length: 1 byte
	// String: 1 byte
	// String length: 1 byte
	// String: 7 byte

	const char *uuid = "2c70029f-ff9f-45da-965b-f120d8a52938";
	const char *username = "keelus_";

	usize index = 0;
	char buffer[1024];
	buffer[index++] = 0;	// Length (unused here)
	buffer[index++] = 0x02; // ID
	buffer[index++] = strlen(uuid);
	memcpy(buffer + index, uuid, strlen(uuid));
	index += strlen(uuid);
	buffer[index++] = strlen(username);
	memcpy(buffer + index, username, strlen(username));
	index += strlen(username);

	usize len = index - 1;

	buffer[0] = len;
	net_common_client_send_packet(client, buffer, len + 1);
}

void send_join_game(net_common_client_t *client) {
	const char *level_type = "default";

	usize index = 0;
	char buffer[1024];
	buffer[index++] = 0;				  // Length (unused here)
	buffer[index++] = 0x01;				  // Packet ID
	buffer[index++] = 0x0;				  // Entity ID (byte 1)
	buffer[index++] = 0x0;				  // Entity ID (byte 2)
	buffer[index++] = 0x0;				  // Entity ID (byte 3)
	buffer[index++] = 0x0;				  // Entity ID (byte 4)
	buffer[index++] = 0x1;				  // Game mode (1=creative)
	buffer[index++] = 0x0;				  // Dimension (0=overworld)
	buffer[index++] = 0x2;				  // Difficulty (2=normal)
	buffer[index++] = 0x8;				  // Max players(8)
	buffer[index++] = strlen(level_type); // Level type str length
	memcpy(buffer + index, level_type, strlen(level_type)); // Level type
	index += strlen(level_type);

	usize len = index - 1;

	buffer[0] = len;
	net_common_client_send_packet(client, buffer, len + 1);
}

typedef union {
	float f;
	u8 bytes[4];
} float_union;

void send_player_look_and_position(net_common_client_t *client) {
	const double x = 0.0;
	const double y = 0.0;
	const double z = 0.0;
	const float yaw = 0.0;
	const float pitch = 0.0;
	const u8 on_ground = 0;


	usize index = 0;
	char buffer[1024];
	buffer[index++] = 0;	// Length (unused here)
	buffer[index++] = 0x08; // Packet ID
	memcpy(buffer + index, &x, 8);
	index += 8;
	memcpy(buffer + index, &y, 8);
	index += 8;
	memcpy(buffer + index, &z, 8);
	index += 8;
	memcpy(buffer + index, &yaw, 4);
	index += 4;
	memcpy(buffer + index, &pitch, 4);
	index += 4;
	buffer[index++] = on_ground;

	usize len = index - 1;

	buffer[0] = len;
	printf("Len: %zu\n", len);
	net_common_client_send_packet(client, buffer, len + 1);
}

static inline void net_sb_packet_handlers_login_login_start(
	net_common_client_t *client, net_sb_packets_login_login_start *packet) {
	printf("Player's name: \"%s\".\n", packet->name);

	send_login_success(client);
	send_join_game(client);
	send_player_look_and_position(client);
}


#endif
