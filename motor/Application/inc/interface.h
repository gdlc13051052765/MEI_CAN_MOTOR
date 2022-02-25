#ifndef  __INTERFACE_H
#define  __INTERFACE_H

#include "includes.h"

#define   APP_CONFIG_ADDR			(0x0801F800)					//用户配置参数地址
//修改配置参数
typedef enum
{
	Cc_Set_Addr =0,
	Cc_Set_Soft_Ver,
	Cc_Set_Hard_Ver,
	
}_Config_cmd;

void inter_init(void); 
void can1_rev_decode(void);
void can2_rev_decode(void);
uint8_t read_config_param(void* c_param);
void si_sed_loop_check(void* const instance);  
uint8_t si_sed_buff(void *instance, uint8_t slave_addr, uint8_t *buff, uint16_t len, uint8_t try_time);
void write_config_param(_Config_cmd cmd,uint8_t data);

#endif
