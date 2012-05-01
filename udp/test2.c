#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>

int test2(uint16_t d_port, uint32_t d_ip)
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

    sendto(fd, "123456", strlen("123456"), 0, (struct sockaddr *)&server, sizeof(struct sockaddr));
    close(fd);

    printf("in test2.c\n");
    return 0;
}
