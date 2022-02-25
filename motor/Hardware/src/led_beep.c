#include "led_beep.h"
#include "hwioconfig.h"

/*==================================================================================
* 函 数 名： open_box_power
* 参    数： None
* 功能描述:  打开box电源
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-10-18 160210
==================================================================================*/
uint8_t open_box_power(uint8_t index)
{
	GPIOA->BSRR = index;
	return 0x01;
}

/*==================================================================================
* 函 数 名： close_box_power
* 参    数： None
* 功能描述:  关闭box电源
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-10-18 160222
==================================================================================*/
uint8_t close_box_power(uint8_t index)
{
	GPIOA->BRR = index;
	return 0x01;
}

/*==================================================================================
* 函 数 名： set_beep_state
* 参    数： None
* 功能描述:  设置蜂鸣器状态
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-09-18 120139
==================================================================================*/
void set_beep_state( uint8_t type)
{
 
}
/*==================================================================================
* 函 数 名： led_gpio_init
* 参    数： None
* 功能描述:  led gpio ???
* 返 回 值： None
* 备    注： None
* 作    者： lc
* 创建时间： 2019-09-18 120135
==================================================================================*/
void led_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
/*==================================================================================
* 函 数 名： set_led_state
* 参    数： None
* 功能描述:  设置led灯状态
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-09-18 120135
==================================================================================*/
void set_led_state(uint8_t type)
{
	if(type)
	 HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET);  
	else	
	HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);  

}
/*==================================================================================
* 函 数 名： set_led_state
* 参    数： None
* 功能描述:  设置led灯交替闪烁
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-09-18 120135
==================================================================================*/
void set_led_toggle(void)
{
	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
}
