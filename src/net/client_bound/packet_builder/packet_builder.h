#ifndef CHICKEN_NET_CB_PACKET_BUILDER
#define CHICKEN_NET_CB_PACKET_BUILDER

#include "../../common/aliases.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
	char *buffer;
	usize len;
} net_cb_packet;

void net_cb_packet_builder_add_bool(net_cb_packet *packet, const bool value);
void net_cb_packet_builder_add_byte(net_cb_packet *packet, const i8 value);
void net_cb_packet_builder_add_short(net_cb_packet *packet, const i16 value);
void net_cb_packet_builder_add_int(net_cb_packet *packet, const i32 value);
void net_cb_packet_builder_add_long(net_cb_packet *packet, const i64 value);
void net_cb_packet_builder_add_int_128(net_cb_packet *packet, const i64 upper,
									   const i64 lower);
void net_cb_packet_builder_add_float(net_cb_packet *packet, const float value);
void net_cb_packet_builder_add_double(net_cb_packet *packet,
									  const double value);
void net_cb_packet_builder_add_string(net_cb_packet *packet, const char *value);
void net_cb_packet_builder_add_varint(net_cb_packet *packet, const u32 value);

void net_cb_packet_builder_add_raw_bytes(net_cb_packet *packet, const u8 *bytes,
										 const usize bytes_len);

void net_cb_packet_builder_add_unsigned_byte(net_cb_packet *packet,
											 const u8 value);
void net_cb_packet_builder_add_unsigned_short(net_cb_packet *packet,
											  const u16 value);

#endif
