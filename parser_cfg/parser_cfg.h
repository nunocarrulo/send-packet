#ifndef __PARSER_CONFIG_H__
#define __PARSER_CONFIG_H__

typedef struct _parser_config {
    int offset; /* offset bits from config file head */
    int type;   /* bits or string */
    int vary;   /* vary method, the following is example.
                 *
                 * if type is bits,
                 * if vary ==  1, next packet value is value + 1;
                 * if vary ==  2, next packet value is value + 2;
                 * if vary == -2, next packet value is value - 2;
                 *
                 * if type is string,
                 * Just now I don't know how to vary.
                 */
    int num;    /* if type is bits, num is n_bits; */
                /* if type is string, num is bytes; */
    union {
        uint64_t value;
        char *str;
    } u;
} parser_config_t;
int parser_config(char *filename, char *ptr, int len, int *get_len);
int set_parser_debug(int log);
#endif
