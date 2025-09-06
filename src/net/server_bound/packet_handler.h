#ifndef CHICKEN_NET_SB_PACKET_HANDLER
#define CHICKEN_NET_SB_PACKET_HANDLER

#include "raw_packet.h"
#include "../client/client.h"


void net_sb_packet_handle(net_sb_raw_packet_t raw_packet, net_client_t *client);

#endif
