
#include <stdio.h>
#include <string.h>

#include "motor_control.h"
#include "hmc80x5_api.h"
#include "application.h"

motor_queue_t motorMsgSt;//电机参数结构体
/*==================================================================================
* 函 数 名： motor_position_zero
* 参    数： None
* 功能描述:  hk 电机位置归零
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-05-29 170658
==================================================================================*/
void motor_position_zero(void)
{
	motor_queue_t pMotorMsgSt;
	int i;
	int limitStatus = 0;
	int rdiv_value = 0;
	
	printf("\n 所有轴开始初始化\r\n");
	
	rdiv_value=HMC_inpw(0,50);
	printf("RDIV == %d\n",rdiv_value);
	for(i=0; i<MOTOR_NUM; i++)
	{
		Set_Axs(0,i,0,0,0,0);//先禁用使能
	}
	for(i=0; i<MOTOR_NUM; i++)
	{
		Set_Axs(0,i,0,0,0,0);//先禁用使能
	}
	for(i=0; i<MOTOR_NUM; i++)
	{
		Set_Axs(0,i,1,1,1,1);//再启用使能，逻辑位置回0
	}
	for(i=0; i<MOTOR_NUM; i++)
	{
		Set_Encorder(0,i,3,1,0,0,0);//先禁用使能
	}
	for(i=0; i<MOTOR_NUM; i++)
	{
		Set_Encorder(0,i,3,1,0,0,1);//再启用使能，逻辑位置回0
	}
//	
	
	
	//所有轴回到原点
		pMotorMsgSt = get_motor_current_state();

		
		MovToOrg(0,0,1,0,ZERO_SPEED/5);
		MovToOrg(0,1,1,0,ZERO_SPEED/2);
		MovToOrg(0,2,1,0,ZERO_SPEED/2);//此轴运动方向跟其他轴相反，归零位时
		MovToOrg(0,3,1,0,ZERO_SPEED/2);
	
//		FL_ContinueMov(0,0,1,0,ZERO_SPEED/4,ZERO_SPEED*5);//此轴运动方向跟其他轴相反，归零位时
//		FL_ContinueMov(0,1,1,0,ZERO_SPEED/4,ZERO_SPEED*5);
//		FL_ContinueMov(0,2,1,0,ZERO_SPEED/4,ZERO_SPEED*5);
//		FL_ContinueMov(0,3,1,0,ZERO_SPEED/4,ZERO_SPEED*5);
	
		
		//FL_ContinueMov(0,2,0,0,1000,2000);

	//等待电机复位
	while(1)
	{
		pMotorMsgSt = get_motor_current_state();
		if((pMotorMsgSt.queue[0].Io_state == negative_limit)&(pMotorMsgSt.queue[1].Io_state == negative_limit)&(pMotorMsgSt.queue[2].Io_state == negative_limit)&(pMotorMsgSt.queue[3].Io_state == negative_limit) )
			break;
	}

	//电机位置复位完成所有轴逻辑位置归零
	get_motor_current_state();
	for(i=0; i<MOTOR_NUM; i++)
	{
		Set_Axs(0,i,0,0,0,0);//先禁用使能
	}
	for(i=0; i<MOTOR_NUM; i++)
	{
		Set_Axs(0,i,1,1,1,1);//再启用使能，逻辑位置回0
	}
	pMotorMsgSt = get_motor_current_state();
		
	//到达低位极限后往上提升一些距离
//	for(i=0; i<MOTOR_NUM; i++)
//	{
//		DeltMov(0,i,0,0,0,1000,5000,ZREO_PLUS,0,500,500,0,0);
//	}
	HAL_Delay(1000);
	printf("所有轴初始化完成\r\n");
}

/*==================================================================================
* 函 数 名： motor_emergency_stop
* 参    数： None
* 功能描述:  hk 电机急停
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-06-29 170658
==================================================================================*/
void motor_emergency_stop(void)
{
	int i = 0;
	
	for(i=0; i<MOTOR_NUM; i++)
	{
		AxsStop(0,i);
	}
}

/*==================================================================================
* 函 数 名： get_motor_current_state
* 参    数： None
* 功能描述:  获取点击当前状态
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-06-29 170658
==================================================================================*/
motor_queue_t get_motor_current_state(void)
{	
	uint8_t SyncIO,i;
	int code_num = 0;
	int limitStatus = 0;
	motor_queue_t pMotorMsgSt;
	
	for(i=0; i<MOTOR_NUM; i++)
	{
		Read_Position(0, i, &pMotorMsgSt.queue[i].position, &pMotorMsgSt.queue[i].run_state, &pMotorMsgSt.queue[i].Io_state, &SyncIO);
		debug_print("postion[%d] = %d, run_state[%d] = %d, Io_state[%d] = %d \r\n",i,pMotorMsgSt.queue[i].position, i,pMotorMsgSt.queue[i].run_state, i,pMotorMsgSt.queue[i].Io_state);
		if(pMotorMsgSt.queue[i].Io_state == negative_limit)
		{
			Set_Axs(0,i,0,0,0,0);
			Set_Axs(0,i,1,1,1,1);//再启用使能，逻辑位置回0
		}
	}

	return pMotorMsgSt;
}
