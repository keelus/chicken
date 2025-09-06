#include <assert.h>

#include "unsigned_short.h"

uint16_t net_sb_unsigned_short_parse(const char *buffer, size_t len,
									 size_t *index) {
	uint8_t upper = buffer[*index];
	uint8_t lower = buffer[*index + 1];

	*index += 2;

	return ((uint16_t)upper << 8) | (uint16_t)lower;
}
