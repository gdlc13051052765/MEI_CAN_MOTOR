#ifndef  __LOAD_GPIO_H
#define  __LOAD_GPIO_H
#include "main.h"

#define ALL_BOX_POWER_ON			0x0FFF		//打开所有的BOX电源


#define BOX_POWER_PIN_NUM			12				//

extern uint16_t g_box_power_pin[];

extern void *g_box_power_port[];

typedef enum
{
	POWER_OFF = 0,		//设置关闭
	POWER_ON = 1, 		//设置打开
	CLOSE_OTHERS,			//关闭除此之外的节点,此节点打开
	OPEN_OTHERS,			//打开除此之外的节点，此节点关闭
}_Operate_Type;

void set_box_power(uint8_t index, _Operate_Type opt_type);

#define SET_BOX_POWER(index, type)    set_box_power(index, type)

void box_power_all_open(void);	//开启所有电源

void box_power_test_func(void); 
#endif
