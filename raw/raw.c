#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <endian.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <linux/tcp.h>
#include <linux/sockios.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <fcntl.h>
#include <linux/if_packet.h>
#include "log.h"

static int raw_debug = 0;

#define raw_print(fmt, args...)    \
    do {                           \
        if (raw_debug)             \
            printf(fmt, ##args);   \
    } while(0)

int set_raw_debug(int log)
{
    raw_debug = log;
    return 0;
}

static int get_ifindex(const char *device)
{
    int sock;
    struct ifreq ifr;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return (0);
    }
    memset((char *)&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, device);
    if (ioctl(sock, SIOCGIFINDEX, (char *)&ifr) < 0) {
        perror("ioctl");
    }
    close(sock);
    return ifr.ifr_ifindex;
}

int get_macaddr(const char *device, char *mac)
{
    int32_t sock;
    struct ifreq ifr;

    memset(mac, 0, 6);
    sock = socket(AF_INET,SOCK_DGRAM, 0);
    if (sock < 0 ) {
        perror("socket\n");
        return -1;
    }
    memset((char *)&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, device);
    fflush(stdout);
    if (ioctl(sock, SIOCGIFHWADDR, (char *)&ifr) < 0) {
        printf("mac addr get failed.\n");
        close(sock);
        return -1;
    }
    fflush(stdout);
    memcpy(mac, ifr.ifr_hwaddr.sa_data, 6);
    close(sock);
    return 0;
}

int raw_send(char *net_i, uint16_t ether_type, uint8_t *ptr, int len)
{
    int fd;
    int ifindex = 0;
    uint8_t *buf = NULL;
    char *device = NULL;
    struct sockaddr_ll sll;
    uint16_t n_ethtype;
    char dmac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    char smac[6];

    raw_print("line: %d.\n", __LINE__);
    if (net_i) {
        device = net_i;
        if (get_macaddr(device, smac)) {
            memset(smac, 0x88, 6);
        }
    } else {
        device = "lo";
        memset(smac, 0x88, 6);
    }
    if ((ifindex = get_ifindex(device)) <= 0) {
        return -1;
    }
    n_ethtype = htons(ether_type);
    if ((fd = socket(AF_PACKET, SOCK_RAW, n_ethtype)) < 0) {
        printf("create socket failed.\n");
        return fd;
    }
    raw_print("line: %d.\n", __LINE__);

    bzero(&sll, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_halen = 6;
    //memcpy(sll.sll_addr, dmac, 6);
    sll.sll_ifindex = ifindex;
    sll.sll_protocol = n_ethtype;

    bind(fd, (struct sockaddr *)&sll, sizeof(sll));
    buf = malloc(len + 14);
    memcpy(buf, dmac, 6);
    memcpy(buf + 6, smac, 6);
    memcpy(buf + 12, &n_ethtype, 2);
    memcpy(buf + 14, ptr, len);
    raw_print("line: %d.\n", __LINE__);
    if (sendto(fd, buf, len + 14, 0, (struct sockaddr *)&sll, sizeof(sll)) < 0) {
        perror("sendto error.\n");
        return -1;
    }
    raw_print("line: %d.\n", __LINE__);
    free(buf);
    close(fd);
    return 0;
}

int raw_send_all(char *net_i, uint8_t *ptr, int len)
{
    int fd;
    int ifindex = 0;
    char *device = NULL;
    struct sockaddr_ll sll;

    raw_print("line: %d.\n", __LINE__);
    condition_if_false_ret(ptr != NULL, -1);
    if (net_i) {
        device = net_i;
    } else {
        device = "lo";
    }
    if ((ifindex = get_ifindex(device)) <= 0) {
        return -1;
    }
    if ((fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
        printf("create socket failed.\n");
        return fd;
    }
    raw_print("line: %d.\n", __LINE__);

    bzero(&sll, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_halen = 6;
    //memcpy(sll.sll_addr, dmac, 6);
    sll.sll_ifindex = ifindex;
    sll.sll_protocol = htons(ETH_P_ALL);

    bind(fd, (struct sockaddr *)&sll, sizeof(sll));
    raw_print("line: %d.\n", __LINE__);
    if (sendto(fd, ptr, len, 0, (struct sockaddr *)&sll, sizeof(sll)) < 0) {
        perror("sendto error.\n");
        return -1;
    }
    raw_print("line: %d.\n", __LINE__);
    close(fd);
    return 0;
}
