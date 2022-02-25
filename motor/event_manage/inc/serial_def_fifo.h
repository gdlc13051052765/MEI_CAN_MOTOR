#ifndef   __SERIAL_DEF_FIFO_H
#define   __SERIAL_DEF_FIFO_H

#include "can_drv.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "struct_type.h"
#include "def_fifo.h"
  

typedef struct
{ 
	uint8_t (*init_s)(void*, _pSerial_Item, uint16_t);
	void* (*init_m)(_pSerial_Item, uint16_t);
	uint8_t (*is_empty)(void*);
	uint8_t (*push)(void*, _pSerial_Item);
	uint8_t (*pop)(void*, _pSerial_Item);
}_Serial_Fifo_Func,*_pSerial_Fifo_Func;


extern const _pSerial_Fifo_Func pSerial_Fifo_Func;
#endif
