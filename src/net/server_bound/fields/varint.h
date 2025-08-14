#ifndef NET_SB_FIELDS_VARINT_H
#define NET_SB_FIELDS_VARINT_H

#include "../../../common.h"
#include <stdio.h>

usize net_sb_varint_parse(const raw_packet_t *raw_packet, usize *index) {
	usize value = 0;
	usize position = 0;
	char b;

	while(*index < raw_packet->len) {
		b = raw_packet->buffer[*index];
		*index += 1;
		value |= (b & 0b01111111) << position;

		if((b & 0b10000000) == 0) { break; }

		position += 7;
		if(position >= 32) {
			printf("VarInt cannot exceed 32 bits.\n");
			exit(1);
		}
	}

	return value;
}

#endif
