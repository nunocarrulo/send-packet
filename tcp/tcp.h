#ifndef __TCP_H__
#define __TCP_H__
int tcp_connect(uint16_t dst_port, uint32_t dst_ip, uint32_t m_s);
int tcp_connect_send(uint16_t dst_port, uint32_t dst_ip, char *msg);
int set_tcp_debug(int log);
#endif
