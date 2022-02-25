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
 
 
#ifndef _SWITCH_CONFIG_H__
#define _SWITCH_CONFIG_H__

/********************************************************************************************************************************************
*
*																									Applicaiton Macro defines
*
*********************************************************************************************************************************************/
#define APP_HK_MOTOR_EP														//CCB DC-EP


/********************************************************************************************************************************************
*
*																									system version
*
*********************************************************************************************************************************************/
#ifdef APP_HK_MOTOR_EP
#define APP_SYSTEM_FW_VER												"1.0.0.28"
#endif

/********************************************************************************************************************************************
*
*																									peripheral Module Selections
*
*********************************************************************************************************************************************/


#ifdef APP_XY_TD_EP


#endif

#endif 		//_SWITCH_CONFIG_H__
