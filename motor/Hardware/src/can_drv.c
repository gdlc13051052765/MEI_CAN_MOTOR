#include "can_drv.h" 

//extern CAN_HandleTypeDef hcan; 

 
/*==================================================================================
* 函 数 名： can_sed_pkg_without_cache
* 参    数： None
* 功能描述:  直接发送
* 返 回 值： 0:操作成功  0x81：失败
* 备    注： 
* 作    者：  xiaozh
* 创建时间： 2019-09-23 180101
==================================================================================*/
uint8_t  can_sed_pkg_without_cache(CAN_HandleTypeDef hcan, void* can_msg)
{
	uint8_t res = 0;
	uint32_t mTxMailbox = 0; 
	_pCan_Msg pmsg = can_msg;
	CAN_TxHeaderTypeDef mCan_header;
	uint8_t i =0;
	
	mCan_header.StdId  = CAN_STD_ID;		//标准标识符
	mCan_header.ExtId  = pmsg->ex_id.EX_ID;		//扩展标识符(29)
	mCan_header.IDE		 = CAN_ID_EXT;	//使用标准帧
	mCan_header.RTR 	 = CAN_RTR_DATA;  //数据帧
	mCan_header.DLC		 = pmsg->byte_count;//数据长度
	
	do
	{
		res = HAL_CAN_AddTxMessage(&hcan, &mCan_header, (uint8_t*)pmsg->data, &mTxMailbox);
		if(res == HAL_ERROR)
		{
			res = 0x81;
			i ++;
			HAL_Delay(3);
		} else if (res == HAL_OK)
		{
			res = 0;
			i = 3;
		}
	} while (i < 3);//最多填充3次
	
	return res;
}


