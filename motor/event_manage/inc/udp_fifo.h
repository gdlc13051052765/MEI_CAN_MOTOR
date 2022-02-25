#ifndef __CAN_FIFO_H
#define __CAN_FIFO_H

#include <stdint.h>
#include <string.h>
#include "can_drv.h"

#define UDP_MAX_CACHE_LEN			200	//UDP发送缓存
#define MAX_CACHE_NUM						32	//最大缓存池

typedef struct
{ 
	uint8_t  byte_count;
	uint8_t  data[UDP_MAX_CACHE_LEN];
}_Udp_Msg,*_pUdp_Msg;

typedef struct send_queue {
	_Udp_Msg queue[UDP_MAX_CACHE_LEN];
	char wp;
	char rd;
	char count;
}send_queue_t, * p_send_queue_t;

void msg_queue_init(p_send_queue_t p_queue_buff);
unsigned int msg_queue_num(p_send_queue_t p_queue_buff);
void msg_queue_push(p_send_queue_t p_queue_buff, _Udp_Msg *data);
void msg_queue_pop(p_send_queue_t p_queue_buff, uint8_t msg_id);


#endif
