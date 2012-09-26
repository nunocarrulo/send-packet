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
#include "sendpacket.h"
#include "raw.h"
#include "udp.h"
#include "tcp.h"
#include "parser_cfg.h"

static uint32_t timeout = 0;
static uint32_t src_ip = 0;
static uint32_t dst_ip = 0;
static uint16_t src_port = 0;
static uint16_t dst_port = 0;
static uint16_t send_type = 0;
static uint16_t tcp_send_type = 0;
static uint16_t ethertype = 0;

static uint32_t sp_show_version;
static uint32_t sp_show_help;
static uint32_t sp_show_configure;
static uint32_t sp_quiet_mode;
static uint8_t *sp_conf_file = NULL;
static char    *interface = NULL;

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
            tmp_arg = NULL;
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
                } else if (argv[++i]) {
                    tmp_arg = (char *) argv[i];
                }
                if (tmp_arg == NULL) {
                    printf("option \"-D\" requires dst ip.\n");
                    return SP_ERROR;
                }
                result = inet_aton(tmp_arg, &inp);
                if (result == 0) {
                    printf("wrong format of dst ip, %s.\n", tmp_arg);
                    return SP_ERROR;
                }
                dst_ip = ntohl((uint32_t)(inp.s_addr));
                goto next;
            case 'd':
                if (*p) {
                    tmp_arg = (char *)p;
                } else if (argv[++i]) {
                    tmp_arg = (char *) argv[i];
                }
                if (tmp_arg == NULL) {
                    printf("option \"-d\" requires dst port.\n");
                    return SP_ERROR;
                }
                dst_port = (uint16_t)atoi(tmp_arg);
                goto next;
            case 'S':
                if (*p) {
                    tmp_arg = (char *)p;
                } else if (argv[++i]) {
                    tmp_arg = (char *) argv[i];
                }
                if (tmp_arg == NULL) {
                    printf("option \"-S\" requires dst ip.\n");
                    return SP_ERROR;
                }
                result = inet_aton(tmp_arg, &inp);
                if (result == 0) {
                    printf("wrong format of dst ip, %s.\n", tmp_arg);
                    return SP_ERROR;
                }
                src_ip = ntohl((uint32_t)(inp.s_addr));
                goto next;
            case 's':
                if (*p) {
                    tmp_arg = (char *)p;
                } else if (argv[++i]) {
                    tmp_arg = (char *) argv[i];
                }
                if (tmp_arg == NULL) {
                    printf("option \"-s\" requires src port.\n");
                    return SP_ERROR;
                }
                src_port = (uint16_t)atoi(tmp_arg);
                goto next;
            case 'T':
                if (*p) {
                    tmp_arg = (char *)p;
                } else if (argv[++i]) {
                    tmp_arg = (char *) argv[i];
                }
                if (tmp_arg == NULL) {
                    printf("option \"-T\" requires timeout(int ms).\n");
                    return SP_ERROR;
                }
                timeout = (uint32_t)atoi(tmp_arg);
                goto next;
            case 't':
                if (*p) {
                    tmp_arg = (char *)p;
                } else if (argv[++i]) {
                    tmp_arg = (char *) argv[i];
                }
                if (tmp_arg == NULL) {
                    printf("option \"-t\" requires packet type.\n");
                    return SP_ERROR;
                }
                if (strncmp("tcp", tmp_arg, 3) == 0) {
                    send_type = TCP_SOCKET;
                } else if (strncmp("udp", tmp_arg, 3) == 0) {
                    send_type = UDP_SOCKET;
                } else if (strncmp("raw", tmp_arg, 3) == 0) {
                    send_type = RAW_SOCKET;
                }
                break;
             case 'p':
                if (*p) {
                    tmp_arg = (char *)p;
                } else if (argv[++i]) {
                    tmp_arg = (char *) argv[i];
                }
                if (tmp_arg == NULL) {
                    printf("option \"-p\" requires tcp packets type.\n");
                    return SP_ERROR;
                }
                if (strncmp("c", tmp_arg, 1) == 0) {
                    tcp_send_type = TCP_CNCT;
                } else if (strncmp("s", tmp_arg, 1) == 0) {
                    tcp_send_type = TCP_SND;
                } else if (strncmp("all", tmp_arg, 3) == 0) {
                    tcp_send_type = TCP_CNCT_SND;
                }
                break;
             case 'L':
                if (*p) {
                    tmp_arg = (char *)p;
                } else if (argv[++i]) {
                    tmp_arg = (char *) argv[i];
                }
                if (tmp_arg == NULL) {
                    printf("option \"-L\" requires all/raw/tcp/udp/config open.\n");
                    return SP_ERROR;
                }
                if (strncmp("raw", tmp_arg, 3) == 0) {
                } else if (strncmp("tcp", tmp_arg, 3) == 0) {
                    set_tcp_debug(1);
                } else if (strncmp("udp", tmp_arg, 3) == 0) {
                } else if (strncmp("config", tmp_arg, 6) == 0) {
                    set_parser_debug(1);
                } else if (strncmp("all", tmp_arg, 3) == 0) {
                    set_parser_debug(1);
                    set_tcp_debug(1);
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
             case 'e':
                if (*p) {
                    tmp_arg = (char *)p;
                } else if (argv[++i]) {
                    tmp_arg = (char *) argv[i];
                }
                if (tmp_arg == NULL) {
                    printf("option \"-e\" requires ether type.\n");
                    return SP_ERROR;
                }
                ethertype = (uint32_t)strtoul(tmp_arg, NULL, 0);
                goto next;
             case 'i':
                if (*p) {
                    interface = (char *)p;
                    goto next;
                }
                if (argv[++i]) {
                    interface = argv[i];
                    goto next;
                }
                printf("option \"-i\" requires interface name.\n");
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

void show_help()
{
    printf("Usage: sendpacket [-?hvV] [-s src_port] [-d dst_port]\n"
           "                  [-S src_ip] [-D dst_ip] [-t tcp/udp/raw]\n"
           "                  [-p c/s/all] [-T timeout] [-e ether_type]\n"
           "                  [-L all/raw/tcp/udp/config]\n"
           "                  [-i interface] [-c config_file]\n");
}

int main(int argc, char *const *argv)
{
    int ret = 0;
    if (get_options(argc, argv) != SP_OK) {
        return 1;
    }
    if (sp_show_version) {
        printf("sendpacket version: %s.\n", SP_VER);
        if (sp_show_help) {
            show_help();
        }
        if (sp_show_configure) {
        }
        return 0;
    }
#if 0
    printf("time out: %u.\n", timeout);
    printf("dst ip:   0x%x.\n", dst_ip);
    printf("src ip:   0x%x.\n", src_ip);
    printf("dst port: 0x%x, %u.\n", dst_port, dst_port);
    printf("src port: 0x%x, %u.\n", src_port, src_port);
#endif
    if (sp_conf_file) {
        char buf[2048];
        memset(buf, 0, 2048);
        if (access((char *)sp_conf_file, F_OK)) {
            printf("file %s not exit.\n", sp_conf_file);
            return 1;
        }
        //printf("config file: %s.\n", sp_conf_file);
        ret = parser_config((char *)sp_conf_file, buf, 2048);
        if (ret) {
            printf("config file not valid.\n");
        }
        return 0;
    }
    /*
    ta();
    tb();
    */
    if (send_type == TCP_SOCKET) {
        if (tcp_send_type == TCP_CNCT_SND) {
            tcp_connect_send(dst_port, dst_ip, NULL);
        } else if (tcp_send_type == TCP_CNCT) {
            tcp_connect(dst_port, dst_ip, timeout);
        } else if (tcp_send_type == TCP_SND) {
        }
    } else if (send_type == UDP_SOCKET) {
        udp_send(dst_port, dst_ip);
    } else {
        raw_send(interface, ethertype, (uint8_t *)"xuchunxiao", 10);
    }
    return 0;
}
