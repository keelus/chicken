#ifndef NET_SB_FIELDS_VARINT_H
#define NET_SB_FIELDS_VARINT_H

#include "../../common/aliases.h"

usize net_sb_varint_parse(const char *buffer, const usize len, usize *index);

#endif
