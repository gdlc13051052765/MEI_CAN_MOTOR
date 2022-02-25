#include "interface.h"
#include "includes.h" 
#include "application.h"
#include "st_flash.h"

/*==================================================================================
* �� �� ���� inter_init
* ��    ���� None
* ��������:  �ӿ�Э���ʼ��
* �� �� ֵ�� None
* ��    ע�� Ĭ�ϼ��10ms���н������ݽ���
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-09-23 180348
==================================================================================*/
void inter_init(void)
{  
	
}

/*==================================================================================
* �� �� ���� read_config_param
* ��    ���� _pApp_Config_Param
* ��������:  ��ȡ���ò���
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-09-06 115746
==================================================================================*/
uint8_t read_config_param(void* c_param)
{
	_pApp_Param pmsg = c_param;
	
	stFlash_Func.read(APP_CONFIG_ADDR, pmsg->config_param.flash_buff, CONFIG_DATA_LEN);	//��ȡ����
	//�жϲ����Ƿ�Ϸ�
	if(crc32(pmsg->config_param.flash_buff, pmsg->config_param._Use_Param.crc_data_len) == pmsg->config_param._Use_Param.crc32)	//�ж�У���Ƿ�Ϸ�
	{
		//�ж�ÿһ������Ƿ�Ϸ�
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
		//У�����ʹ��Ĭ�����ò���
		pmsg->can_addr = 1;
		pmsg->hard_ver = HARD_VERSION;
		pmsg->soft_ver = SOFT_VERSION; 
	}
	return 0;
}
/*==================================================================================
* �� �� ���� write_config_param
* ��    ���� _pApp_Config_Param
* ��������:  �޸����ò���
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-02-05 115746
==================================================================================*/
void write_config_param(_Config_cmd cmd,uint8_t data)
{
	_pApp_Param pmsg ;
	
	stFlash_Func.read(APP_CONFIG_ADDR, pmsg->config_param.flash_buff, CONFIG_DATA_LEN);	//��ȡ����	
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
	stFlash_Func.write(APP_CONFIG_ADDR, pmsg->config_param.flash_buff, CONFIG_DATA_LEN);	//д����
}
/*==================================================================================
* �� �� ���� can_rev_decode
* ��    ���� None
* ��������:  canЭ�����
* �� �� ֵ�� None
* ��    ע�� none
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-10-28 172557
==================================================================================*/
void can1_rev_decode(void)
{
	_Ret_Msg c_msg; 
	 

}


/*==================================================================================
* �� �� ���� HAL_CAN_RxFifo0MsgPendingCallback
* ��    ���� None
* ��������:  �������
* �� �� ֵ�� None
* ��    ע�� �̰�
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-23 180101
==================================================================================*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) 
{
	
}

/*==================================================================================
* �� �� ���� HAL_CAN_RxFifo0MsgPendingCallback
* ��    ���� None
* ��������:  �������
* �� �� ֵ�� None
* ��    ע�� ����
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-23 180101
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
* �� �� ���� cc_radio_send_to_android_task
* ��    ���� None
* ��������:  CC�㲥���ݵ���׿
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� lc
* ����ʱ�䣺 2021-02-05 105001
==================================================================================*/
void cc_radio_send_to_android_task(void* argv)
{
	
}
