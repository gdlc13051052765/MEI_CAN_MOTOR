#ifndef  __COMM_CAN_H
#define  __COMM_CAN_H
#include <stdint.h>
#include <stdio.h>
#include "udp_fifo.h"

#define MAX_MUTIL_LEN   256 
#define MAX_MUTIL_ITEM_NUM	10

#define RETRY_TIME				3000 //3s没收到回复重发

#define SAME_ID					1
#define SAME_PKG_ID				2


typedef struct
{
	uint8_t is_complete;	//是否接收完成 
	uint8_t in_use;			//是否正在使用  0：空闲 1：使用中
	uint8_t recv_pkg_num;		//记录接收数据帧的数量
	uint8_t r_len;
	uint8_t r_data[MAX_MUTIL_LEN];
}_Mutil_Ring,*_pMutil_Ring;
  

typedef struct
{ 
	uint8_t  byte_count;		//实际缓存中的长度
	uint8_t  data[MAX_MUTIL_LEN];
}_Ret_Msg,*_pRet_Msg;		//包含连包和单包

typedef struct
{
	uint8_t  tbale_index;	//表索引值，对应缓存
}_Mutil_Table,*_pMutil_Table;

typedef struct
{
	//发送fifo
	void*     		Sed_Fifo;		//发送fifo
	//接收fifo
	void* 				Rcv_Fifo;	//短包fifo
	
	//fifo函数接口
//  _pCan_Fifo_Func  p_ffunc;			//函数接口
	
	//多帧处理缓存
	_pMutil_Ring 	pMutil_Fifo;		//长包fifo
}_Can_Instance, *_pCan_Instance;


#endif
