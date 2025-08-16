#ifndef NET_SB_FIELDS_STRING_H
#define NET_SB_FIELDS_STRING_H

#include "../../common/aliases.h"
#include "varint.h"

static inline char *net_sb_string_parse(const char *buffer, const usize len,
										usize *index) {
	usize string_len = net_sb_varint_parse(buffer, len, index);
	char *s = (char *)malloc(sizeof(char) * (string_len + 1));
	for(usize i = 0; i < len; i++) {
		s[i] = buffer[*index + i];
	}
	s[len] = '\0';
	*index += len;
	return s;
}

#endif
