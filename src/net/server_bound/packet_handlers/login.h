#ifndef CHICKEN_NET_SB_PACKET_HANDLERS_LOGIN
#define CHICKEN_NET_SB_PACKET_HANDLERS_LOGIN

#include "../../client_bound/packet_builder/packet_builder.h"

#include "../../common/client.h"
#include "../packet_definitions/login.h"
#include "../../client_bound/packet_definitions/login.h"
#include "../../client_bound/packet_definitions/play.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEND_PACKET(packet)                                                    \
	do {                                                                       \
		net_cb_packet *final_packet =                                          \
			(net_cb_packet *)malloc(sizeof(net_cb_packet));                    \
		final_packet->buffer = NULL;                                           \
		final_packet->len = 0;                                                 \
		net_cb_packet_builder_add_varint(final_packet, packet->len);           \
		net_cb_packet_builder_add_raw_bytes(                                   \
			final_packet, (const u8 *)packet->buffer, packet->len);            \
		net_common_client_send_packet(client, final_packet);                   \
	} while(0);

void send_login_success(net_common_client_t *client) {
	char *uuid =
		"2c70029f-ff9f-45da-965b-f120d8a52938"; // TODO: Obtain UUID properly
	SEND_PACKET(
		net_cb_packets_login_login_success_create(uuid, client->username));
}

void send_join_game(net_common_client_t *client) {
	i32 entity_id = 0;
	i8 gamemode = 1;
	i8 dimension = 0;
	u8 difficulty = 0;
	u8 max_players = 8;
	char *level_type = "default";

	SEND_PACKET(net_cb_packets_play_join_game_create(
		entity_id, gamemode, dimension, difficulty, max_players, level_type));
}

void send_player_look_and_position(net_common_client_t *client) {
	const double x = 0.0;
	const double y_eye_pos = 0.0;
	const double z = 0.0;
	const float yaw = 0.0;
	const float pitch = 0.0;
	const u8 on_ground = 0;

	SEND_PACKET(net_cb_packets_play_player_position_and_look_create(
		x, y_eye_pos, z, yaw, pitch, on_ground));
}

// 0x00 - Login Start
static inline void net_sb_packet_handlers_login_login_start(
	net_common_client_t *client, net_sb_packets_login_login_start *packet) {
	client->username = strdup(packet->name);
	client->state = CLIENT_STATE_PLAY;

	send_login_success(client);
	send_join_game(client);
	send_player_look_and_position(client);
}

#endif
