#include "interface.h"
#include "includes.h" 
#include "application.h"
#include "st_flash.h"

/*==================================================================================
* 函 数 名： inter_init
* 参    数： None
* 功能描述:  接口协议初始化
* 返 回 值： None
* 备    注： 默认间隔10ms进行接收数据解析
* 作    者： xiaozh
* 创建时间： 2019-09-23 180348
==================================================================================*/
void inter_init(void)
{  
	
}

/*==================================================================================
* 函 数 名： read_config_param
* 参    数： _pApp_Config_Param
* 功能描述:  读取配置参数
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2019-09-06 115746
==================================================================================*/
uint8_t read_config_param(void* c_param)
{
	_pApp_Param pmsg = c_param;
	
	stFlash_Func.read(APP_CONFIG_ADDR, pmsg->config_param.flash_buff, CONFIG_DATA_LEN);	//读取参数
	//判断参数是否合法
	if(crc32(pmsg->config_param.flash_buff, pmsg->config_param._Use_Param.crc_data_len) == pmsg->config_param._Use_Param.crc32)	//判断校验是否合法
	{
		//判断每一项参数是否合法
		if(pmsg->config_param._Use_Param.can_addr == ((~pmsg->config_param._Use_Param.can_addr_n)&0x000000FF))
		{
			pmsg->can_addr = pmsg->config_param._Use_Param.can_addr;
		}
		
		if(pmsg->config_param._Use_Param.hard_ver == ((~pmsg->config_param._Use_Param.hard_ver_n)&0x000000FF))
		{
			pmsg->hard_ver = pmsg->config_param._Use_Param.hard_ver;
		}
		
		if(pmsg->config_param._Use_Param.hard_ver == ((~pmsg->config_param._Use_Param.hard_ver_n)&0x000000FF))
		{
			pmsg->hard_ver = pmsg->config_param._Use_Param.hard_ver;
		}
		
		if(pmsg->config_param._Use_Param.soft_ver == ((~pmsg->config_param._Use_Param.soft_ver_n)&0x000000FF))
		{
			pmsg->soft_ver = pmsg->config_param._Use_Param.soft_ver;
		}
		  
	}
	else
	{
		//校验出错使用默认配置参数
		pmsg->can_addr = 1;
		pmsg->hard_ver = HARD_VERSION;
		pmsg->soft_ver = SOFT_VERSION; 
	}
	return 0;
}
/*==================================================================================
* 函 数 名： write_config_param
* 参    数： _pApp_Config_Param
* 功能描述:  修改配置参数
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-02-05 115746
==================================================================================*/
void write_config_param(_Config_cmd cmd,uint8_t data)
{
	_pApp_Param pmsg ;
	
	stFlash_Func.read(APP_CONFIG_ADDR, pmsg->config_param.flash_buff, CONFIG_DATA_LEN);	//读取参数	
	pmsg->config_param._Use_Param.crc_data_len = CONFIG_DATA_LEN -4;
	pmsg->config_param._Use_Param.crc_data_len_n = ~pmsg->config_param._Use_Param.crc_data_len;
	switch(cmd)
	{
		case Cc_Set_Addr:
			pmsg->can_addr = data;
		break;
		case Cc_Set_Soft_Ver:
			pmsg->soft_ver = data;			
		break;
		case Cc_Set_Hard_Ver:
			pmsg->hard_ver = data;
		break;
	}

	pmsg->config_param._Use_Param.can_addr = pmsg->can_addr;
	pmsg->config_param._Use_Param.can_addr_n = ~pmsg->can_addr;
	pmsg->config_param._Use_Param.hard_ver = pmsg->hard_ver;
	pmsg->config_param._Use_Param.hard_ver_n = ~pmsg->hard_ver;
	pmsg->config_param._Use_Param.soft_ver = pmsg->soft_ver;
	pmsg->config_param._Use_Param.soft_ver_n = ~pmsg->soft_ver;
	
	pmsg->config_param._Use_Param.crc32 = crc32(pmsg->config_param.flash_buff, pmsg->config_param._Use_Param.crc_data_len);
	stFlash_Func.write(APP_CONFIG_ADDR, pmsg->config_param.flash_buff, CONFIG_DATA_LEN);	//写参数
}
/*==================================================================================
* 函 数 名： can_rev_decode
* 参    数： None
* 功能描述:  can协议解析
* 返 回 值： None
* 备    注： none
* 作    者： xiaozh
* 创建时间： 2019-10-28 172557
==================================================================================*/
void can1_rev_decode(void)
{
	_Ret_Msg c_msg; 
	 

}


/*==================================================================================
* 函 数 名： HAL_CAN_RxFifo0MsgPendingCallback
* 参    数： None
* 功能描述:  接收完成
* 返 回 值： None
* 备    注： 短包
* 作    者：  
* 创建时间： 2019-09-23 180101
==================================================================================*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) 
{
	
}

/*==================================================================================
* 函 数 名： HAL_CAN_RxFifo0MsgPendingCallback
* 参    数： None
* 功能描述:  接收完成
* 返 回 值： None
* 备    注： 长包
* 作    者：  
* 创建时间： 2019-09-23 180101
==================================================================================*/
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef* hcan) 
{
	
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	debug_print("can error\n");
}
static void can_send_mcuid_to_android(char cmd,char msg_id)
{

}
/*==================================================================================
* 函 数 名： cc_radio_send_to_android_task
* 参    数： None
* 功能描述:  CC广播数据到安卓
* 返 回 值： None
* 备    注： None
* 作    者： lc
* 创建时间： 2021-02-05 105001
==================================================================================*/
void cc_radio_send_to_android_task(void* argv)
{
	
}
