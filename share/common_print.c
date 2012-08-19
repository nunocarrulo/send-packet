#include <stdio.h>
#include <stdint.h>

void print_content(uint8_t *ptr, int length)
{
    int i;
    for (i = 0; i < length; i++) {
        if (i % 16 == 0)
            printf("\n");
        printf("%02x ", ptr[i]);
    }
    printf("\n");
}

#if 0
/* Embedway-cxxu start */
printf("for debug, @func: %s, line: %d.\n", __FUNCTION__, __LINE__); 
printf("Line: %d, Just for debug...\n", __LINE__);
/* Embedway-cxxu end */
#endif

