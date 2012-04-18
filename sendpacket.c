#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include "app.h"

static uint32_t src_ip = 0;
static uint32_t dst_ip = 0;
static uint16_t src_port = 0;
static uint16_t dst_port = 0;
static uint16_t send_type = 0;

#define RAW_SOCKET 1
#define TCP_SOCKET 2
#define UDP_SOCKET 3

int main(int argc, char **argv)
{
    int ch;
    int result = 0;
    struct in_addr inp;
    while ((ch = getopt(argc, argv, "D:d:S:s:t:")) != -1) {
        switch (ch) {
        case 'D':
            printf("option a optarg is :'%s'\n", optarg);
            result = inet_aton(optarg, &inp);
            dst_ip = (uint32_t)(inp.s_addr);
            break;
        case 'd':
            dst_port = (uint16_t)atoi(optarg);
            break;
        case 'S':
            result = inet_aton(optarg, &inp);
            src_ip = (uint32_t)(inp.s_addr);
            break;
        case 's':
            src_port = (uint16_t)atoi(optarg);
            break;
        case 't':
            if (strncmp("tcp", optarg, 3) == 0) {
                send_type = TCP_SOCKET;
            } else if (strncmp("udp", optarg, 3) == 0) {
                send_type = UDP_SOCKET;
            } else if (strncmp("raw", optarg, 3) == 0) {
                send_type = RAW_SOCKET;
            }
            break;
        default:
            break;
        }
    }
    printf("abc, dst ip: %x.\n", dst_ip);
    printf("ssssssssssss +%c\n", optopt);
    printf("in main\n");
    ta();
    tb();
    test2(dst_port, dst_ip);
    return 0;
}
