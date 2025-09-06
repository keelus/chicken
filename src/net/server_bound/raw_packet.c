#include "raw_packet.h"
#include "fields/varint.h"
#include "sys/socket.h"
#include <stdio.h>

int read_packet_length(int fd, u32 *value) {
	u8 b;
	isize n;
	usize position = 0;

	*value = 0;

	while((n = recv(fd, &b, 1, 0)) > 0) {
		*value |= (b & 0b01111111) << position;

		if((b & 0b10000000) == 0) { break; }

		position += 7;
		if(position >= 32) {
			printf("[ERROR] 32 bits exceeding VarInt encountered when parsing "
				   "packet length.\n");
			return -1;
		}
	}

	if(n <= 0) {
		printf("[ERROR] Invalid read of length %lu encountered when parsing "
			   "packet length.\n",
			   n);
		return -1;
	}

	return 0;
}

int net_sb_read_raw_packet(int fd, net_sb_raw_packet_t *raw_packet) {
	u32 expected_len;
	if(read_packet_length(fd, &expected_len) != 0) { return -1; }

	char *buffer = malloc(sizeof(char) * expected_len);
	isize len = recv(fd, buffer, expected_len, 0);
	if(expected_len <= 0) {
		printf("[ERROR] Invalid read of length %lu.\n", len);
		free(buffer);
		return -1;
	}

	if(len != expected_len) {
		printf("[ERROR] Expected %u bytes, but read only %lu.\n", expected_len,
			   len);
		free(buffer);
		return -1;
	}

	usize index = 0;
	raw_packet->id = net_sb_varint_parse(buffer, expected_len, &index);
	raw_packet->len = expected_len;
	raw_packet->buffer = buffer + index;

	return 0;
}
