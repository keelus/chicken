#include <stdio.h>
#include "varint.h"

usize net_sb_varint_parse(const char *buffer, const usize len, usize *index) {
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
			printf("[ERROR] VarInt cannot exceed 32 bits.\n");
			exit(1);
		}
	}

	return value;
}
