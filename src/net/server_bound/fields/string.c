#include <stdlib.h>

#include "string.h"
#include "varint.h"

char *net_sb_string_parse(const char *buffer, const size_t len, size_t *index) {
	size_t string_len = net_sb_varint_parse(buffer, len, index);
	char *s = malloc(sizeof(char) * (string_len + 1));
	for(size_t i = 0; i < string_len; i++) {
		s[i] = buffer[*index + i];
	}
	s[string_len] = '\0';
	*index += string_len;
	return s;
}
