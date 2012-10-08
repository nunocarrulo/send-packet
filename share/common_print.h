
#ifndef __COMMON_PRINT__
#define __COMMON_PRINT__

void hex_and_ascii_print(register const char *ident, register const uint8_t * cp, register uint32_t length);
void print_content(uint8_t * ptr, int length);
#if 0
/* Embedway-cxxu start */
printf("for debug, @func: %s, line: %d.\n", __FUNCTION__, __LINE__);
printf("Line: %d, Just for debug...\n", __LINE__);
/* Embedway-cxxu end */
#endif
#endif
