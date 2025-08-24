#ifndef CHICKEN_NET_SB_PACKET_HANDLERS_PLAY
#define CHICKEN_NET_SB_PACKET_HANDLERS_PLAY

#include "../../common/client.h"
#include "../packet_definitions/play.h"

#include "../../client_bound/packet_builder/packet_builder.h"

#include "../../common/client.h"
#include "../../client_bound/packet_definitions/play.h"

#include <stdio.h>

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


static inline void net_sb_packet_handlers_play_chat_message(
	net_common_client_t *client, net_sb_packets_play_chat_message *packet) {
	char json[1024];
	snprintf(json, sizeof(json), "{\"text\": \"%s: %s\"}", client->username,
			 packet->message);
	SEND_PACKET(net_cb_packets_play_chat_message_create(json));
}

#endif
