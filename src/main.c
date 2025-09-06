#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include "bits/pthreadtypes.h"
#include "net/client/client.h"
#include "net/server_bound/packet_handler.h"
#include "net/server_bound/raw_packet.h"
#include "netinet/in.h"
#include "pthread.h"
#include "sys/socket.h"

typedef struct {
	char *buff;
	size_t len;
} packet_t;

int connect_client(net_client_t *client) {
	net_sb_raw_packet_t raw_packet;

	// Handshake packet
	if(net_sb_read_raw_packet(client->fd, &raw_packet) != 0) {
		// TODO: Handle error
		return -1;
	}
	net_sb_packet_handle(raw_packet, client);

	// LoginStart packet
	if(net_sb_read_raw_packet(client->fd, &raw_packet) != 0) {
		// TODO: Handle error
		return -1;
	}
	net_sb_packet_handle(raw_packet, client);

	return 0;
}

void *handle_client(void *arg) {
	int fd = *(int *)arg;
	free(arg);

	net_server_t server;
	net_client_t client = {.fd = fd, .username = NULL};
	if(connect_client(&client) == 0) {
		printf("Client connected successfully.\n");

		net_client_loop(&server, &client);

		return NULL;
	} else {
		printf("[WARNING] The client was not completly connected.\n");
		printf("[INFO] Terminating client connection...\n");
		net_client_destroy(&client);
		return NULL;
	}
}

int server_fd = -1;

void handle_exit(int) {
	printf("Handling exit.\n");
	if(server_fd != -1) {
		close(server_fd);
		server_fd = -1;
	}
}

int main() {
	signal(SIGINT, handle_exit);

	struct sockaddr_in a;

	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}


	int opt = 1;
	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
	   -1) {
		perror("setsockopt");
		close(server_fd);
		exit(1);
	}

	memset(&a, 0, sizeof(a));
	a.sin_family = AF_INET;
	a.sin_port = htons(25565);
	a.sin_addr.s_addr = INADDR_ANY;

	if(bind(server_fd, (struct sockaddr *)&a, sizeof(a)) == -1) {
		perror("bind");
		close(server_fd);
		exit(1);
	}

	if(listen(server_fd, 5) == -1) {
		perror("listen");
		close(server_fd);
		exit(1);
	}

	printf("Listening for connections...\n");
	while(server_fd != -1) {
		int *cfd = malloc(sizeof(int));
		if(!cfd) {
			perror("malloc");
			handle_exit(0);
			return EXIT_FAILURE;
		}
		*cfd = accept(server_fd, NULL, NULL);
		if(*cfd == -1) {
			perror("accept");
			free(cfd);
			continue;
		}
		printf("Connection received.\n");

		pthread_t tid;
		if(pthread_create(&tid, NULL, handle_client, cfd) != 0) {
			perror("pthread_create");
			close(*cfd);
			free(cfd);
			continue;
		}
		pthread_detach(tid);
	}

	return EXIT_SUCCESS;
}
