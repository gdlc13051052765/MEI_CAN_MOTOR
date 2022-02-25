#include "led_beep.h"
#include "hwioconfig.h"

/*==================================================================================
* �� �� ���� open_box_power
* ��    ���� None
* ��������:  ��box��Դ
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-10-18 160210
==================================================================================*/
uint8_t open_box_power(uint8_t index)
{
	GPIOA->BSRR = index;
	return 0x01;
}

/*==================================================================================
* �� �� ���� close_box_power
* ��    ���� None
* ��������:  �ر�box��Դ
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-10-18 160222
==================================================================================*/
uint8_t close_box_power(uint8_t index)
{
	GPIOA->BRR = index;
	return 0x01;
}

/*==================================================================================
* �� �� ���� set_beep_state
* ��    ���� None
* ��������:  ���÷�����״̬
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-09-18 120139
==================================================================================*/
void set_beep_state( uint8_t type)
{
 
}
/*==================================================================================
* �� �� ���� led_gpio_init
* ��    ���� None
* ��������:  led gpio ???
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� lc
* ����ʱ�䣺 2019-09-18 120135
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
* �� �� ���� set_led_state
* ��    ���� None
* ��������:  ����led��״̬
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-09-18 120135
==================================================================================*/
void set_led_state(uint8_t type)
{
	if(type)
	 HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET);  
	else	
	HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);  

}
/*==================================================================================
* �� �� ���� set_led_state
* ��    ���� None
* ��������:  ����led�ƽ�����˸
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-09-18 120135
==================================================================================*/
void set_led_toggle(void)
{
	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
}
