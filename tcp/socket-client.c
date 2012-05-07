
/*
 * TCP客户端
 */
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT           4811
#define MAXDATASIZE    100
 int main(int argc, char **argv)
{
    int fd;
    int numbytes;
    char buf[MAXDATASIZE];
    struct sockaddr_in server; //server's address information
    struct hostent *he;
    if (argc != 2) {
        printf("Usage: %s <IP Address>\n", argv[0]);
        exit(1);
    }
    if ((he = gethostbyname(argv[1])) == NULL) {
        printf("gethostbyname() error\n");
        exit(1);
    }

    /*Create TCP socket */
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
         {

        /*handle exception */
        perror("Creating socket failed");
        exit(1);
    }
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    /*下面一句有必要做网络字节序到主机字节序的转化？ */
//    server.sin_addr = *((struct in_addr *)he->h_addr);
    server.sin_addr.s_addr = inet_addr(argv[1]);
    if (connect(fd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
         {

        /*handle exception */
        perror("connect() error\n");
        exit(1);
        }

    /*send something */
    sendto(fd, "counter", strlen("counter"), 0, (struct sockaddr *)&server, sizeof(server));

    /*Recv welcome message */
    if ((numbytes = recv(fd, buf, MAXDATASIZE, 0)) == -1)
         {
        perror("recv() error\n");
        exit(1);
        }
    buf[numbytes] = '\0';
    printf("Server Message: %s\n", buf);
    close(fd);
    return 0;
}

/******************************
1．建立socket
2．将该socket设置为非阻塞模式
3．调用connect()
4．使用select()检查该socket描述符是否可写（注意，是可写）
5．根据select()返回的结果判断connect()结果
6．将socket设置为阻塞模式（如果你的程序不需要用阻塞模式的，
   这步就省了，不过一般情况下都是用阻塞模式的，这样也容易管理）
******************************/

#include <sys/socket.h>
#include <sys/types.h>

#define TIME_OUT_TIME 20        //connect超时时间20秒
int main(int argc, char **argv)
{
    ………………int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        exit(1);
    struct sockaddr_in serv_addr;
    ………                  //以服务器地址填充结构serv_addr
    int error = -1, len;
    len = sizeof(int);
    timeval tm;
    fd_set set;
    unsigned long ul = 1;
    ioctl(sockfd, FIONBIO, &ul);   //设置为非阻塞模式
    bool ret = false;
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
         {
        tm.tv_set = TIME_OUT_TIME;
        tm.tv_uset = 0;
        FD_ZERO(&set);
        FD_SET(sockfd, &set);
        if (select(sockfd + 1, NULL, &set, NULL, &tm) > 0)
             {
            getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *) & len);
            if (error == 0)
                ret = true;

            else
                ret = false;
        } else
            ret = false;
        }

    else
        ret = true;
    ul = 0;
    ioctl(sockfd, FIONBIO, &ul);   //设置为阻塞模式
    if (!ret)
         {
        close(sockfd);
        fprintf(stderr, "Cannot Connect the server!\n");
        return;
        }
    fprintf(stderr, "Connected!\n");

    //下面还可以进行发包收包操作
}
