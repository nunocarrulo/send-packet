
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MAXDATASIZE    100

int main(uint32_t dst_ip, uint16_t dst_port)
{
    int fd;
    int numbytes;
    char buf[MAXDATASIZE];
    struct sockaddr_in server;
    struct hostent *he;

    if (argc != 2) {
        printf("Usage: %s <IP Address>\n", argv[0]);
        exit(1);
    }

    /*Create TCP socket */
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        /*handle exception */
        perror("Creating socket failed");
        exit(1);
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(dst_port);
    server.sin_addr.s_addr = htonl(dst_ip);

    if (connect(fd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {
        perror("connect() error\n");
        exit(1);
    }

    /*send something */
    sendto(fd, "counter", strlen("counter"), 0, (struct sockaddr *)&server, sizeof(server));
    /*Recv welcome message */
    if ((numbytes = recv(fd, buf, MAXDATASIZE, 0)) == -1) {
        perror("recv() error\n");
        exit(1);
    }

    buf[numbytes] = '\0';
    printf("Server Message: %s\n", buf);

    close(fd);
    return 0;
}
