#define __LINUX_SYS__

#ifdef __LINUX_SYS__
#include <linux/types.h>
#include <stdint.h>
#endif
#include <string.h>
#include "packet_about.h"

void swap_ip_addr(uint8_t *ptr)
{
    uint32_t s;
    s = *(uint32_t *)(ptr + 26);
    *(uint32_t *)(ptr + 26) = *(uint32_t *)(ptr + 30);
    *(uint32_t *)(ptr + 30) = s;
}

void swap_ip_addr_arp(uint8_t *ptr)
{
    uint32_t ip = 0;
    ip = *((uint32_t*)(ptr + 28));
    *((uint32_t*)(ptr + 28)) = *((uint32_t*)(ptr + 38));
    *((uint32_t*)(ptr + 38)) = ip;

}

void swap_port(uint8_t *buf)
{
    uint16_t tmp = *(uint16_t *)(buf + 34);
    *(uint16_t *)(buf + 34) = *(uint16_t *)(buf + 36);
    *(uint16_t *)(buf + 36) = tmp;
}

void swap_mac_addr(uint8_t *pkt_ptr)
{
    uint16_t s;
    uint32_t w;

    s = *((uint16_t*)pkt_ptr);
    w = *((uint32_t*)(pkt_ptr + 2));
    *(uint16_t*)pkt_ptr = *((uint16_t*)(pkt_ptr + 6));
    *((uint32_t*)(pkt_ptr + 2)) = *((uint32_t*)(pkt_ptr + 8));
    *((uint16_t*)(pkt_ptr + 6)) = s;
    *((uint32_t*)(pkt_ptr + 8)) = w;
}

uint16_t ip_checksum(uint16_t *ip_hdr)
{
    int i = 0;
    uint32_t checksum = *ip_hdr;
    for ( i = 1; i < 10; i++ ) {
        checksum +=  *(ip_hdr + i);
        checksum = (checksum & 0xffff) + (checksum >> 16);
    }
    return 0xffff - checksum;
}

void set_ip_len_checksum(uint8_t *ip_ptr, int length)
{
    ip_hdr_t *ip_hdr;
    ip_hdr = (ip_hdr_t *)ip_ptr;
    ip_hdr->length = length;
    ip_hdr->checksum = 0;
    ip_hdr->checksum = ip_checksum((uint16_t *)ip_ptr);

}

void set_udp_len_checksum(uint8_t *udp_ptr, int length)
{
    udp_hdr_t *udp_hdr;
    udp_hdr = (udp_hdr_t *)(udp_ptr);
    udp_hdr->length = length;
    udp_hdr->checksum = 0;
}

uint16_t icmp_checksum_ping(uint16_t *icmp_hdr)
{
    int i = 0;
    uint32_t checksum = *icmp_hdr;
    for ( i = 1; i < 64; i++ ) {
        checksum +=  *(icmp_hdr + i);
        checksum = ( checksum & 0xffff ) + ( checksum >> 16 );
    }
    return 0xffff - checksum;
}

#ifndef __LINUX_SYS__
uint32_t inet_addr(const char *ip)
{
    int j = 0, i = 0;
    int len = strlen(ip);
    char tmp[4] = {0};
    uint32_t ip_addr = 0;
    while(i < len) {
        i++;
        if(*ip != '.') {
            tmp[j++] = *ip;
        } else {
            ip_addr = ip_addr + atoi(tmp);
            ip_addr = ip_addr * 256;
            memset(tmp, 0, 4);
            j = 0;
        }
        ip++;
    }
    ip_addr = ip_addr + atoi(tmp);
    return ip_addr;
}
#endif
