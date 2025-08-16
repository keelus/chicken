#ifndef NET_SB_FIELDS_UNSIGNED_SHORT_H
#define NET_SB_FIELDS_UNSIGNED_SHORT_H

#include "../../common/aliases.h"

static inline u16 net_sb_unsigned_short_parse(const char *buffer, usize len,
											  usize *index) {
	u8 upper = buffer[*index];
	u8 lower = buffer[*index + 1];

	*index += 2;

	return ((u16)upper << 8) | (u16)lower;
}

#endif
