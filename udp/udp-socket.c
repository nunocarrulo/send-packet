#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>

#define PORT           1234
#define MAXDATASIZE    1024

main(int argc, char ** argv)
{
    int                    fd;
    int                    numbytes;
    char                buf[MAXDATASIZE];
    struct hostent *    he;
    struct sockaddr_in    server;                //server's address information
    struct sockaddr_in    reply;
    int                 len;
    
    if(argc != 3)
    {
        printf("Usage: %s <IP Address> <message>\n", argv[0]);
        exit(1);
    }
    
    if((he = gethostbyname(argv[1])) == NULL)
    {
        printf("gethostbyname() error\n");
        exit(1);
    }
    
    /*Create UDP socket*/
    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        /*handle exception*/
        perror("Creating socket failed");
        exit(1);
    }

    bzero(&server, sizeof(server));
    server.sin_family    = AF_INET;
    server.sin_port        = htons(PORT);
//    server.sin_addr = *((struct in_addr *)he->h_addr);
    server.sin_addr.s_addr = inet_addr(argv[1]);

    sendto(fd, argv[2], strlen(argv[2]), 0, 
                (struct sockaddr *)&server, sizeof(struct sockaddr));
                

    while(1)
    {
        /*len必须被初始化为reply的长度*/
        len = sizeof(struct sockaddr);
        numbytes = recvfrom(fd, (void *)buf, MAXDATASIZE, 0,
                            (struct sockaddr *)&reply, (socklen_t *)&len);
        if(numbytes == -1)
        {
            perror("recvfrom() error");
            exit(1);
        }
    
        if(len != sizeof(struct sockaddr) 
            || memcmp((const void *)&server, (const void *)&reply, len) != 0)
        {
            printf("Receive message from other server. \n");
            continue;
        }
    
        buf[numbytes] = '\0';
        /*Prints server's message*/
        printf("Server Message: %s\n", buf);
        
        break;
    }
    
    close(fd);
}

