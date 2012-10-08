#ifndef __RAW_H__
#define __RAW_H__
int set_raw_debug(int log);
int raw_send(char *net_i, uint16_t ether_type, uint8_t *ptr, int len);
int raw_send_all(char *net_i, uint8_t *ptr, int len);
#endif
