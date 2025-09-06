#include "packet_builder.h"
#include <assert.h>
#include <string.h>

void extract_bytes(const void *value, u8 *bytes, usize bytes_len) {
	memcpy(bytes, value, bytes_len);
#if __BYTE_ORDER == __LITTLE_ENDIAN
	for(usize i = 0; i < bytes_len / 2; i++) {
		usize j = bytes_len - 1 - i;
		u8 temp = bytes[i];
		bytes[i] = bytes[j];
		bytes[j] = temp;
	}
#endif
}
#define ADD_BIG_ENDIAN_BYTES(packet, value, n)                                 \
	do {                                                                       \
		u8 bytes[n];                                                           \
		extract_bytes(&value, bytes, n);                                       \
		net_cb_packet_builder_add_raw_bytes(packet, bytes, n);                 \
	} while(0);

void net_cb_packet_builder_add_bool(net_cb_packet *packet, const bool value) {
	net_cb_packet_builder_add_unsigned_byte(packet, (u8)value);
}

void net_cb_packet_builder_add_byte(net_cb_packet *packet, const i8 value) {
	packet->len += 1;
	packet->buffer =
		(char *)realloc(packet->buffer, sizeof(char) * packet->len);
	packet->buffer[packet->len - 1] = (char)value;
}

void net_cb_packet_builder_add_short(net_cb_packet *packet, const i16 value) {
	ADD_BIG_ENDIAN_BYTES(packet, value, 2);
}

void net_cb_packet_builder_add_int(net_cb_packet *packet, const i32 value) {
	ADD_BIG_ENDIAN_BYTES(packet, value, 4);
}

void net_cb_packet_builder_add_long(net_cb_packet *packet, const i64 value) {
	ADD_BIG_ENDIAN_BYTES(packet, value, 8);
}

void net_cb_packet_builder_add_int_128(net_cb_packet *packet, const i64 upper,
									   const i64 lower) {
	ADD_BIG_ENDIAN_BYTES(packet, upper, 8);
	ADD_BIG_ENDIAN_BYTES(packet, lower, 8);
}

// TODO: Fix maybe?
void net_cb_packet_builder_add_float(net_cb_packet *packet, const float value) {
	ADD_BIG_ENDIAN_BYTES(packet, value, 4);
}

void net_cb_packet_builder_add_double(net_cb_packet *packet,
									  const double value) {
	ADD_BIG_ENDIAN_BYTES(packet, value, 8);
}

void net_cb_packet_builder_add_string(net_cb_packet *packet,
									  const char *value) {
	usize value_len = strlen(value);
	net_cb_packet_builder_add_varint(packet, value_len);
	net_cb_packet_builder_add_raw_bytes(packet, (const u8 *)value, value_len);
}

void net_cb_packet_builder_add_varint(net_cb_packet *packet, const u32 value) {
	u8 bytes[4];
	u8 i = 0, byte;
	usize current_value = value;
	do {
		byte = current_value & 0b01111111;
		current_value >>= 7;
		if(current_value != 0) { byte |= 0b10000000; }

		assert(i <= sizeof(bytes));
		bytes[i++] = byte;
	} while(current_value != 0);
	net_cb_packet_builder_add_raw_bytes(packet, bytes, i);
}

void net_cb_packet_builder_add_raw_bytes(net_cb_packet *packet, const u8 *bytes,
										 const usize bytes_len) {
	packet->buffer = (char *)realloc(packet->buffer,
									 sizeof(char) * (packet->len + bytes_len));
	memcpy(packet->buffer + packet->len, bytes, bytes_len);
	packet->len += bytes_len;
}

void net_cb_packet_builder_add_unsigned_byte(net_cb_packet *packet,
											 const u8 value) {
	net_cb_packet_builder_add_byte(packet, (i8)value);
}

void net_cb_packet_builder_add_unsigned_short(net_cb_packet *packet,
											  const u16 value) {
	net_cb_packet_builder_add_short(packet, (i16)value);
}
