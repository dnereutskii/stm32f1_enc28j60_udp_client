#include <stdio.h>
#include <string.h>
#include "stm32f1xx.h"
#include "ethernet.h"
#include "ip.h"
#include "arp.h"
#include "lan.h"

uint8_t arp_cache_wr;
arp_cache_entry_t arp_cache[ARP_CACHE_SIZE];

void arp_filter(eth_frame_t *frame, uint16_t len)
{
    arp_message_t *msg = (arp_message_t *)(frame->data);

    if(len < sizeof(arp_message_t)) return;
    if ((msg->hw_type == ARP_HW_TYPE_ETH) &&
        (msg->proto_type == ARP_PROTO_TYPE_IP))
    {
        if ((msg->type == ARP_TYPE_REQUEST) && 
        	(msg->ip_addr_to == ip_addr))
        {
            msg->type = ARP_TYPE_RESPONSE;
            memcpy(msg->mac_addr_to, msg->mac_addr_from, 6);
            memcpy(msg->mac_addr_from, mac_addr, 6);
            msg->ip_addr_to = msg->ip_addr_from;
            msg->ip_addr_from = ip_addr;

            eth_reply(frame, sizeof(arp_message_t));
        }         
    }
}

// process arp packet
void arp_filter(eth_frame_t *frame, uint16_t len)
{
	arp_message_t *msg = (void*)(frame->data);

	if(len >= sizeof(arp_message_t))
	{
		if( (msg->hw_type == ARP_HW_TYPE_ETH) &&
			(msg->proto_type == ARP_PROTO_TYPE_IP) &&
			(msg->ip_addr_to == ip_addr) )
		{
			switch(msg->type)
			{
			case ARP_TYPE_REQUEST:
				msg->type = ARP_TYPE_RESPONSE;
				memcpy(msg->mac_addr_to, msg->mac_addr_from, 6);
				memcpy(msg->mac_addr_from, mac_addr, 6);
				msg->ip_addr_to = msg->ip_addr_from;
				msg->ip_addr_from = ip_addr;
				eth_reply(frame, sizeof(arp_message_t));
				break;
			case ARP_TYPE_RESPONSE:
				if(!arp_search_cache(msg->ip_addr_from))
				{
					arp_cache[arp_cache_wr].ip_addr = msg->ip_addr_from;
					memcpy(arp_cache[arp_cache_wr].mac_addr, msg->mac_addr_from, 6);
					arp_cache_wr++;
					if(arp_cache_wr == ARP_CACHE_SIZE)
						arp_cache_wr = 0;
				}
				break;
			}
		}
	}
}

// search ARP cache
uint8_t *arp_search_cache(uint32_t node_ip_addr)
{
    uint8_t i;

    for(i = 0; i < ARP_CACHE_SIZE; ++i)
    {
        if(arp_cache[i].ip_addr == node_ip_addr) return arp_cache[i].mac_addr;
    }
    return NULL;
}

uint8_t *arp_resolve(uint32_t node_ip_addr)
{
    eth_frame_t *frame = (void*)net_buf;
    arp_message_t *msg = (void*)(frame->data);
    uint8_t *mac;
    
    // search arp cache
    mac = arp_search_cache(node_ip_addr);
    if(mac != NULL)	return mac;
    
    // send request
    memset(frame->to_addr, 0xff, 6);
    frame->type = ETH_TYPE_ARP;
    
    msg->hw_type = ARP_HW_TYPE_ETH;
    msg->proto_type = ARP_PROTO_TYPE_IP;
    msg->hw_addr_len = 6;
    msg->proto_addr_len = 4;
    msg->type = ARP_TYPE_REQUEST;
    memcpy(msg->mac_addr_from, mac_addr, 6);
    msg->ip_addr_from = ip_addr;
    memset(msg->mac_addr_to, 0x00, 6);
    msg->ip_addr_to = node_ip_addr;
    
    eth_send(frame, sizeof(arp_message_t));
    return 0;
}