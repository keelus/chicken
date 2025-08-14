#ifndef NET_SB_FIELDS_USHORT_H
#define NET_SB_FIELDS_USHORT_H

#include "../../../common.h"

u16 net_sb_ushort_parse(const raw_packet_t *raw_packet, usize *index) {
	u8 upper = raw_packet->buffer[*index];
	u8 lower = raw_packet->buffer[*index + 1];

	*index += 2;

	return ((u16)upper << 8) | (u16)lower;
}

#endif
