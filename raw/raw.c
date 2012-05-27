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
#include <fcntl.h>
#include <linux/if_packet.h>

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

int raw_send(char *net_i, uint16_t ether_type, uint8_t *ptr, int len)
{
    int fd;
    int ifindex = 0;
    uint8_t *buf = NULL;
    struct sockaddr_ll sll;
    uint16_t n_ethtype;
    char dmac[] = {0x08, 0x00, 0x27, 0x00, 0x60, 0x5A};

    printf("line: %d.\n", __LINE__);
    if (net_i) {
        if ((ifindex = get_ifindex(net_i)) <= 0) {
            return (-1);
        }
    } else {
        if ((ifindex = get_ifindex("lo")) <= 0) {
            return (-1);
        }
    }
    n_ethtype = htons(ether_type);
    if ((fd = socket(AF_PACKET, SOCK_RAW, n_ethtype)) < 0) {
        return fd;
    }
    printf("line: %d.\n", __LINE__);

    bzero(&sll, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_halen = 6;
    memcpy(sll.sll_addr, dmac, 6);
    sll.sll_ifindex = ifindex;
    sll.sll_protocol = n_ethtype;

    bind(fd, (struct sockaddr *)&sll, sizeof(sll));
    buf = malloc(len + 14);
    memcpy(buf, dmac, 6);
    memcpy(buf + 12, &n_ethtype, 2);
    memcpy(buf + 14, ptr, len);
    printf("line: %d.\n", __LINE__);
    if (sendto(fd, buf, len + 14, 0, (struct sockaddr *)&sll, sizeof(sll)) < 0) {
        perror("sendto error.\n");
        return -1;
    }
    printf("line: %d.\n", __LINE__);
    free(buf);
    return fd;
}
