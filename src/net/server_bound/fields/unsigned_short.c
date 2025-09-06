#include "unsigned_short.h"
#include <assert.h>

u16 net_sb_unsigned_short_parse(const char *buffer, usize len, usize *index) {
	u8 upper = buffer[*index];
	u8 lower = buffer[*index + 1];

	*index += 2;

	return ((u16)upper << 8) | (u16)lower;
}
