#ifndef CHICKEN_NET_COMMON_CLIENT
#define CHICKEN_NET_COMMON_CLIENT

#include "aliases.h"
#include "../client_bound/packet_builder/packet_builder.h"

#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

typedef enum {
	CLIENT_STATE_HANDSHAKE = 0,
	CLIENT_STATE_LOGIN = 1,
	CLIENT_STATE_PLAY = 2,
} ClientState;

typedef struct {
	int fd;
	ClientState state;
	char *username;
} net_common_client_t;

static inline void
net_common_client_send_packet(const net_common_client_t *client,
							  const net_cb_packet *packet) {
	send(client->fd, packet->buffer, packet->len, 0);
}

static inline void net_common_client_destroy(net_common_client_t *client) {
	if(!client) { return; }
	if(client->username) {
		free(client->username);
		client->username = NULL;
	}
	if(client->fd > 0) {
		close(client->fd);
		client->fd = -1;
	}
}

#endif
