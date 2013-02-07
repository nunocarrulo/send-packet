#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>

int udp_send(uint16_t d_port, uint32_t d_ip, char *snd_msg)
{
    int fd;
    struct sockaddr_in server;

    /*Create UDP socket */
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Creating socket failed");
        exit(1);
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(d_port);
    server.sin_addr.s_addr = htonl(d_ip);

    printf("in %s, msg: %s.\n", __func__, snd_msg);
    if (strlen(snd_msg) == 0) {
        sendto(fd, "xuchunxiao", strlen("xuchunxiao"), 0, (struct sockaddr *)&server, sizeof(struct sockaddr));
    } else {
        sendto(fd, snd_msg, strlen(snd_msg), 0, (struct sockaddr *)&server, sizeof(struct sockaddr));
    }
    close(fd);

    return 0;
}
