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
#include <inttypes.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <endian.h>
#include "log.h"
#include "common.h"
#include "parser_cfg.h"

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

parser_config_t cfg_rslt[PARSER_FILE_DEPTH];

void init_cfg_rslt(void)
{
    memset(cfg_rslt, 0, sizeof(parser_config_t) * PARSER_FILE_DEPTH);
}

void show_cfg_rslt(void)
{
    int line = 0;
    while (line < PARSER_FILE_DEPTH) {
        if (cfg_rslt[line].type == 0) {
            break;
        }
        printf("offset: %04d, type: %d, vary, %d, num: %02d, ",\
               cfg_rslt[line].offset,\
               cfg_rslt[line].type,\
               cfg_rslt[line].vary,\
               cfg_rslt[line].num);
        if (cfg_rslt[line].type == SP_CNFG_TYPE_BITS) {
            printf("value: 0x%" PRIxFAST64, cfg_rslt[line].u.value);
        } else if (cfg_rslt[line].type == SP_CNFG_TYPE_STRING) {
            printf("string: %s", cfg_rslt[line].u.str);
        }
        printf("\n");
        line++;
    }
    printf("\n");
}

static int get_bitsoffset_value(void *ptr, int bits_offset)
{
    int a;
    int b;
    uint8_t  tmp = 0;
    a = bits_offset / 8;
    b = bits_offset % 8;
    tmp = *((uint8_t *)ptr + a);
    if (tmp >> (7 - b)) {
        return 1;
    } else {
        return 0;
    }
}

static void set_bitsoffset_value(void *ptr, int bits_offset, int v)
{
    int a;
    int b;
    uint8_t  tmp = 0;
    a = bits_offset / 8;
    b = bits_offset % 8;
    tmp = *((uint8_t *)ptr + a);
    if (v) {
        tmp |= (1 << (7 - b));
    } else {
        tmp &= (~(1 << (7 - b)));
    }
    *((uint8_t *)ptr + a) = tmp;
}

static void inc_value_by_offset(void *ptr, int bits_offset, int bits_num)
{
    uint64_t value = 0ull;
    int bits_v;
    int i = 0;

    for (i = 0; i < bits_num; i++) {
        bits_v = get_bitsoffset_value(ptr, bits_offset + i);
        if (bits_v) {
            value |= (1ull << (bits_num - i));
        }
    }
    parser_print("value: %" PRIdLEAST64, value);
    value++;
    for (i = 0; i < bits_num; i++) {
        set_bitsoffset_value(ptr, bits_offset + i, value >> (bits_num - i));
    }
}

void reconfig_cfg_rslt(void *ptr)
{
    int line = 0;
    while (line < PARSER_FILE_DEPTH) {
        if (cfg_rslt[line].type == 0) {
            break;
        }
        printf("offset: %04d, type: %d, vary, %d, num: %02d, ",\
               cfg_rslt[line].offset,\
               cfg_rslt[line].type,\
               cfg_rslt[line].vary,\
               cfg_rslt[line].num);
        if (cfg_rslt[line].vary == SP_CNFG_VARY_INC) {
            if (cfg_rslt[line].type == SP_CNFG_TYPE_BITS) {
                printf("value: 0x%" PRIxFAST64, cfg_rslt[line].u.value);
            }
            inc_value_by_offset(ptr, cfg_rslt[line].offset, cfg_rslt[line].num);
        } else if (cfg_rslt[line].vary == SP_CNFG_VARY_DEC) {
        }
        line++;
    }
    printf("\n");
}

void get_newbuf_by_cfg_rslt(char *buf, int counter)
{
    /* */
    int line = 0;
    buf = buf;
    if (counter == 0) {
        return;
    }
    while (line < PARSER_FILE_DEPTH) {
        if (cfg_rslt[line].type == 0) {
            break;
        }
        printf("offset: %04d, type: %d, vary, %d, num: %02d, ",\
               cfg_rslt[line].offset,\
               cfg_rslt[line].type,\
               cfg_rslt[line].vary,\
               cfg_rslt[line].num);
        if (cfg_rslt[line].vary != 0) {
            if (cfg_rslt[line].type == SP_CNFG_TYPE_BITS) {
                printf("value: 0x%" PRIxFAST64, cfg_rslt[line].u.value);
            } else if (cfg_rslt[line].type == SP_CNFG_TYPE_STRING) {
                printf("string: %s", cfg_rslt[line].u.str);
            }
        }
        line++;
    }
}

void clean_cfg_rslt(void)
{
    int line = 0;
    while (line < PARSER_FILE_DEPTH) {
        if (cfg_rslt[line].type == 0) {
            break;
        }
        if (cfg_rslt[line].type == SP_CNFG_TYPE_STRING) {
            if (cfg_rslt[line].u.str != NULL) {
                free(cfg_rslt[line].u.str);
            }
        }
        line++;
    }
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
            *value = (uint64_t)strtoull(p, NULL, 0);
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
    int valid_line = 0;
    int bits_count = 0;

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

                cfg_rslt[valid_line].offset = bits_count;
                cfg_rslt[valid_line].type = SP_CNFG_TYPE_BITS;
                cfg_rslt[valid_line].vary = 0;
                cfg_rslt[valid_line].num = n_bit;
                cfg_rslt[valid_line].u.value = tmp_data;
                bits_count += n_bit;
                valid_line++;

                tmp_data = (tmp_data << (MAX_CALC_BITS - n_bit));
                parser_print("hawhaw, tmp_data: 0x%" PRIxFAST64 ".\n", tmp_data);
                tmp_data = htonll(tmp_data);
                parser_print("hawhaw, tmp_data: 0x%" PRIxFAST64 ".\n", tmp_data);
                memcpy(tmp_ptr, (char *)&tmp_data, n_bit >> 3);
                tmp_ptr += (n_bit >> 3);
            } else if (strncmp(buf, "string", 6) == 0) {
                n_char = atoi(buf + 6);
                strncpy(tmp_ptr, strchr(buf, '\"') + 1, n_char);
                tmp_ptr += n_char;

                cfg_rslt[valid_line].offset = bits_count;
                cfg_rslt[valid_line].type = SP_CNFG_TYPE_STRING;
                cfg_rslt[valid_line].vary = 0;
                cfg_rslt[valid_line].num = n_bit;
                cfg_rslt[valid_line].u.str = sp_zmalloc(n_char + 1);
                strncpy(cfg_rslt[valid_line].u.str, strchr(buf, '\"') + 1, n_char);
                bits_count += (n_char * 8);
                valid_line++;
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

            cfg_rslt[valid_line].offset = bits_count;
            cfg_rslt[valid_line].type = SP_CNFG_TYPE_BITS;
            cfg_rslt[valid_line].vary = 0;
            cfg_rslt[valid_line].num = n_bit;
            cfg_rslt[valid_line].u.value = tmp_data;
            bits_count += n_bit;
            valid_line++;

            data_64 |= (tmp_data << (MAX_CALC_BITS - size));
            if (size == MAX_CALC_BITS) {
                //data = ;
                *(uint64_t *)tmp_ptr = htonll(data_64);
                tmp_ptr += 8;
                parser_print("that is a word, oh hawhaw, data64: 0x%" PRIxFAST64 ".\n", data_64);
                size = 0;
                data_32 = 0;
                data_64 = 0;
            }
        } else if (strncmp(buf, "string", 6) == 0) {
            parser_print("that is a data64 or data 32: 0x%" PRIxFAST64 ".\n", data_64);
            if (size != 0) {
                /* size must be 32 */
                *(uint32_t *)tmp_ptr = htonl((uint32_t)(data_64 >> 32));
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

            cfg_rslt[valid_line].offset = bits_count;
            cfg_rslt[valid_line].type = SP_CNFG_TYPE_STRING;
            cfg_rslt[valid_line].vary = 0;
            cfg_rslt[valid_line].num = n_char;
            cfg_rslt[valid_line].u.str = sp_zmalloc(n_char + 1);
            strncpy(cfg_rslt[valid_line].u.str, strchr(buf, '\"') + 1, n_char);
            bits_count += (n_char * 8);
            valid_line++;
        }
        memset(buf, 0, MAXDATASIZE);
    }
    fclose(fp);
    *get_len = tmp_ptr - ptr;
    return ret;
}


