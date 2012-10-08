/**
 * @file parser_cfg.c
 * @Synopsis parser config file
 * @author xuchunxiao369@gmail.com
 * @version
 * @date 2012-10-08
 */
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
#include <endian.h>
#include "log.h"

#define MAXDATASIZE  2048
#define PARSER_FILE_DEPTH 4096 
#define CONFIG_UNVALID 1
#define MAX_PARSER_BITS 32
#define MAX_CALC_BITS   64

static int parser_debug = 0;

#define parser_print(fmt, args...) \
    do {                           \
        if (parser_debug)          \
            printf(fmt, ##args);   \
    } while(0)

int set_parser_debug(int log)
{
    parser_debug = log;
    return 0;
}

/* get value from the second word of the buf, 
 * this function is not safe in multi-thread programm
 */
static int get_value(char *buf, uint64_t *value)
{
    char *p = NULL;
    int count = 0;
    p = strtok(buf, " ");
    while (p) {
        count++;
        if (count == 2) {
            //parser_print("p: %s.\n", p);
            *value = (uint64_t)strtoul(p, NULL, 0);
            return 0;
        }
        p = strtok(NULL, " ");
    }

    return -1;
}

static int check_config(char *filename)
{
    FILE *fp;
    int n_bit = 0;
    int n_char = 0;
    int size = 0;
    int ret = 0;
    int aligned = 1;
    int started = 0;
    char buf[MAXDATASIZE] = {0};

    parser_print("addr line or data line %d bits.\n", MAX_PARSER_BITS);

    if ((fp = fopen(filename, "r")) == NULL)
        return 2;
    while (fgets(buf, MAXDATASIZE, fp)) {
        /* TODO need strip the buf's head blank */
        if (buf[0] == '\n') {
            continue;
        }
        if (buf[0] == '#') {
            continue;
        }
        if (strncmp(buf, "seg_start", 9) == 0) {
            started = 1;
        } else if (strncmp(buf, "seg_end", 7) == 0) {
            started = 0;
            aligned = 1;
        } else if (strncmp(buf, "notalign", 7) == 0) {
            aligned = 0;
        }
        if (aligned != 1 || started == 0) {
            if (strncmp(buf, "bits", 4) == 0) {
                n_bit = atoi(buf + 4);
                parser_print("bits: %d.\n", n_bit);
                if (n_bit % 8) {
                    printf("Not aligned content must be divided exactly by 8.\n");
                    ret = CONFIG_UNVALID;
                    goto error;
                }
            }
            continue;
        }
        //parser_print("%s", buf);
        if (strncmp(buf, "bits", 4) == 0) {
            n_bit = atoi(buf + 4);
            parser_print("bits: %d.\n", n_bit);
            size += n_bit;
            if (size > MAX_CALC_BITS) {
                ret = CONFIG_UNVALID;
                goto error;
            }
            if (size == MAX_PARSER_BITS) {
                //parser_print("that is a word, oh hawhaw.\n");
                size = 0;
            } else if (size > MAX_PARSER_BITS && (size % MAX_PARSER_BITS == 0)) {
                size = 0;
            } else if (size > MAX_PARSER_BITS) {
                printf("content not %d bits aligned, exit.\n", MAX_PARSER_BITS);
                ret = CONFIG_UNVALID;
                goto error;
            }
        } else if (strncmp(buf, "string", 6) == 0) {
            if (size) {
                ret = CONFIG_UNVALID;
                goto error;
            }
            n_char = atoi(buf + 6);
            parser_print("char: %d.\n", n_char);
            /* check if the string is n_char bytes */
            /*
            ret = CONFIG_UNVALID;
            goto error;
            */
        }
        memset(buf, 0, MAXDATASIZE);
    }
error:
    fclose(fp);
    return ret;
}

int parser_config(char *filename, char *ptr, int len, int *get_len)
{
    FILE *fp;
    int n_bit = 0;
    int n_char = 0;
    int size = 0;
    int ret = 0;
    char *tmp_ptr = ptr;
    uint32_t data_32 = 0;
    uint64_t data_64 = 0;
    uint64_t tmp_data = 0;
    char buf[MAXDATASIZE] = {0};
    int aligned = 1;
    int started = 0;

    data_64 = data_64;
    data_32 = data_32;
    len = len;
    condition_if_true_ret(ptr == NULL, 1);
    condition_if_false_ret(len > 0, 2);

    ret = check_config(filename);
    condition_if_false_ret(ret == 0, ret);

    parser_print("------------------------------------\n");

    if ((fp = fopen(filename, "r")) == NULL)
        return 2;
    while (fgets(buf, MAXDATASIZE, fp)) {
        /* TODO need strip the buf's head blank */
        if (buf[0] == '\n') {
            continue;
        }
        if (buf[0] == '#') {
            continue;
        }
        if (strncmp(buf, "seg_start", 9) == 0) {
            started = 1;
        } else if (strncmp(buf, "seg_end", 7) == 0) {
            started = 0;
            aligned = 1;
        } else if (strncmp(buf, "notalign", 7) == 0) {
            aligned = 0;
        }
        if (aligned != 1 || started == 0) {
            /* get value from not aligned seg */
            if (strncmp(buf, "bits", 4) == 0) {
                n_bit = atoi(buf + 4);
                ret = get_value(buf, &tmp_data);
                tmp_data = (tmp_data << (MAX_CALC_BITS - n_bit));
                parser_print("hawhaw, tmp_data: 0x%lx.\n", tmp_data);
                tmp_data = htobe64(tmp_data);
                parser_print("hawhaw, tmp_data: 0x%lx.\n", tmp_data);
                memcpy(tmp_ptr, (char *)&tmp_data, n_bit >> 3);
                tmp_ptr += (n_bit >> 3);
            } else if (strncmp(buf, "string", 6) == 0) {
                n_char = atoi(buf + 6);
                strncpy(tmp_ptr, strchr(buf, '\"') + 1, n_char);
                tmp_ptr += n_char;
            }
            continue;
        }

        //parser_print("%s", buf);
        /* get value from aligned seg */
        if (strncmp(buf, "bits", 4) == 0) {
            n_bit = atoi(buf + 4);
            //parser_print("bits: %d.\n", n_bit);
            size += n_bit;
            ret = get_value(buf, &tmp_data);
            //parser_print("that is a word, oh hawhaw, tmp data: 0x%lx.\n", tmp_data);
            data_64 |= (tmp_data << (MAX_CALC_BITS - size));
            if (size == MAX_CALC_BITS) {
                //data = ;
                *(uint64_t *)tmp_ptr = htobe64(data_64);
                tmp_ptr += 8;
                parser_print("that is a word, oh hawhaw, data64: 0x%lx.\n", data_64);
                size = 0;
                data_32 = 0;
                data_64 = 0;
            }
        } else if (strncmp(buf, "string", 6) == 0) {
            parser_print("that is a data64 or data 32: 0x%lx.\n", data_64);
            if (size != 0) {
                /* size must be 32 */
                *(uint32_t *)tmp_ptr = htobe32((uint32_t)(data_64 >> 32));
                tmp_ptr += 4;
                size = 0;
                data_32 = 0;
                data_64 = 0;
            }
            n_char = atoi(buf + 6);
            parser_print("char: %d.\n", n_char);
            parser_print("string: %s.\n", strchr(buf, '\"') + 1);
            strncpy(tmp_ptr, strchr(buf, '\"') + 1, n_char);
            tmp_ptr += n_char;
        }
        memset(buf, 0, MAXDATASIZE);
    }
    fclose(fp);
    *get_len = tmp_ptr - ptr;
    return ret;
}

