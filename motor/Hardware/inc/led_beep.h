#ifndef  __LED_BEEP_H
#define  __LED_BEEP_H
#include "includes.h"

#define   BEEP_PORT		BEEP_GPIO_Port
#define 	BEEP_PIN		BEEP_Pin

#define BEEP_ON()				do{HAL_GPIO_WritePin(BEEP_PORT, BEEP_PIN, GPIO_PIN_SET);}while(0)
#define BEEP_OFF()			do{HAL_GPIO_WritePin(BEEP_PORT, BEEP_PIN, GPIO_PIN_RESET);}while(0)


uint8_t open_box_power(uint8_t index);
uint8_t close_box_power(uint8_t index);
void led_gpio_init(void);
void set_led_toggle(void);

#endif
