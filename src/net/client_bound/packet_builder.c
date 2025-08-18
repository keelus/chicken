#include "packet_builder.h"
#include <assert.h>
#include <string.h>

void net_cb_packet_builder_add_bool(net_cb_packet *packet, const bool value) {
	net_cb_packet_builder_add_byte(packet, (u8)value);
}

void net_cb_packet_builder_add_byte(net_cb_packet *packet, const u8 value) {
	packet->len += 1;
	packet->buffer =
		(char *)realloc(packet->buffer, sizeof(char) * packet->len);
	packet->buffer[packet->len - 1] = (char)value;
}

void net_cb_packet_builder_add_short(net_cb_packet *packet, const i16 value) {
	net_cb_packet_builder_add_byte(packet, (u8)(value >> 8));
	net_cb_packet_builder_add_byte(packet, (u8)value);
}

void net_cb_packet_builder_add_int(net_cb_packet *packet, const i32 value) {
	u8 bytes[4];
	memcpy(bytes, &value, 4);
	net_cb_packet_builder_add_raw_bytes(packet, bytes, 4);
}

void net_cb_packet_builder_add_long(net_cb_packet *packet, const i64 value) {
	u8 bytes[8];
	memcpy(bytes, &value, 8);
	net_cb_packet_builder_add_raw_bytes(packet, bytes, 8);
}

void net_cb_packet_builder_add_128_bit_int(net_cb_packet *packet,
										   const i64 upper, const i64 lower) {
	u8 bytes[16];
	memcpy(bytes, &upper, 8);
	memcpy(bytes + 8, &lower, 8);
	net_cb_packet_builder_add_raw_bytes(packet, bytes, 16);
}

void net_cb_packet_builder_add_float(net_cb_packet *packet, const float value) {
	u8 bytes[4];
	memcpy(bytes, &value, 4);
	net_cb_packet_builder_add_raw_bytes(packet, bytes, 4);
}

void net_cb_packet_builder_add_double(net_cb_packet *packet,
									  const double value) {
	u8 bytes[8];
	memcpy(bytes, &value, 8);
	net_cb_packet_builder_add_raw_bytes(packet, bytes, 8);
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
	net_cb_packet_builder_add_raw_bytes(packet, bytes, 4);
}

void net_cb_packet_builder_add_raw_bytes(net_cb_packet *packet, const u8 *bytes,
										 const usize bytes_len) {
	packet->buffer = (char *)realloc(packet->buffer,
									 sizeof(char) * (packet->len + bytes_len));
	memcpy(packet->buffer + packet->len, bytes, bytes_len);
	packet->len += bytes_len;
}
