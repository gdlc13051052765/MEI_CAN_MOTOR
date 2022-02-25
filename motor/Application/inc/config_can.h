#ifndef  __CONFIG_CAN_H
#define __CONFIG_CAN_H

#include <stdint.h>
#include <stdio.h>
#include "load_gpio.h"

//初始化
//安卓to CAN地址

typedef struct
{ 
	uint8_t box_addr;				//通信地址
	GPIO_TypeDef * box_pw_prot;
	uint16_t box_pw_pin;		//box对应的电源引脚索引
	uint8_t power_addr;			//电源地址 
}_IO_Node_Tab,*_pIO_Node_Tab;


typedef enum
{
	cfg_power = 0,	//关闭其他电源
	is_exist,				//判读当前地址是否存在
	sed_addr,				//发送分配地址
	wait_ack,				//等待从机响应
	cfg_complete,		//分配成功
	
}_Cfg_State;		//配置CAN总线地址


typedef uint8_t (*can_parse)(void*);			//CAN总线数据解析函数解析

extern _IO_Node_Tab mANDROID_CAN_MAP_Tab[];


void can_power_and_addr_init(void);
//配置从机CAN地址 
void config_can_addr_task(void *argv);

#endif
