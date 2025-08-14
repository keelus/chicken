#ifndef NET_SB_FIELDS_STRING_H
#define NET_SB_FIELDS_STRING_H

#include "../../../common.h"
#include "varint.h"

char *net_sb_string_parse(const raw_packet_t *raw_packet, usize *index) {
	usize len = net_sb_varint_parse(raw_packet, index);
	char *s = (char *)malloc(sizeof(char) * (len + 1));
	for(usize i = 0; i < len; i++) {
		s[i] = raw_packet->buffer[*index + i];
	}
	s[len] = '\0';
	*index += len;
	return s;
}

#endif
