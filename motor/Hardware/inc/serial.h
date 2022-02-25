#ifndef  __SERIAL_H
#define  __SERIAL_H
 
#include <stdint.h>

#include "serial_def_fifo.h"

typedef struct
{
	uint8_t rev_complete;		//接收完成
	void* pDev;
	
	void* p_sed_fifo;
	void* p_rev_fifo;
	_pSerial_Fifo_Func p_ffunc;	//fifo函数接口
}_Serial_Info,*_pSerial_Info;


void serial_register_init(void *instance, void* pdev);
void serial_sed_buff(void *instance, uint8_t *buff, uint16_t len);
uint8_t serial_push_byte(void *instance, uint8_t value);
#endif
