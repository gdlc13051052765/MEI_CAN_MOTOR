#ifndef __BSP_INIT_H
#define __BSP_INIT_H
#include <stdint.h>


#define FILTER_H(filter)			(((filter<<3)>>16)&0xFFFF)
#define	FILTER_L(filter)			(((filter<<3)&0xFFFF)|CAN_ID_EXT)

#define FILTER_MASK_H(filter)		(((filter<<3)>>16) & 0xFFFF)//取高16位
#define FILTER_MASK_L(filter)		((filter<<3)&0xFFFF)//取低16位

#define GET_FILTER_BY_S1(filter, s1_addr) (( filter & 0xFFE1FFFF ) | ((s1_addr & 0xF) << 17))

void bsp_init(void);
void MX_CAN_Init(uint8_t s1_addr, uint8_t s2_addr);
uint32_t get_mcu_id(void);

#endif
