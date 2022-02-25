#include "application.h"
#include "includes.h"
#include "hmc80x5_api.h"
#include "motor_control.h"

_App_Param mApp_Param = 
{ 
	.can_addr 		= 0x00,
	.soft_ver			= 0x00,  
	.hard_ver			= 0xFF,
};
 
_Serial_Info mSerial_Info;
 
/**
  * @brief System Clock Configuration
  * @retval None
  */
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

  HAL_RCC_EnableCSS();
}

/*==================================================================================
* �� �� ���� systerm_init
* ��    ���� None
* ��������:  ϵͳ��ʼ
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-09-17 121318
==================================================================================*/
void systerm_init(void)
{
	//�ж��������ʼ��
//#if  EN_DEBUG
//	SCB->VTOR = FLASH_BASE | 0X03000; /* Vector Table Relocation in Internal FLASH. */
//#else
//  
//#endif
	
	HAL_Init();
	
	SystemClock_Config();
	/*===========�弶��ʼ��==========*/ 
	//��ȡ���ò���
	read_config_param(&mApp_Param);
	/*===========�弶��ʼ��==========*/
	bsp_init();
	st_crc_init(); 
	//debug_print("can addr = %d\n",mApp_Param.can_addr);
	/*============����ϵͳ�ж�========*/
//	HAL_NVIC_EnableIRQ(EXTI0_IRQn); 
//	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	
	//box_info_init();

	debug_print("systerm init is complete!\n");
}

/*==================================================================================
* �� �� ���� app_loop_task
* ��    ���� None
* ��������:  ����
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-09-17 141735
==================================================================================*/ 
void app_dispatch(void)
{
	//��ʱ�������
	TaskProcess(0);  
	
	//����ҵ����
	//si_radio_event_handle(DEFAULT_SED_SI);
	  
	//��������Э��
	//radio_frame_param(DEFAULT_SED_SI); 
	 
	//CANЭ�����
	//can1_rev_decode();
	//can_sed_loop_check(hcan1);
	//debug ����
	//cmd_handle_func_task();
}


/*==================================================================================
* �� �� ���� HAL_SysTick_Callback
* ��    ���� None
* ��������:  ϵͳ�δ�ص�
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-09-06 115746
==================================================================================*/
void HAL_SysTick_Callback(void)
{
	//�������
	TaskRemarks();
}



typedef struct
{
    uint16_t Head;
    uint16_t len;
    uint8_t instruction;
    uint8_t data[150];
    uint32_t crc;
}SlaveTestCmdTypeDef;


uint8_t Slave_SendTestCmd(void)
{
    SlaveTestCmdTypeDef TestCmd;
    TestCmd.Head = 0xAA55;
    TestCmd.len = sizeof(SlaveTestCmdTypeDef) - 9;
    TestCmd.instruction = 0x04;
    memset(TestCmd.data, 0xff, 150);
    TestCmd.crc = st_crc32(((uint8_t *) &TestCmd ) + 5,sizeof(SlaveTestCmdTypeDef) - 9);

//    Slave_AddTranceiverMessage((uint8_t *) &TestCmd,sizeof(SlaveTestCmdTypeDef),0);
	//si_sed_buff_to(0x0E, 0X55, (uint8_t *) &TestCmd, sizeof(SlaveTestCmdTypeDef), 1);  
	
	return 0;
}


/*==================================================================================
* �� �� ���� test_loop_task
* ��    ���� None
* ��������:  ��������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-09-29 170658
==================================================================================*/
bool bDirflag =false;
extern motor_queue_t motorMsgSt;//��������ṹ��
void test_loop_task(void* argv)
{
	int err =-1;
	set_led_toggle();
	
	get_motor_current_state();
//	if(motorMsgSt.queue[0].Io_state == 4)
//	{
//		Set_Axs(0,0,0,0,0,0);
//		Set_Axs(0,0,1,1,1,1);//������ʹ�ܣ��߼�λ�û�0
//		get_motor_current_state();

//	if(bDirflag==false)
//	{	
//		bDirflag = true;
//		DeltMov(0,0,0,0,0,1000,5000,50000,0,500,500,0,0);
//		DeltMov(0,1,0,0,0,1000,5000,50000,0,500,500,0,0);
//		DeltMov(0,2,0,0,0,1000,5000,50000,0,500,500,0,0);
//		DeltMov(0,3,0,0,0,1000,5000,50000,0,500,500,0,0);
//	}else{
//		bDirflag = false;
//		DeltMov(0,0,0,1,0,1000,5000,50000,0,500,500,0,0);
//		DeltMov(0,1,0,1,0,1000,5000,50000,0,500,500,0,0);
//		DeltMov(0,2,0,1,0,1000,5000,50000,0,500,500,0,0);
//		DeltMov(0,3,0,1,0,1000,5000,50000,0,500,500,0,0);
//	}
}
