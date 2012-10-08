#ifndef __PARSER_CONFIG_H__
#define __PARSER_CONFIG_H__

typedef struct _parser_config {
    int offset;
    int type;
    int vary;
    int num; /* if type is bits, num is n_bits; */
             /* if type is string, num is bytes; */
    union {
        char s[2048];
        uint64_t value;
    }u;
}parser_config_t;
int parser_config(char *filename, char *ptr, int len, int *get_len);
int set_parser_debug(int log);
#endif
