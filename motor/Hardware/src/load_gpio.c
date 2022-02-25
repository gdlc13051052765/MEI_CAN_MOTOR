#include "load_gpio.h"
/*==================================================================================
* Copyright(C), 2019-2020, Red Hat Inc. 
* File name: load_gpio.c
* Author:      xiaozh
* Version:    V0.01
* Date: 	2019-10-11
* Description: 外设GPIO控制
* Function List: // 主要函数的列表，每条记录应包括函数名及功能简要说明
* History:    // 修改历史，包括每次修改的日期、修改者和修改内容简述
==================================================================================*/
uint16_t g_box_power_pin[BOX_POWER_PIN_NUM] = {
	LOAD_ACT0_Pin,
	LOAD_ACT1_Pin,
	LOAD_ACT2_Pin,
	LOAD_ACT3_Pin,
	LOAD_ACT4_Pin,
	LOAD_ACT5_Pin,
	LOAD_ACT6_Pin,
	LOAD_ACT7_Pin,
	LOAD_ACT8_Pin,
	LOAD_ACT9_Pin,
	LOAD_ACT10_Pin,
	LOAD_ACT11_Pin,  
};

void * g_box_power_port[BOX_POWER_PIN_NUM] = {
	LOAD_ACT0_GPIO_Port,
	LOAD_ACT1_GPIO_Port,
	LOAD_ACT2_GPIO_Port,
	LOAD_ACT3_GPIO_Port,
	LOAD_ACT4_GPIO_Port,
	LOAD_ACT5_GPIO_Port,
	LOAD_ACT6_GPIO_Port,
	LOAD_ACT7_GPIO_Port,
	LOAD_ACT8_GPIO_Port,
	LOAD_ACT9_GPIO_Port,
	LOAD_ACT10_GPIO_Port,
	LOAD_ACT11_GPIO_Port,  
};

/*==================================================================================
* 函 数 名： box_power_open
* 参    数： None
* 功能描述:  打开box电源
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-11-07 174224
==================================================================================*/
void box_power_open(uint8_t index)
{
	//HAL_GPIO_WritePin(LOAD_ACT0_GPIO_Port, g_box_power_pin[index], GPIO_PIN_SET);
	if (index >= 0 && index < BOX_POWER_PIN_NUM)
	{
		HAL_GPIO_WritePin((GPIO_TypeDef *)g_box_power_port[index], g_box_power_pin[index], GPIO_PIN_SET);

	}
}

/*==================================================================================
* 函 数 名： box_power_all_open
* 参    数： None
* 功能描述:  打开所有box电源
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-11-08 141724
==================================================================================*/
void box_power_all_open(void)
{ 
	 int i = 0; 
	
	for(i=0; i<BOX_POWER_PIN_NUM; i++)
	{
		box_power_open(i); 
		HAL_Delay(50);	//加一定延时防止电源出问题
	} 
}

/*==================================================================================
* 函 数 名： box_power_close
* 参    数： None
* 功能描述:  关闭box电源
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-11-07 173607
==================================================================================*/
void box_power_close(uint8_t index)
{ 
	if (index >= 0 && index < BOX_POWER_PIN_NUM)
	{
		HAL_GPIO_WritePin((GPIO_TypeDef *)g_box_power_port[index], g_box_power_pin[index], GPIO_PIN_RESET);
	}
}
									
/*==================================================================================
* 函 数 名： set_box_state
* 参    数： index : 0~13:设置单个box状态， 0xFF：设置所有
* 功能描述:  设置box电源状态
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-11-07 174937
==================================================================================*/
void set_box_power(uint8_t index, _Operate_Type opt_type)
{  
	uint32_t gpio_temp = 0;
	 if(index == 0xFF)		//所有
	 {
		 switch(opt_type)
		 {
			 case POWER_OFF:		//关闭所有
			 case CLOSE_OTHERS:	//关闭
			 {
				for (int i = 0; i < BOX_POWER_PIN_NUM; ++i)
				{
					box_power_close(i);
				}				
				break;
			 }
			 
			 case POWER_ON:
			 case OPEN_OTHERS:
			 {
				for (int i = 0; i < BOX_POWER_PIN_NUM; ++i)
				{
					box_power_open(i);
				}				
				break;
			 }
			 
			 default:
			 {
				 break;
			 }
		 }
	 }
	 else if(index <= 0xFF)
	 { 
		 switch(opt_type)
		 {
			 case POWER_OFF:		//关闭所有
			 {
				 box_power_close(index);
				 break;
			 }
			 
			 case CLOSE_OTHERS:	//关闭
			 {
				 
				for (int i = 0; i < BOX_POWER_PIN_NUM; ++i)
				{
					if (i == index)
					{
					  	box_power_open(i);
					} else {
						box_power_close(i);
					}
				}
				break;
			 }
			 
			 case POWER_ON:
			 {
				 box_power_open( index);
				 break;
			 }
			 
			 case OPEN_OTHERS:
			 { 
			 	for (int i = 0; i < BOX_POWER_PIN_NUM; ++i)
				{
					if (i == index)
					{
					  	box_power_close(i);
					} else {
						box_power_open(i);
					}
				}
				break;
			 }
			 
			 default:
			 {
				 break;
			 }
		 }
	 }
}

/*==================================================================================
* 函 数 名： box_power_test
* 参    数： None
* 功能描述:  测试函数
* 返 回 值： None
* 备    注： 如果超过最大值，则自动进行分包发送
* 作    者： xiaozh
* 创建时间： 2019-09-17 122635
==================================================================================*/
void box_power_test_func(void)
{
	int i = 0; 
	
	for(i=0; i<12; i++)
	{
		box_power_open(i); 
		HAL_Delay(50);
	} 
}
 
