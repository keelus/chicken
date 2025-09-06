#ifndef CHICKEN_NET_CB_PACKET_BUILDER
#define CHICKEN_NET_CB_PACKET_BUILDER

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
	char *buffer;
	size_t len;
} net_cb_packet;

void net_cb_packet_builder_add_bool(net_cb_packet *packet, const bool value);
void net_cb_packet_builder_add_byte(net_cb_packet *packet, const int8_t value);
void net_cb_packet_builder_add_short(net_cb_packet *packet,
									 const int16_t value);
void net_cb_packet_builder_add_int(net_cb_packet *packet, const int32_t value);
void net_cb_packet_builder_add_long(net_cb_packet *packet, const int64_t value);
void net_cb_packet_builder_add_int_128(net_cb_packet *packet,
									   const int64_t upper,
									   const int64_t lower);
void net_cb_packet_builder_add_float(net_cb_packet *packet, const float value);
void net_cb_packet_builder_add_double(net_cb_packet *packet,
									  const double value);
void net_cb_packet_builder_add_string(net_cb_packet *packet, const char *value);
void net_cb_packet_builder_add_varint(net_cb_packet *packet,
									  const uint32_t value);

void net_cb_packet_builder_add_raw_bytes(net_cb_packet *packet,
										 const uint8_t *bytes,
										 const size_t bytes_len);

void net_cb_packet_builder_add_unsigned_byte(net_cb_packet *packet,
											 const uint8_t value);
void net_cb_packet_builder_add_unsigned_short(net_cb_packet *packet,
											  const uint16_t value);

#endif
