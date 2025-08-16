#ifndef NET_SB_FIELDS_VARINT_H
#define NET_SB_FIELDS_VARINT_H

#include "../../common/aliases.h"
#include <stdio.h>

static inline usize net_sb_varint_parse(const char *buffer, const usize len,
										usize *index) {
	usize value = 0;
	usize position = 0;
	char b;

	while(*index < len) {
		b = buffer[*index];
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
