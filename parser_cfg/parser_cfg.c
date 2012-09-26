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
#include "log.h"

#define MAXDATASIZE  256
#define CONFIG_UNVALID 1
#define MAX_PARSER_BITS 32

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
static int get_value(char *buf, unsigned long *value)
{
    char *p = NULL;
    int count = 0;
    p = strtok(buf, " ");
    while (p) {
        count++;
        if (count == 2) {
            //parser_print("p: %s.\n", p);
            *value = strtoul(p, NULL, 0);
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
        //parser_print("%s", buf);
        if (strncmp(buf, "bits", 4) == 0) {
            n_bit = atoi(buf + 4);
            parser_print("bits: %d.\n", n_bit);
            size += n_bit;
            if (size == MAX_PARSER_BITS) {
                //parser_print("that is a word, oh hawhaw.\n");
                size = 0;
            } else if (size > MAX_PARSER_BITS && (size % MAX_PARSER_BITS == 0)) {
                size = 0;
            } else if (size > MAX_PARSER_BITS) {
                parser_print("content not %d bits aligned, exit.\n", MAX_PARSER_BITS);
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

int parser_config(char *filename, char *ptr, int len)
{
    FILE *fp;
    int n_bit = 0;
    int n_char = 0;
    int size = 0;
    int ret = 0;
    char *tmp_ptr = ptr;
    unsigned long data = 0;
    unsigned long tmp_data = 0;
    char buf[MAXDATASIZE] = {0};

    data = data;
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
        //parser_print("%s", buf);
        if (strncmp(buf, "bits", 4) == 0) {
            n_bit = atoi(buf + 4);
            //parser_print("bits: %d.\n", n_bit);
            size += n_bit;
            ret = get_value(buf, &tmp_data);
            //parser_print("that is a word, oh hawhaw, tmp data: 0x%lx.\n", tmp_data);
            data |= (tmp_data << (MAX_PARSER_BITS - size));
            if (size == MAX_PARSER_BITS) {
                //data = ;
                *(unsigned long *)tmp_ptr = data;
                tmp_ptr += MAX_PARSER_BITS;
                parser_print("that is a word, oh hawhaw, data: 0x%lx.\n", data);
                size = 0;
                data = 0;
            }
        } else if (strncmp(buf, "string", 6) == 0) {
            n_char = atoi(buf + 6);
            parser_print("char: %d.\n", n_char);
        }
        memset(buf, 0, MAXDATASIZE);
    }
    fclose(fp);
    return ret;
}

