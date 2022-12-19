#include <string.h>
#include "stm32f1xx.h"
#include "ethernet.h"
#include "ip.h"
#include "arp.h"
#include "enc28j60.h"
#include "lan.h"

uint8_t mac_addr[] = MAC_ADDR;

void eth_reply(eth_frame_t *frame, uint16_t len)
{
	memcpy(frame->to_addr, frame->from_addr, 6);
	memcpy(frame->from_addr, mac_addr, 6);
	enc28j60_send_packet((uint8_t *)frame, len + sizeof(eth_frame_t));
}


void eth_filter(eth_frame_t *frame, uint16_t len)
{
    if (len < sizeof(eth_frame_t)) return;
    switch(frame->type)
    {
        case ETH_TYPE_ARP:
            arp_filter(frame, len - sizeof(eth_frame_t));
            break;
        case ETH_TYPE_IP:
            ip_filter(frame, len - sizeof(eth_frame_t));
            break;
    }
}

// send Ethernet frame
// fields must be set:
//	- frame.to_addr
//	- frame.type
void eth_send(eth_frame_t *frame, uint16_t len)
{
	memcpy(frame->from_addr, mac_addr, 6);
	enc28j60_send_packet((void*)frame, len +
		sizeof(eth_frame_t));
}