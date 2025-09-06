#ifndef CHICKEN_NET_CLIENT
#define CHICKEN_NET_CLIENT

#include "../client_bound/packet_builder/packet_builder.h"
#include "../server/server.h"

typedef enum {
	CLIENT_STATE_HANDSHAKE = 0,
	CLIENT_STATE_LOGIN = 1,
	CLIENT_STATE_PLAY = 2,
} ClientState;

typedef struct {
	int fd;
	ClientState state;
	char *username;
} net_client_t;

void net_client_send_packet(const net_client_t *client,
							const net_cb_packet *packet);

void net_client_destroy(net_client_t *client);

void net_client_loop(net_server_t *server, net_client_t *client);


#endif
