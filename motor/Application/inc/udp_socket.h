#ifndef  __UDP_SOCKET_H
#define  __UDP_SOCKET_H

#include <stdint.h>

#include "Types.h"

extern uint16 udp_port;/*����UDP��һ���˿ڲ���ʼ��*/
void do_udp(void);

/*==================================================================================
* �� �� ���� udp_socket_send_data
* ��    ���� None
* ��������:  udp ��������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-09-29 170658
==================================================================================*/
void udp_socket_send_data(uint8_t *data,uint16_t len);

#endif
