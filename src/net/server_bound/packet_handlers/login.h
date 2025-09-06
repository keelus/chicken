#ifndef CHICKEN_NET_SB_PACKET_HANDLERS_LOGIN
#define CHICKEN_NET_SB_PACKET_HANDLERS_LOGIN

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include "../packet_definitions/login.h"
#include "../../client/client.h"
#include "../../client_bound/packet_builder/packet_builder.h"
#include "../../client_bound/packet_definitions/login.h"
#include "../../client_bound/packet_definitions/play.h"

#define SEND_PACKET(packet)                                                    \
	do {                                                                       \
		net_cb_packet *final_packet =                                          \
			(net_cb_packet *)malloc(sizeof(net_cb_packet));                    \
		final_packet->buffer = NULL;                                           \
		final_packet->len = 0;                                                 \
		net_cb_packet_builder_add_varint(final_packet, packet->len);           \
		net_cb_packet_builder_add_raw_bytes(                                   \
			final_packet, (const uint8_t *)packet->buffer, packet->len);            \
		net_client_send_packet(client, final_packet);                          \
	} while(0);

void send_login_success(net_client_t *client) {
	char *uuid =
		"2c70029f-ff9f-45da-965b-f120d8a52938"; // TODO: Obtain UUID properly
	SEND_PACKET(
		net_cb_packets_login_login_success_create(uuid, client->username));
}

void send_join_game(net_client_t *client) {
	int32_t entity_id = 0;
	int8_t gamemode = 1;
	int8_t dimension = 0;
	uint8_t difficulty = 0;
	uint8_t max_players = 8;
	char *level_type = "default";

	SEND_PACKET(net_cb_packets_play_join_game_create(
		entity_id, gamemode, dimension, difficulty, max_players, level_type));
}

void send_player_look_and_position(net_client_t *client) {
	const double x = 0.5;
	const double y_eye_pos = 18.0;
	const double z = 0.5;
	const float yaw = 0.0;
	const float pitch = 0.0;
	const uint8_t on_ground = 0;

	SEND_PACKET(net_cb_packets_play_player_position_and_look_create(
		x, y_eye_pos, z, yaw, pitch, on_ground));
}

void send_chunk_data(net_client_t *client, int chunk_x, int chunk_z) {
	net_cb_packet *packet = (net_cb_packet *)malloc(sizeof(net_cb_packet));
	packet->buffer = NULL;
	packet->len = 0;

	bool ground_up_continuous = true;
	short primary_bit_map = 0b0000000000000001;
	short add_bit_map = 0b0000000000000000;

	net_cb_packet_builder_add_varint(packet, 0x21);
	net_cb_packet_builder_add_int(packet, chunk_x);
	net_cb_packet_builder_add_int(packet, chunk_z);
	net_cb_packet_builder_add_bool(packet, ground_up_continuous);
	net_cb_packet_builder_add_unsigned_short(packet, primary_bit_map);
	net_cb_packet_builder_add_unsigned_short(packet, add_bit_map);

	size_t raw_data_len = 4096 + 2048 + 2048 + 2048 + 256;
	uint8_t *raw_data = (uint8_t *)malloc(sizeof(uint8_t) * raw_data_len);

	memset(raw_data, 3, 4096);
	memset(raw_data + 4096, 0, 2048);
	memset(raw_data + 4096 + 2048, 0, 2048);
	memset(raw_data + 4096 + 2048 + 2048, 0xFF, 2048);
	memset(raw_data + 4096 + 2048 + 2048 + 2048, 0x00, 256);

	uLongf compressed_len = compressBound(raw_data_len);
	unsigned char *compressed = (unsigned char *)malloc(compressed_len);

	int result = compress(compressed, &compressed_len, raw_data, raw_data_len);
	assert(result == Z_OK);

	printf("Compressed length: %zu\n", compressed_len);
	net_cb_packet_builder_add_int(packet, compressed_len);
	net_cb_packet_builder_add_raw_bytes(packet, (uint8_t *)compressed,
										compressed_len);

	SEND_PACKET(packet);
}

// 0x00 - Login Start
static inline void net_sb_packet_handlers_login_login_start(
	net_client_t *client, net_sb_packets_login_login_start *packet) {
	client->username = strdup(packet->name);
	client->state = CLIENT_STATE_PLAY;

	send_login_success(client);

	send_join_game(client);
	send_chunk_data(client, 0, 0);
	send_chunk_data(client, -1, 0);
	send_chunk_data(client, 0, -1);
	send_chunk_data(client, -1, -1);
	send_player_look_and_position(client);
}

#endif
