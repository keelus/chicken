#ifndef NET_SB_FIELDS_VARINT_H
#define NET_SB_FIELDS_VARINT_H

#include <stddef.h>

size_t net_sb_varint_parse(const char *buffer, const size_t len, size_t *index);

#endif
