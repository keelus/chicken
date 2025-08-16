#ifndef CHICKEN_NET_SB_PACKET_HANDLERS_LOGIN_LOGIN_START
#define CHICKEN_NET_SB_PACKET_HANDLERS_LOGIN_LOGIN_START

#include "../../../common/client.h"
#include "../../packet_definitions/login/login_start.h"
#include <stdio.h>
#include <string.h>

static inline void net_sb_packet_handlers_login_login_start(
	net_common_client_t *client, net_sb_packets_login_login_start *packet) {
	printf("Player's name: \"%s\".\n", packet->name);

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


#endif
