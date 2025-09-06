#ifndef NET_SB_FIELDS_UNSIGNED_SHORT_H
#define NET_SB_FIELDS_UNSIGNED_SHORT_H

#include <stddef.h>
#include <stdint.h>

uint16_t net_sb_unsigned_short_parse(const char *buffer, size_t len,
									 size_t *index);

#endif
