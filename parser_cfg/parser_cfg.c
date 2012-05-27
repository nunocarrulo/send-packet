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

#define MAXDATASIZE  256
int parser_config(char *filename, uint8_t *ptr)
{
    FILE *fp;
    char buf[MAXDATASIZE] = {0};

    ptr = ptr;
    if (ptr == NULL) {
        return 0;
    }
    if ((fp = fopen(filename, "r")) == NULL)
        return 2;
    while (fgets(buf, MAXDATASIZE, fp)) {
        if (buf[0] == '\n') {
            continue;
        }
        if (buf[0] == '#') {
            continue;
        }
        printf("%s", buf);
        memset(buf, 0, MAXDATASIZE);
    }
    fclose(fp);
    return 0;
}

