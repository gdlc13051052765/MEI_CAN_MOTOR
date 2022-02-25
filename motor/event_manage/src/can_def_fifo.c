#include "can_def_fifo.h"

//申请结构 
DEF_SAFE_FIFO(can_fifo_1, _Can_Item, uint16_t, uint8_t)     
DEF_SAFE_FIFO_END

static _Can_Fifo_Func mCan1_Fifo_Func = 
{
	.init_s						=  	FIFO_INIT_S_X(can_fifo_1),
	.init_m						=  	FIFO_INIT_M_X(can_fifo_1),
	.is_empty 				= 	FIFO_IS_EMPTY_X(can_fifo_1),
	.push	 						= 	FIFO_PUSH_X(can_fifo_1),
	.pop 							= 	FIFO_POP_X(can_fifo_1),
};

const _pCan_Fifo_Func pCan1_Fifo_Func = &mCan1_Fifo_Func;

//申请结构 
DEF_SAFE_FIFO(can_fifo_2, _Can_Item, uint16_t, uint8_t)     
DEF_SAFE_FIFO_END

static _Can_Fifo_Func mCan2_Fifo_Func = 
{
	.init_s						=  	FIFO_INIT_S_X(can_fifo_2),
	.init_m						=  	FIFO_INIT_M_X(can_fifo_2),
	.is_empty 				= 	FIFO_IS_EMPTY_X(can_fifo_2),
	.push	 						= 	FIFO_PUSH_X(can_fifo_2),
	.pop 							= 	FIFO_POP_X(can_fifo_2),
};

const _pCan_Fifo_Func pCan2_Fifo_Func = &mCan2_Fifo_Func;

