#include "ex_debug_func.h"
#include "includes.h"
#include "debug_task.h"

extern _App_Param mApp_Param;
/*==================================================================================
* 函 数 名： print_si446x_irq
* 参    数： argv[0] == 0xFF 打印详细信息
* 功能描述:  使能si信息打印
* 返 回 值： None
* 备    注： si_irq_print
* 作    者： xiaozh
* 创建时间： 2019-11-14 113231
==================================================================================*/
void DEF_NAME_FUNC(SI_INT_STATE)(void *argc, void *argv)
{
	if(strstr(argc, "help") != 0)
	{
		printf("命令:[SI_INT_STATE]\t参数:[NONE]\t(注：打印 SI4463->GET_INT_STATUS)\n");
		return ;
	}
}
 
/*==================================================================================
* 函 数 名： print_si446x_irq
* 参    数： argv[0] == 0xFF 打印详细信息
* 功能描述:  使能si信息打印
* 返 回 值： None
* 备    注： si_irq_print
* 作    者： xiaozh
* 创建时间： 2019-11-14 113231
==================================================================================*/
void DEF_NAME_FUNC(SI_RESET)(void *argc, void *argv)
{
	if(strstr(argc, "help") != 0)
	{
		printf("命令:[SI_RESET]\t参数:[NONE]\t(注：打印 SI4463->RESET)\n");
		return ;
	}
	printf(" si446x reset complete state :");
}

/*==================================================================================
* 函 数 名： print_si446x_irq
* 参    数： argv[0] == 0xFF 打印详细信息
* 功能描述:  使能si信息打印
* 返 回 值： None
* 备    注： si_dev_state
* 作    者： xiaozh
* 创建时间： 2019-11-14 113231
==================================================================================*/ 
void DEF_NAME_FUNC(SI_DEV_STATE)(void *argc, void* argv) 
{ 
 	if(strstr(argc, "help") != 0)
	{
		printf("命令:[SI_DEV_STATE]\t参数:[NONE]\t(注：打印 SI4463->REQUEST_DEVICE_STATE)\n");
		return ;
	}
	
	//函数功能
	uint8_t ret_buff[16] = {0};
 
	printf(" device state :");
	print_hex(ret_buff, 3);
	printf("\n");
}

/*==================================================================================
* 函 数 名： print_si446x_name_info
* 参    数： argv[0] == 0xFF 打印详细信息
* 功能描述:  打印si4463命令列表返回值
* 返 回 值： None
* 备    注： si_dev_state
* 作    者： xiaozh
* 创建时间： 2019-11-14 113231
==================================================================================*/
void DEF_NAME_FUNC(SI_NAME_RET)(void *argc, void *argv)
{ 
	uint8_t argv0=0, argv1=0;
	uint8_t ret_buff[16] = {0}; 
	
 	if(strstr(argc, "help") != 0)
	{
		printf("命令:[SI_NAME_RET]\t参数:[SI4463 NAME] [返回值长度] \t(注：打印命令执行返回值)\n");
		return ;
	}
	argv0 = (((uint32_t*)argv)[0]&0x00FF);
	argv1 = (((uint32_t*)argv)[1]&0x00FF);
	
	if(argv1 > 16)
	{
		printf("参数不合法, 接收长度要小于16\n");
		return ;
	}
	printf("read info:");
	print_hex(ret_buff, argv1);
	printf("\n");
}
