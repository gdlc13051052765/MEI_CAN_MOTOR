#ifndef  __COMM_CAN_H
#define  __COMM_CAN_H
#include <stdint.h>
#include <stdio.h>
#include "udp_fifo.h"

#define MAX_MUTIL_LEN   256 
#define MAX_MUTIL_ITEM_NUM	10

#define RETRY_TIME				3000 //3sû�յ��ظ��ط�

#define SAME_ID					1
#define SAME_PKG_ID				2


typedef struct
{
	uint8_t is_complete;	//�Ƿ������� 
	uint8_t in_use;			//�Ƿ�����ʹ��  0������ 1��ʹ����
	uint8_t recv_pkg_num;		//��¼��������֡������
	uint8_t r_len;
	uint8_t r_data[MAX_MUTIL_LEN];
}_Mutil_Ring,*_pMutil_Ring;
  

typedef struct
{ 
	uint8_t  byte_count;		//ʵ�ʻ����еĳ���
	uint8_t  data[MAX_MUTIL_LEN];
}_Ret_Msg,*_pRet_Msg;		//���������͵���

typedef struct
{
	uint8_t  tbale_index;	//������ֵ����Ӧ����
}_Mutil_Table,*_pMutil_Table;

typedef struct
{
	//����fifo
	void*     		Sed_Fifo;		//����fifo
	//����fifo
	void* 				Rcv_Fifo;	//�̰�fifo
	
	//fifo�����ӿ�
//  _pCan_Fifo_Func  p_ffunc;			//�����ӿ�
	
	//��֡������
	_pMutil_Ring 	pMutil_Fifo;		//����fifo
}_Can_Instance, *_pCan_Instance;


#endif
