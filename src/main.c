#include <signal.h>
#include <zlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "net/common/client.h"
#include "net/common/raw_packet.h"
#include "net/server_bound/fields/varint.h"
#include "net/server_bound/packet_handler.h"

typedef struct {
	char *buff;
	usize len;
} packet_t;

isize obtain_packet(int fd, char **buffer) {
	u8 b;
	isize n;
	usize value = 0;
	usize position = 0;

	while((n = recv(fd, &b, 1, 0)) > 0) {
		value |= (b & 0b01111111) << position;

		if((b & 0b10000000) == 0) { break; }

		position += 7;
		if(position >= 32) {
			printf("VarInt cannot exceed 32 bits.\n");
			exit(1);
		}
	}

	if(n <= 0) {
		printf("[WARNING] Obtained read of length %lu.\n", n);
		return n;
	}

	*buffer = malloc(sizeof(char) * value);
	n = recv(fd, *buffer, value, 0);
	if(n <= 0) {
		printf("[WARNING] Obtained read of length %lu.\n", n);
		free(buffer);
		buffer = NULL;
		return n;
	}
	if(n != value) {
		printf("Expected %lu bytes, but read only %lu.\n", value, n);
		free(buffer);
		buffer = NULL;
		return -1;
	}

	return n;
}

int connect_client(net_common_client_t *client) {
	char *buffer = NULL;
	usize index, len, id;
	net_sb_raw_packet_t raw_packet;

	// Handshake packet
	len = obtain_packet(client->fd, &buffer);
	if(len <= 0) { return -1; }
	index = 0;
	id = net_sb_varint_parse(buffer, len, &index);
	raw_packet =
		(net_sb_raw_packet_t){.buffer = buffer + index, .len = len, .id = id};
	net_sb_packet_handle(raw_packet, client);
	free(buffer);
	buffer = NULL;

	// LoginStart packet
	len = obtain_packet(client->fd, &buffer);
	if(len <= 0) { return -1; }
	index = 0;
	id = net_sb_varint_parse(buffer, len, &index);
	raw_packet =
		(net_sb_raw_packet_t){.buffer = buffer + index, .len = len, .id = id};
	net_sb_packet_handle(raw_packet, client);
	free(buffer);
	buffer = NULL;

	return 0;
}

void *handle_client(void *arg) {
	int fd = *(int *)arg;
	free(arg);

	net_common_client_t client = {.fd = fd, .username = NULL};
	if(connect_client(&client) == 0) {
		printf("Client connected successfully.\n");

		char *buffer = NULL;
		usize index, len, id;
		net_sb_raw_packet_t raw_packet;

		while(1) {
			len = obtain_packet(client.fd, &buffer);
			if(len <= 0) {
				printf("[INFO] Terminating client connection...\n");
				net_common_client_destroy(&client);
				break;
			}

			index = 0;
			id = net_sb_varint_parse(buffer, len, &index);
			raw_packet = (net_sb_raw_packet_t){
				.buffer = buffer + index, .len = len, .id = id};
			net_sb_packet_handle(raw_packet, &client);
			free(buffer);
			buffer = NULL;
		}

		return NULL;
	} else {
		printf("[WARNING] The client was not completly connected.\n");
		printf("[INFO] Terminating client connection...\n");
		net_common_client_destroy(&client);
		return NULL;
	}
}

int server_fd = -1;

void handle_exit() {
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
			handle_exit();
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
