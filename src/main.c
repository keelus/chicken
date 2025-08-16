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

usize obtain_packet(int fd, char **buffer) {
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
		printf("Obtained read of length %lu.\n", n);
		exit(1);
	}

	*buffer = malloc(sizeof(char) * value);
	n = recv(fd, *buffer, value, 0);
	if(n <= 0) {
		printf("Obtained read of length %lu.\n", n);
		exit(1);
	}
	if(n != value) {
		printf("Expected %lu bytes, but read only %lu.\n", value, n);
		exit(1);
	}

	return n;
}

int connect_client(net_common_client_t *client) {
	char *buffer = NULL;
	usize index, len, id;
	net_sb_raw_packet_t raw_packet;

	// Handshake packet
	len = obtain_packet(client->fd, &buffer);
	index = 0;
	id = net_sb_varint_parse(buffer, len, &index);
	raw_packet =
		(net_sb_raw_packet_t){.buffer = buffer + index, .len = len, .id = id};
	net_sb_packet_handle(raw_packet, client);
	free(buffer);
	buffer = NULL;

	// LoginStart packet
	len = obtain_packet(client->fd, &buffer);
	index = 0;
	id = net_sb_varint_parse(buffer, len, &index);
	raw_packet =
		(net_sb_raw_packet_t){.buffer = buffer + index, .len = len, .id = id};
	net_sb_packet_handle(raw_packet, client);
	free(buffer);
	buffer = NULL;

	return -1;
}

void *handle_client(void *arg) {
	int fd = *(int *)arg;
	free(arg);

	net_common_client_t client = {.fd = fd};
	if(connect_client(&client) == 0) {
		printf("Client connected successfully.\n");
	} else {
		printf("The client was not connected.\n");
	}

	close(fd);
	return NULL;
}

int main() {
	int s;
	struct sockaddr_in a;

	if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}


	int opt = 1;
	if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		perror("setsockopt");
		close(s);
		exit(1);
	}

	a.sin_family = AF_INET;
	a.sin_port = htons(25565);
	a.sin_addr.s_addr = INADDR_ANY;

	if(bind(s, (struct sockaddr *)&a, sizeof(a)) == -1) {
		perror("bind");
		close(s);
		exit(1);
	}

	if(listen(s, 5) == -1) {
		perror("listen");
		close(s);
		exit(1);
	}

	printf("Listening for connections...\n");
	while(1) {
		int *cfd = malloc(sizeof(int));
		if(!cfd) {
			perror("malloc");
			break;
		}
		*cfd = accept(s, NULL, NULL);
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

	close(s);
	return 0;
}
