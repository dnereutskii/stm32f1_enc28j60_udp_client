#ifndef ARP_H
#define ARP_H

#define ARP_CACHE_SIZE		3

#define ARP_HW_TYPE_ETH     htons(0x0001)
#define ARP_PROTO_TYPE_IP   htons(0x0800)

#define ARP_TYPE_REQUEST    htons(1)
#define ARP_TYPE_RESPONSE   htons(2)

/**
 * @brief ARP-message structure
 * 
 */
#pragma pack(push, 1)
typedef struct arp_message {
    uint16_t hw_type;           /*!< Data link protocol (OSI-2) -> Ethernet */
    uint16_t proto_type;        /*!< Network protocol (OSI-3) -> IP */
    uint8_t hw_addr_len;        /*!< MAC-address length = 6 */
    uint8_t proto_addr_len;     /*!< IP-address length = 4 */
    uint16_t type;              /*!< Message type: request or response*/
    uint8_t mac_addr_from[6];   /*!< Source MAC-address */
    uint32_t ip_addr_from;      /*!< Source IP-address */
    uint8_t mac_addr_to[6];     /*!< Destination MAX-address  */
    uint32_t ip_addr_to;        /*!< Destination IP-address */
} arp_message_t;
#pragma pack(pop)

/**
 * @brief ARP-cache structure
 * 
 */
#pragma pack(push, 1)
typedef struct arp_cache_entry {
    uint32_t ip_addr;       /*!< IP-address */
    uint8_t mac_addr[6];    /*!< MAC-address */
} arp_cache_entry_t;
#pragma pack(pop)

/**
 * @brief ARP message analyser.
 * 
 * The function checkout of datalink and protocol types, forms a response 
 * to ARP-request.
 * 
 * @param frame Ethernet frame
 * @param len   ARP-message length
 */
void arp_filter(eth_frame_t *frame, uint16_t len);

/**
 * @brief Resolve MAC address
 * 
 * @param node_ip_addr IP-address to be looking for
 * @return uint8_t* Pointer to array of desired 
 * @retval 0 if still resolving
 */
uint8_t *arp_resolve(uint32_t node_ip_addr);
// 
// returns 0 if still resolving
// (invalidates net_buffer if not resolved)
#endif /*ARP_H*/