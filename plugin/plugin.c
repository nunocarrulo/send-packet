#include <stdio.h>
#include <stdint.h>

int plugin_test1(void)
{
    printf("plugin test: %s.\n", __func__);
    return 0;  
}  

int plugin_test2(void)
{
    printf("plugin test: %s.\n", __func__);
    return 0;  
}

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

void print_content(uint8_t * ptr, int length)
{
    int i;
    printf("leng: %d.\n", length);
    for (i = 0; i < length; i++) {
        if (i % 16 == 0)
            printf("\n");
        printf("%02x ", ptr[i]);
    }
    printf("\n");
}

int get_ip_checksum(uint8_t *cks_ptr, uint16_t *get_sum)
{
    int i = 0;
    int offset = 0;
    uint8_t *ip_hdr;
    ip_hdr_t *ip_p;
    int hdr_len = 0;
    uint32_t checksum = 0;

    offset = &((ip_hdr_t *)0)->checksum;
    ip_hdr = cks_ptr - offset;
    ip_p = (ip_hdr_t *)ip_hdr;
    ip_p->checksum = 0;
    hdr_len = ip_p->hdr_len * 4;
    //print_content(ip_hdr, hdr_len);
    for (i = 0; i < hdr_len; i += 2) {
        checksum += *(ip_hdr + i + 1);
        checksum += *(ip_hdr + i) << 8;
    }
    while (checksum > 0xffff) {
        checksum = (checksum & 0xffff) + (checksum >> 16);
    }
    *get_sum = ~checksum;
    return 0;
}

