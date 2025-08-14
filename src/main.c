#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "common.h"
#include "net/server_bound/fields/varint.h"
#include "net/server_bound/packets/handshake/handshake.h"

typedef enum {
	SERVER_STATE_HANDSHAKE = 0,
	SERVER_STATE_LOGIN = 1,
	SERVER_STATE_PLAY = 2,
} ServerState;

typedef struct {
	int fd;
} Client;

typedef struct {
	char *buff;
	usize len;
} packet_t;

packet_t *obtain_packet(int fd) {
	u8 b;
	isize n;
	usize value = 0;
	usize position = 0;

	printf("Will obtain packet.\n");

	while((n = recv(fd, &b, 1, 0)) > 0) {
		value |= (b & 0b01111111) << position;

		if((b & 0b10000000) == 0) { break; }

		position += 7;
		if(position >= 32) {
			printf("VarInt cannot exceed 32 bits.\n");
			exit(1);
		}
	}
	printf("Last byte: 0x%02x\n", b);
	printf("Length: %zu\n", value);

	if(n <= 0) {
		printf("Obtained read of length %lu.\n", n);
		exit(1);
	}

	char *buff = malloc(sizeof(char) * value);
	for(int i = 0; i < value; i++) {
		buff[i] = 0x69;
	}
	n = recv(fd, buff, value, 0);
	if(n <= 0) {
		printf("Obtained read of length %lu.\n", n);
		exit(1);
	}
	if(n != value) {
		printf("Expected %lu bytes, but read only %lu.\n", value, n);
		exit(1);
	}


	packet_t *packet = malloc(sizeof(packet_t));
	*packet = (packet_t){.buff = buff, .len = value};
	return packet;
}

int connect_client(Client *client) {
	packet_t *packet = obtain_packet(client->fd);
	if(packet == NULL) {
		printf("Got no first packet.\n");
		return -1;
	}

	raw_packet_t raw_packet = {
		.buffer = packet->buff,
		.len = packet->len,
	};

	usize index = 0;
	usize packet_id = net_sb_varint_parse(&raw_packet, &index);

	net_sb_packets_handshake_handshake *parsed_packet =
		net_sb_packets_handshake_handshake_parse(&(raw_packet_t){
			.buffer = packet->buff + index,
			.len = packet->len,
		});

	printf("====\n");
	printf("Protocol version: %zu\n", parsed_packet->protocol_version);
	printf("Address: \"%s\"\n", parsed_packet->address);
	printf("Port: %d\n", parsed_packet->port);
	printf("Next state: %zu\n", parsed_packet->next_state);

	return -1;
}

void *handle_client(void *arg) {
	int fd = *(int *)arg;
	free(arg);

	Client client = {.fd = fd};
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
