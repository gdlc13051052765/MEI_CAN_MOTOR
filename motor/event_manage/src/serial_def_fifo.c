#include "serial_def_fifo.h"


#define func_name   serial_xjy

//…Í«ÎΩ·ππ 
DEF_SAFE_FIFO(serial_xjy, _Serial_Item, uint16_t, uint8_t)     
DEF_SAFE_FIFO_END

static _Serial_Fifo_Func mSerial_Fifo_Func = 
{ 
	.init_s						=  	FIFO_INIT_S_X(serial_xjy),
	.init_m						=  	FIFO_INIT_M_X(serial_xjy),
	.is_empty 				= 	FIFO_IS_EMPTY_X(serial_xjy),
	.push	 						= 	FIFO_PUSH_X(serial_xjy),
	.pop 							= 	FIFO_POP_X(serial_xjy),
};

const _pSerial_Fifo_Func pSerial_Fifo_Func = &mSerial_Fifo_Func;
