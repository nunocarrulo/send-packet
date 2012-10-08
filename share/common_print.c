/**
 * @file common_print.c
 * @Synopsis  print content of the packet
 * @author xuchunxiao369@gmail.com
 * @date 2012-10-08
 */
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#define ASCII_LINELENGTH 300
#define HEXDUMP_BYTES_PER_LINE 16
#define HEXDUMP_SHORTS_PER_LINE (HEXDUMP_BYTES_PER_LINE / 2)
#define HEXDUMP_HEXSTUFF_PER_SHORT 5    /* 4 hex digits and a space */
#define HEXDUMP_HEXSTUFF_PER_LINE \
        (HEXDUMP_HEXSTUFF_PER_SHORT * HEXDUMP_SHORTS_PER_LINE)

void ascii_print(register const uint8_t * cp, register uint32_t length)
{
    register int s;

    putchar('\n');
    while (length > 0) {
        s = *cp++;
        length--;
        if (!isgraph(s) && (s != '\t' && s != ' ' && s != '\n' && s != '\r'))
            putchar('.');
        else
            putchar(s);
    }
}

void
hex_and_ascii_print_with_offset(register const char *ident,
                                register const uint8_t * cp, register uint32_t length, register uint32_t oset)
{
    register uint32_t i;
    register int s1, s2;
    register int nshorts;
    char hexstuff[HEXDUMP_SHORTS_PER_LINE * HEXDUMP_HEXSTUFF_PER_SHORT + 1], *hsp;
    char asciistuff[ASCII_LINELENGTH + 1], *asp;

    nshorts = length / sizeof(uint16_t);
    i = 0;
    hsp = hexstuff;
    asp = asciistuff;
    while (--nshorts >= 0) {
        s1 = *cp++;
        s2 = *cp++;
        (void)snprintf(hsp, sizeof(hexstuff) - (hsp - hexstuff), " %02x%02x", s1, s2);
        hsp += HEXDUMP_HEXSTUFF_PER_SHORT;
        *(asp++) = (isgraph(s1) ? s1 : '.');
        *(asp++) = (isgraph(s2) ? s2 : '.');
        i++;
        if (i >= HEXDUMP_SHORTS_PER_LINE) {
            *hsp = *asp = '\0';
            (void)printf("%s0x%04x: %-*s  %s", ident, oset, HEXDUMP_HEXSTUFF_PER_LINE, hexstuff, asciistuff);
            i = 0;
            hsp = hexstuff;
            asp = asciistuff;
            oset += HEXDUMP_BYTES_PER_LINE;
        }
    }
    if (length & 1) {
        s1 = *cp++;
        s1 = *cp++;
        (void)snprintf(hsp, sizeof(hexstuff) - (hsp - hexstuff), " %02x", s1);
        hsp += 3;
        *(asp++) = (isgraph(s1) ? s1 : '.');
        ++i;
    }
    if (i > 0) {
        *hsp = *asp = '\0';
        (void)printf("%s0x%04x: %-*s  %s", ident, oset, HEXDUMP_HEXSTUFF_PER_LINE, hexstuff, asciistuff);
    }
}

void hex_and_ascii_print(register const char *ident, register const uint8_t * cp, register uint32_t length)
{
    hex_and_ascii_print_with_offset(ident, cp, length, 0);
}

void print_content(uint8_t * ptr, int length)
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
