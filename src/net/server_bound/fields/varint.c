#include <stdio.h>
#include <stdlib.h>

#include "varint.h"

size_t net_sb_varint_parse(const char *buffer, const size_t len,
						   size_t *index) {
	size_t value = 0;
	size_t position = 0;
	char b;

	while(*index < len) {
		b = buffer[*index];
		*index += 1;
		value |= (b & 0b01111111) << position;

		if((b & 0b10000000) == 0) { break; }

		position += 7;
		if(position >= 32) {
			printf("[ERROR] VarInt cannot exceed 32 bits.\n");
			exit(1);
		}
	}

	return value;
}
