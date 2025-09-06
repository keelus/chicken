#include "string.h"
#include "varint.h"

char *net_sb_string_parse(const char *buffer, const usize len, usize *index) {
	usize string_len = net_sb_varint_parse(buffer, len, index);
	char *s = (char *)malloc(sizeof(char) * (string_len + 1));
	for(usize i = 0; i < string_len; i++) {
		s[i] = buffer[*index + i];
	}
	s[string_len] = '\0';
	*index += string_len;
	return s;
}
