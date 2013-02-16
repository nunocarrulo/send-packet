#ifndef _PA_PACKET_H_
#define _PA_PACKET_H_

#define L2_LEN        14
#define IP_LEN        20
#define UDP_LEN       8
#define TCP_LEN       20

#define UDP_HEAD_LEN  42
#define TCP_HEAD_LEN  54

#define MAC_IN_MAC_TAG          0x0810
#define PROTOCOL_VALUE_VLAN     0x8100
#define PROTOCOL_VALUE_IP       0x0800
#define PROTOCOL_VALUE_IPV6     0x86DD

typedef struct _ip_hdr {
#if __BYTE_ORDER == __BIG_ENDIAN
    uint8_t hdr_len : 4;
    uint8_t version : 4;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
    uint8_t version : 4;
    uint8_t hdr_len : 4;
#else
#error  "Please fix share/packet_about.h"
#endif
    uint8_t tos;
    uint16_t length;
    uint16_t id;
#if __BYTE_ORDER == __BIG_ENDIAN
    uint16_t frag_flags : 3;
    uint16_t frag_offset : 13;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
    uint16_t frag_offset : 13;
    uint16_t frag_flags : 3;
#else
#error  "Please fix share/packet_about.h"
#endif
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t src_ip;
    uint32_t dst_ip;
} ip_hdr_t;

typedef struct _udp_hdr {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
} udp_hdr_t;

typedef struct _tcp_hdr {
	uint16_t src_port;
	uint16_t dst_port;
	uint32_t seq;
	uint32_t ack;
	uint8_t hdr_len : 4;
	uint8_t reserved1 : 4;
	uint8_t reserved2 : 2;
	uint8_t f_urg : 1;
	uint8_t f_ack : 1;
	uint8_t f_psh : 1;
	uint8_t f_rst : 1;
	uint8_t f_syn : 1;
	uint8_t f_fin : 1;
	uint16_t window_size;
	uint16_t checksum;
	uint16_t urg_ptr;
} tcp_hdr_t;

typedef struct _l4_hdr {
	uint16_t src_port;
	uint16_t dst_port;
} l4_hdr_t;

void swap_ip_addr(uint8_t  *ptr);
void swap_ip_addr_arp(uint8_t *ptr);
void swap_port(uint8_t *buf);
void swap_mac_addr(uint8_t *pkt_ptr);
uint16_t ip_checksum(uint16_t*ip_hdr);
uint16_t icmp_checksum_ping(uint16_t *icmp_hdr);

void set_ip_len_checksum(uint8_t *ip_ptr, int length);
void set_udp_len_checksum(uint8_t *udp_ptr, int length);
uint32_t inet_addr(const char *ip);
#endif
