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

static uint32_t sp_show_version;
static uint32_t sp_show_help;
static uint32_t sp_show_configure;
static uint32_t sp_quiet_mode;
static uint8_t *sp_conf_file;
#define RAW_SOCKET 1
#define TCP_SOCKET 2
#define UDP_SOCKET 3

#define SP_OK      0
#define SP_ERROR   1

static int get_options(int argc, char *const *argv)
{
    uint8_t *p;
    int i;
    int result = 0;
    struct in_addr inp;
    char *tmp_arg;

    for (i = 1; i < argc; i++) {
        p = (uint8_t *) argv[i];

        if (*p++ != '-') {
            sprintf(0, "invalid option: \"%s\"", argv[i]);
            return 1;
        }

        while (*p) {
            switch (*p++) {
            case '?':
            case 'h':
                sp_show_version = 1;
                sp_show_help = 1;
                break;

            case 'v':
                sp_show_version = 1;
                break;

            case 'V':
                sp_show_version = 1;
                sp_show_configure = 1;
                break;

            case 'q':
                sp_quiet_mode = 1;
                break;
            case 'D':
                if (*p) {
                    tmp_arg = (char *)p;
                    result = inet_aton(tmp_arg, &inp);
                    if (result == 0) {
                        printf("wrong format of dst ip, %s.\n", tmp_arg);
                        return SP_ERROR;
                    }
                    dst_ip = ntohl((uint32_t)(inp.s_addr));
                    goto next;
                }
                if (argv[++i]) {
                    tmp_arg = (char *) argv[i];
                    result = inet_aton(tmp_arg, &inp);
                    if (result == 0) {
                        printf("wrong format of dst ip, %s.\n", tmp_arg);
                        return SP_ERROR;
                    }
                    dst_ip = ntohl((uint32_t)(inp.s_addr));
                    goto next;
                }
                printf("option \"-D\" requires dst ip.\n");
                return SP_ERROR;
            case 'd':
                if (*p) {
                    tmp_arg = (char *)p;
                    dst_port = (uint16_t)atoi(tmp_arg);
                    goto next;
                }
                if (argv[++i]) {
                    tmp_arg = (char *) argv[i];
                    dst_port = (uint16_t)atoi(tmp_arg);
                    goto next;
                }
                printf("option \"-d\" requires dst port.\n");
                return SP_ERROR;
            case 'S':
                if (*p) {
                    tmp_arg = (char *)p;
                    result = inet_aton(tmp_arg, &inp);
                    if (result == 0) {
                        printf("wrong format of dst ip, %s.\n", tmp_arg);
                        return SP_ERROR;
                    }
                    src_ip = ntohl((uint32_t)(inp.s_addr));
                    goto next;
                }
                if (argv[++i]) {
                    tmp_arg = (char *) argv[i];
                    result = inet_aton(tmp_arg, &inp);
                    if (result == 0) {
                        printf("wrong format of dst ip, %s.\n", tmp_arg);
                        return SP_ERROR;
                    }
                    src_ip = ntohl((uint32_t)(inp.s_addr));
                    goto next;
                }
                printf("option \"-S\" requires dst ip.\n");
                return SP_ERROR;
            case 's':
                if (*p) {
                    tmp_arg = (char *)p;
                    src_port = (uint16_t)atoi(tmp_arg);
                    goto next;
                }
                if (argv[++i]) {
                    tmp_arg = (char *) argv[i];
                    src_port = (uint16_t)atoi(tmp_arg);
                    goto next;
                }
                printf("option \"-s\" requires src port.\n");
                return SP_ERROR;
            case 't':
                if (strncmp("tcp", optarg, 3) == 0) {
                    send_type = TCP_SOCKET;
                } else if (strncmp("udp", optarg, 3) == 0) {
                    send_type = UDP_SOCKET;
                } else if (strncmp("raw", optarg, 3) == 0) {
                    send_type = RAW_SOCKET;
                }
                break;
            case 'c':
                if (*p) {
                    sp_conf_file = p;
                    goto next;
                }

                if (argv[++i]) {
                    sp_conf_file = (uint8_t *) argv[i];
                    goto next;
                }

                printf("option \"-c\" requires file name.\n");
                return SP_ERROR;

            default:
                printf("invalid option: \"%c\"", *(p - 1));
                return SP_ERROR;
            }
        }
      next:
        continue;
    }

    return SP_OK;
}

int main(int argc, char *const *argv)
{
    if (get_options(argc, argv) != SP_OK) {
        return 1;
    }
    printf("dst ip:   %x.\n", dst_ip);
    printf("src ip:   %x.\n", src_ip);
    printf("dst port: %x.\n", dst_port);
    printf("src port: %x.\n", src_port);
    /*
    ta();
    tb();
    test2(dst_port, dst_ip);
    */
    return 0;
}
