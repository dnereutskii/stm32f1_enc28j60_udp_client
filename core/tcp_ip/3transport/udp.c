#include "stm32f1xx.h"
#include "ethernet.h"
#include "ip.h"
#include "udp.h"
#include "buart.h"

void udp_filter(eth_frame_t *frame, uint16_t len)
{
    ip_packet_t *ip = (void*)(frame->data);
    udp_packet_t *udp = (void*)(ip->data);

    if(len >= sizeof(udp_packet_t))
    {
        udp_packet(frame, ntohs(udp->len) - sizeof(udp_packet_t));
    }
}

void udp_reply(eth_frame_t *frame, uint16_t len)
{
    ip_packet_t *ip = (void*)(frame->data);
    udp_packet_t *udp = (void*)(ip->data);
    uint16_t temp;

    len += sizeof(udp_packet_t);

    temp = udp->from_port;
    udp->from_port = udp->to_port;
    udp->to_port = temp;

    udp->len = htons(len);

    udp->cksum = 0;
    udp->cksum = ip_cksum(len + IP_PROTOCOL_UDP, (uint8_t*)udp - 8, len + 8);

    ip_reply(frame, len);
}

void udp_packet(eth_frame_t *frame, uint16_t len)
{
    ip_packet_t *ip = (ip_packet_t*)(frame->data);
    udp_packet_t *udp = (udp_packet_t*)(ip->data);
    uint8_t *data = udp->data;
    uint8_t count;

    for(uint8_t i = 0; i < len; ++i)
    {
        uart_write_byte(data[i]);
    }

    count = uart_rx_count();
    if(count)
    {
        for(uint8_t i = 0; i < count; ++i)
        {
        	data[i] = uart_read();
        }
        udp_reply(frame, count);
	}
}

// send UDP packet
// fields must be set:
//	- ip.dst
//	- udp.src_port
//	- udp.dst_port
// uint16_t len is UDP data payload length
uint8_t udp_send(eth_frame_t *frame, uint16_t len)
{
	ip_packet_t *ip = (void*)(frame->data);
	udp_packet_t *udp = (void*)(ip->data);

	len += sizeof(udp_packet_t);

	ip->protocol = IP_PROTOCOL_UDP;
	ip->from_addr = ip_addr;

	udp->len = htons(len);
	udp->cksum = 0;
	udp->cksum = ip_cksum(len + IP_PROTOCOL_UDP, 
		(uint8_t*)udp-8, len+8);

	return ip_send(frame, len);
}

// reply to UDP packet
// len is UDP data payload length
void udp_reply(eth_frame_t *frame, uint16_t len)
{
	ip_packet_t *ip = (void*)(frame->data);
	udp_packet_t *udp = (void*)(ip->data);
	uint16_t temp;

	len += sizeof(udp_packet_t);

	temp = udp->from_port;
	udp->from_port = udp->to_port;
	udp->to_port = temp;

	udp->len = htons(len);

	udp->cksum = 0;
	udp->cksum = ip_cksum(len + IP_PROTOCOL_UDP, 
		(uint8_t*)udp-8, len+8);

	ip_reply(frame, len);
}