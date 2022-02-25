#ifndef  __UDP_SOCKET_H
#define  __UDP_SOCKET_H

#include <stdint.h>

#include "Types.h"

extern uint16 udp_port;/*定义UDP的一个端口并初始化*/
void do_udp(void);

/*==================================================================================
* 函 数 名： udp_socket_send_data
* 参    数： None
* 功能描述:  udp 发送数据
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-09-29 170658
==================================================================================*/
void udp_socket_send_data(uint8_t *data,uint16_t len);

#endif
