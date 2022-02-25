/* Copyright (c)  .
 *
 * The information contained herein is property of Giesecke & Devrient Group ASA.
 * Terms and conditions of usage are described in detail in Giesecke & Devrient Group
 * STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
 
/** @file
 *
 * @defgroup IOPINConfig.h
 * @{
 * @ingroup  global PIN defines
 * @brief    PIN will be defined in this file for each module.
 *
 * The different applicaiton will use different PIN config, developer can 
 * update this file.
 * Each PIN have group number and pin number, 
 * for example #HW_IO_PIN_GROUP_A is group number, #HW_IO_PIN_SPI1_M_SCK_PIN is pin number.
 */
 
 
#ifndef _HW_IO_CONFIG_H__
#define _HW_IO_CONFIG_H__

#include <stdint.h>
#include "SwitchConfig.h"

/**
 * @brief This value can be provided instead of a pin number for signals, 
 * 				specify that the given signal is not used and
 *        therefore does not need to be connected to a pin.
 *				Please make group and number are 0xFF at the same time.
 */
#define HW_IO_PIN_GROUP_NOT_USED  									0xFF
#define HW_IO_PIN_NUMBER_NOT_USED  									0xFF


//check NOT use PIN
#define HW_IO_PIN_IS_NOT_USED(group, number)   			(((group == HW_IO_PIN_GROUP_NOT_USED)&&(number == HW_IO_PIN_NUMBER_NOT_USED))?true:false)

//IO group number
typedef enum {
				HW_IO_PIN_GROUP_A		=		1,
				HW_IO_PIN_GROUP_B,
				HW_IO_PIN_GROUP_C,
				HW_IO_PIN_GROUP_D,
				HW_IO_PIN_GROUP_E,
				HW_IO_PIN_GROUP_F,
				HW_IO_PIN_GROUP_G,
				HW_IO_PIN_GROUP_H,
}hw_io_pin_group;

/************************For APP_HK_MOTOR_EP START*********************/

#ifdef APP_HK_MOTOR_EP
	/*********************w5500 gpio********************************/
	#define W5500_SCS_PORT							GPIOA		
	#define W5500_SCS_PIN								GPIO_PIN_4
	#define W5500_RST_GROUP							GPIOA
	#define W5500_RST_PIN								GPIO_PIN_2
	#define W5500_INT_GROUP							GPIOA
	#define W5500_INT_PIN								GPIO_PIN_1
	#define W5500_MISO_PORT							GPIOA		
	#define W5500_MISO_PIN							GPIO_PIN_6
	#define W5500_MOSI_PORT							GPIOA
	#define W5500_MOSI_PIN							GPIO_PIN_7
	#define W5500_SCK_PORT							GPIOA
	#define W5500_SCK_PIN								GPIO_PIN_5


	#define LED_Pin 										GPIO_PIN_12
	#define LED_GPIO_Port 							GPIOB


#endif

#endif		//_HW_IO_CONFIG_H__
