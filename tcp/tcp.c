
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define TEST_MSG       "come on, baby, please love me!" 
#define MAXDATASIZE    100

static int tcp_debug = 0;

#define tcp_print(fmt, args...)    \
    do {                           \
        if (tcp_debug)             \
            printf(fmt, ##args);   \
    } while(0)

int set_tcp_debug(int log)
{
    tcp_debug = log;
    return 0;
}

int tcp_connect(uint16_t dst_port, uint32_t dst_ip, uint32_t m_s)
{
    int fd;
    struct timeval timeo = {3, 0};
    struct sockaddr_in server;

    tcp_print("............line:%d.\n", __LINE__);
    /*Create TCP socket */
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        /*handle exception */
        perror("Creating socket failed");
        exit(1);
    }
    tcp_print("............line:%d.\n", __LINE__);
    if (m_s) {
        timeo.tv_sec = m_s / 1000;
        timeo.tv_usec = (m_s % 1000) * 1000;
    }
    tcp_print("............line:%d.\n", __LINE__);
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo));
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(dst_port);
    server.sin_addr.s_addr = htonl(dst_ip);

    tcp_print("............line:%d.\n", __LINE__);
    if (connect(fd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {
        perror("connect() error\n");
        exit(1);
    }
    tcp_print("............line:%d.\n", __LINE__);

    return 0;
}

int tcp_connect_send(uint16_t dst_port, uint32_t dst_ip, char *msg)
{
    int fd;
    int numbytes;
    char buf[MAXDATASIZE];
    struct sockaddr_in server;

    tcp_print("............line:%d.\n", __LINE__);
    /*Create TCP socket */
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        /*handle exception */
        perror("Creating socket failed");
        exit(1);
    }
    tcp_print("............line:%d.\n", __LINE__);
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(dst_port);
    server.sin_addr.s_addr = htonl(dst_ip);

    if (connect(fd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {
        perror("connect() error\n");
        exit(1);
    }

    tcp_print("............line:%d.\n", __LINE__);
    /*send something */
    if (msg == NULL) {
        sendto(fd, TEST_MSG, strlen(TEST_MSG), 0, (struct sockaddr *)&server, sizeof(server));
    } else {
        sendto(fd, msg, strlen(msg), 0, (struct sockaddr *)&server, sizeof(server));
    }
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
