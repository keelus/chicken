#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "packet_builder.h"

void extract_bytes(const void *value, uint8_t *bytes, size_t bytes_len) {
	memcpy(bytes, value, bytes_len);
#if __BYTE_ORDER == __LITTLE_ENDIAN
	for(size_t i = 0; i < bytes_len / 2; i++) {
		size_t j = bytes_len - 1 - i;
		uint8_t temp = bytes[i];
		bytes[i] = bytes[j];
		bytes[j] = temp;
	}
#endif
}
#define ADD_BIG_ENDIAN_BYTES(packet, value, n)                                 \
	do {                                                                       \
		uint8_t bytes[n];                                                      \
		extract_bytes(&value, bytes, n);                                       \
		net_cb_packet_builder_add_raw_bytes(packet, bytes, n);                 \
	} while(0);

void net_cb_packet_builder_add_bool(net_cb_packet *packet, const bool value) {
	net_cb_packet_builder_add_unsigned_byte(packet, (uint8_t)value);
}

void net_cb_packet_builder_add_byte(net_cb_packet *packet, const int8_t value) {
	packet->len += 1;
	packet->buffer =
		(char *)realloc(packet->buffer, sizeof(char) * packet->len);
	packet->buffer[packet->len - 1] = (char)value;
}

void net_cb_packet_builder_add_short(net_cb_packet *packet,
									 const int16_t value) {
	ADD_BIG_ENDIAN_BYTES(packet, value, 2);
}

void net_cb_packet_builder_add_int(net_cb_packet *packet, const int32_t value) {
	ADD_BIG_ENDIAN_BYTES(packet, value, 4);
}

void net_cb_packet_builder_add_long(net_cb_packet *packet,
									const int64_t value) {
	ADD_BIG_ENDIAN_BYTES(packet, value, 8);
}

void net_cb_packet_builder_add_int_128(net_cb_packet *packet,
									   const int64_t upper,
									   const int64_t lower) {
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
	size_t value_len = strlen(value);
	net_cb_packet_builder_add_varint(packet, value_len);
	net_cb_packet_builder_add_raw_bytes(packet, (const uint8_t *)value,
										value_len);
}

void net_cb_packet_builder_add_varint(net_cb_packet *packet,
									  const uint32_t value) {
	uint8_t bytes[4];
	uint8_t i = 0, byte;
	size_t current_value = value;
	do {
		byte = current_value & 0b01111111;
		current_value >>= 7;
		if(current_value != 0) { byte |= 0b10000000; }

		assert(i <= sizeof(bytes));
		bytes[i++] = byte;
	} while(current_value != 0);
	net_cb_packet_builder_add_raw_bytes(packet, bytes, i);
}

void net_cb_packet_builder_add_raw_bytes(net_cb_packet *packet,
										 const uint8_t *bytes,
										 const size_t bytes_len) {
	packet->buffer = (char *)realloc(packet->buffer,
									 sizeof(char) * (packet->len + bytes_len));
	memcpy(packet->buffer + packet->len, bytes, bytes_len);
	packet->len += bytes_len;
}

void net_cb_packet_builder_add_unsigned_byte(net_cb_packet *packet,
											 const uint8_t value) {
	net_cb_packet_builder_add_byte(packet, (int8_t)value);
}

void net_cb_packet_builder_add_unsigned_short(net_cb_packet *packet,
											  const uint16_t value) {
	net_cb_packet_builder_add_short(packet, (int16_t)value);
}
