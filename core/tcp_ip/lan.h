#ifndef LAN_H
#define LAN_H

/*
 * Config
 */

#define WITH_ICMP

#define MAC_ADDR            {0x00,0x13,0x37,0x01,0x23,0x45}
#define IP_ADDR	            inet_addr(192, 168, 0, 222)
#define IP_SUBNET_MASK		inet_addr(255,255,255,0)
#define IP_DEFAULT_GATEWAY	inet_addr(192,168,0,1)

#define IP_PACKET_TTL       64

extern uint8_t net_buf[];

void lan_init();
void lan_poll();


#endif /*LAN_H*/