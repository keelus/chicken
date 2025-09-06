#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../server/server.h"
#include "../server_bound/packet_handler.h"
#include "client.h"

void net_client_loop(net_server_t *server, net_client_t *client) {

	while(1) {
		net_sb_raw_packet_t raw_packet;
		if(net_sb_read_raw_packet(client->fd, &raw_packet) != 0) {
			// TODO: Handle error
			break;
		}
		net_sb_packet_handle(raw_packet, client);

		// net_server_add_raw_packet(server, client->username, raw_packet);
	}

	printf("[INFO] Terminating client connection...\n");
	net_client_destroy(client);
}

void net_client_destroy(net_client_t *client) {
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

void net_client_send_packet(const net_client_t *client,
							const net_cb_packet *packet) {
	send(client->fd, packet->buffer, packet->len, 0);
}
