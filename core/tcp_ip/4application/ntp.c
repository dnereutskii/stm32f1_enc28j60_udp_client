#include <stdio.h>
#include "stm32f1xx.h"
#include "ethernet.h"
#include "ip.h"
#include "udp.h"
#include "ntp.h"

#define TIMESTAMP_OFFSET 2208988800UL

uint8_t ntp_request(uint32_t srv_ip)
{
	eth_frame_t *frame = (void*)net_buf;
	ip_packet_t *ip = (void*)(frame->data);
	udp_packet_t *udp = (void*)(ip->data);
	ntp_message_t *ntp =(void*)(udp->data);

	ip->to_addr = srv_ip;
	udp->to_port = NTP_SRV_PORT;
	udp->from_port = NTP_LOCAL_PORT;

	memset(ntp, 0, sizeof(ntp_message_t));
	ntp->status = 0x08;

	return udp_send(frame, sizeof(ntp_message_t));
}

uint32_t ntp_parse_reply(void *data, uint16_t len)
{
	ntp_message_t *ntp = data;
	uint32_t temp;

	if(len >= sizeof(ntp_message_t))
	{
		temp = ntp->xmit_timestamp.seconds;
		return (ntohl(temp) - TIMESTAMP_OFFSET);
	}
	return 0;
}
