#include "command.h"
#include "includes.h"
#include "struct_type.h"
#include "motor_control.h"
#include "udp_socket.h"
#include "hmc80x5_api.h"

//定义X Y Z 轴序号
#define  X_MOTOR 0
#define  Y_MOTOR 2
#define  Z_MOTOR 3

//盛菜动作1 运动坐标
#define X_COORDINATE_1   40000
#define Y_COORDINATE_1   65000
#define Z_COORDINATE_1   65000

//减速时间
#define DECELERATION_TIME 1000 //1000ms

//
send_queue_t	udp_queue;//udp接收队列

extern motor_queue_t motorMsgSt;//电机参数结构体

//电机复位指令
static uint8_t motor_reset_cmd[] =     {0x55,0x55,0x55};
static uint8_t motor_reset_ack_cmd[] = {0x55,0xAA,0x00,0x00,0x12,0x02,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00};
//电机急停指令
static uint8_t motor_stop_cmd[] =      {0x55,0xAA,0x00,0x00,0x12,0x01,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x90,0x00,0x01,0x00,0x01};
static uint8_t motor_stop_ack_cmd[] =  {0x55,0xAA,0x00,0x00,0x12,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x90,0x00,0x01,0x00,0x01};

//3轴相对时间播放
static uint8_t motor3_run_cmd[] =      {0x55,0xAA,0x00,0x00,0x14,0x01,0x00,0x00};
static uint8_t motor3_run_ack_cmd[] =  {0x55,0xAA,0x00,0x00,0x14,0x02};

//6轴相对时间播放
static uint8_t motor4_run_cmd[] =      {0x55,0xAA,0x00,0x00,0x14,0x01,0x00,0x01};
static uint8_t motor4_run_ack_cmd[] =  {0x55,0xAA,0x00,0x00,0x14,0x02};


//四字节大小端转换
int32_t FourBigToSmall(uint32_t a)
{
	uint32_t c;
	unsigned char b[4];
	b[0] =(unsigned char) (a);
	b[1] = (unsigned char)(a>>8);
	b[2] = (unsigned char)(a>>16);
	b[3] = (unsigned char)(a>>24);
	c = (uint32_t)(b[0]<<24) +(uint32_t)(b[1]<<16)+(uint32_t)(b[2]<<8)+ b[3];
	return c;		
}

/*==================================================================================
* 函 数 名： motor_delt_move
* 参    数： dir = 0正向运动 dir = 1负向运动
* 功能描述:  电机运行速度，位置
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-06-20 025540
==================================================================================*/
static void motor_delt_move(uint8_t axs,uint8_t dir,int runspeed,int runpostion)
{
	DeltMov(0,axs,0,dir,0,1000,runspeed,runpostion,0,500,500,0,0);
}

/*==================================================================================
* 函 数 名： motor_relative_time_run
* 参    数： None
* 功能描述:  电机相对时间运行
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-06-20 025540
==================================================================================*/
static void motor_relative_time_run(void* ret_msg)
{
	uint8_t i,dir;
	int plus = 0;
	p_motor_queue_t pmsg = ret_msg;
	
	for(i=0;i<MOTOR_NUM;i++)
	{
		if(pmsg->queue[i].tagetPosition > pmsg->queue[i].position) {
			plus = pmsg->queue[i].tagetPosition - pmsg->queue[i].position;
			dir = 0;
		} else {
			plus = pmsg->queue[i].position - pmsg->queue[i].tagetPosition;
			dir = 1;
		}
		pmsg->queue[i].runSpeed = (plus*1000)/PLUS_RATE;
		if(pmsg->queue[i].runSpeed>LIMIT_SPEED)
		{
			pmsg->queue[i].runSpeed = LIMIT_SPEED;
		}
		debug_print("speed = %d, tagetPosition = %d , position = %d \r\n", pmsg->queue[i].runSpeed, pmsg->queue[i].tagetPosition, pmsg->queue[i].position);
		motor_delt_move(i,dir, pmsg->queue[i].runSpeed, plus);
	}
}

/*==================================================================================
* 函 数 名： udp_frame_parse
* 参    数： None
* 功能描述:  udp socket 接收到的数据解析
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-06-20 025540
==================================================================================*/
uint32_t motorSpeed = ZERO_SPEED;
void udp_frame_parse(void* ret_msg)
{
	_pRet_Msg pmsg = ret_msg;
	uint8_t buffer[8] = {0};
	uint16_t ret_id = 0;
	uint8_t ret_s = 0,i;
	uint32_t temp;
	int dir = 0;
	motor_queue_t pMotorMsgSt;
	uint8_t SyncIO;

	int runPlus[6];
	uint8_t RunDataBuf[32];
	
	memset(pmsg->data, MAX_MUTIL_LEN, 0);
	debug_print("udp rec data = ");
	debug_print_hex(pmsg->data, pmsg->byte_count);
	debug_print("\r\n");
	
	//进行协议解析
	//电机复位
	if(!memcmp(pmsg->data,motor_reset_cmd,sizeof(motor_reset_cmd)))
	{
		debug_print("电机复位指令 \r\n");
		motor_position_zero();
		
		//电机复位完成应答回复
		debug_print("motor_reset_ack_cmd = %d \r\n ",sizeof(motor_reset_ack_cmd));
		udp_socket_send_data(motor_reset_ack_cmd,sizeof(motor_reset_ack_cmd));	
	}
	//电机急停
	if(!memcmp(pmsg->data,motor_stop_cmd,sizeof(motor_stop_cmd)))
	{
		debug_print("电机急停指令 \r\n");
		motor_emergency_stop();
		
		//电机急停完成应答回复
		debug_print("motor_stop_ack_cmd = %d \r\n ",sizeof(motor_stop_ack_cmd));
		udp_socket_send_data(motor_stop_ack_cmd,sizeof(motor_stop_ack_cmd));
	}
	//3轴电机相对时间播放
	if(!memcmp(pmsg->data,motor3_run_cmd,6))
	{
		debug_print("电机3轴电机运行指令 \r\n");
		memcpy(RunDataBuf,pmsg->data+20,12);
		//电机停止运行
		//读取电机当前位置
		motorMsgSt = get_motor_current_state();
		for(i=0;i<MOTOR_NUM;i++)
		{
			motorMsgSt.queue[i].tagetPosition = *(uint32_t *)(RunDataBuf+i*4);
			motorMsgSt.queue[i].tagetPosition = FourBigToSmall(motorMsgSt.queue[i].tagetPosition);
			//motorMsgSt.queue[i].tagetPosition -= ZREO_PLUS;
		}
		//第四轴代替第二轴(第二轴运动方向与其它轴运行不一致，用第四轴代替第二轴)
		motorMsgSt.queue[3].tagetPosition = motorMsgSt.queue[1].tagetPosition;
		motor_relative_time_run(&motorMsgSt);
	}
	//6轴电机相对时间播放
	if(!memcmp(pmsg->data,motor4_run_cmd,8))
	{
	
	}	
	
	//电机测试指令
	//pmsg->data[1]运动轴；pmsg->data[2]运动方向
	if(pmsg->data[0] == 0xAA &pmsg->data[1] != 0xFF)
	{
		for(i=0;i<MOTOR_NUM ;i++)
		{
			Set_Axs(0,i,0,0,0,0);//先禁用使能
		}
		for(i=0;i<MOTOR_NUM ;i++)
		{
			Set_Axs(0,i,1,1,1,1);//再启用使能，逻辑位置回0
		}
		debug_print("单电机电机测试指令 \r\n");
		
		//MovToOrg(0,pmsg->data[1],pmsg->data[2],0,motorSpeed);//电机以最低速度持续运动
		switch(pmsg->data[1])
		{
			case 0:
				//MovToOrg(0,pmsg->data[1],pmsg->data[2],0,motorSpeed);//电机以最低速度持续运动
				FH_ContinueMov(0,pmsg->data[1],pmsg->data[2],0,100,motorSpeed,1);
			break;
			case 1:
				//MovToOrg(0,pmsg->data[1],pmsg->data[2],0,motorSpeed);//电机以最低速度持续运动
				FH_ContinueMov(0,pmsg->data[1],pmsg->data[2],0,100,motorSpeed,1);
			break;
			case 3:
				//MovToOrg(0,pmsg->data[1],pmsg->data[2],0,7000);//电机以最低速度持续运动
				FH_ContinueMov(0,pmsg->data[1],pmsg->data[2],0,motorSpeed/2,motorSpeed,1);
			break;
			case 2:
				FH_ContinueMov(0,pmsg->data[1],pmsg->data[2],0,1000,motorSpeed,1);
			break;
		}
	} 
	else if(pmsg->data[0] == 0xAA &pmsg->data[1] == 0xFF)
	{//所有电机统一运行
		debug_print("多电机统一测试指令 \r\n");
		for(i=0;i<MOTOR_NUM ;i++)
		{
			Set_Axs(0,i,0,0,0,0);//先禁用使能
		}
		for(i=0;i<MOTOR_NUM ;i++)
		{
			Set_Axs(0,i,1,1,1,1);//再启用使能，逻辑位置回0
		}
		if(pmsg->data[2] != 0xFF)
		{
			for(i=0;i<MOTOR_NUM;i++)
			{
				MovToOrg(0,i,pmsg->data[2],0,ZERO_SPEED);
			}

		} else {//电机全部停止运动
			for(i=0;i<MOTOR_NUM;i++)
			{
				AxsStop(0,i);
			}
		}
	}
	//电机加减速
	if(pmsg->data[0] == 0xA0 &pmsg->data[1] == 0)//加速
	{
		motorSpeed += 100;
		debug_print("电机加速速度 = %d \n", motorSpeed);
	}
	if(pmsg->data[0] == 0xA0 &pmsg->data[1] == 1)//减速
	{
		if(motorSpeed)
			motorSpeed -= 100;
		debug_print("电机减速速度 = %d \n", motorSpeed);
	}
	
	//电机位置控制模式 电机组合动作
	if(pmsg->data[0] == 0xBB)
	{
		switch(pmsg->data[1])//菜品标号
		{
			case 0://电机归位
				debug_print("做完动作后电机归位\n");
				for(i=0; i<MOTOR_NUM; i++)
				{
					Read_Position(0, i, &pMotorMsgSt.queue[i].position, &pMotorMsgSt.queue[i].run_state, &pMotorMsgSt.queue[i].Io_state, &SyncIO);
				}
				dir = 1;//运动方向
//				DeltMov(0,X_MOTOR,0,dir,0,1000,LIMIT_SPEED,pMotorMsgSt.queue[X_MOTOR].position,0,DECELERATION_TIME,DECELERATION_TIME,0,0);//X轴电机运动距离
//				DeltMov(0,Y_MOTOR,0,dir,0,1000,LIMIT_SPEED,pMotorMsgSt.queue[Y_MOTOR].position,0,DECELERATION_TIME,DECELERATION_TIME,0,0);//y轴电机运动距离
//				DeltMov(0,Z_MOTOR,0,dir,0,1000,LIMIT_SPEED,pMotorMsgSt.queue[Z_MOTOR].position,0,DECELERATION_TIME,DECELERATION_TIME,0,0);//Z轴电机运动距离
				
				AXIS_Interpolation3(0,X_MOTOR,Y_MOTOR,Z_MOTOR,0,dir,dir,dir,0,1000,LIMIT_SPEED,pMotorMsgSt.queue[X_MOTOR].position,pMotorMsgSt.queue[Y_MOTOR].position,pMotorMsgSt.queue[Z_MOTOR].position,0,DECELERATION_TIME,DECELERATION_TIME,0,0);
			break;
				
			case 1:
				debug_print("盛菜品1 动作编辑\n");
				dir = 0;//运动方向
//				DeltMov(0,X_MOTOR,1,dir,0,1000,LIMIT_SPEED,X_COORDINATE_1,0,DECELERATION_TIME,DECELERATION_TIME,0,0);//X轴电机运动距离
//				DeltMov(0,Y_MOTOR,1,dir,0,1000,LIMIT_SPEED,Y_COORDINATE_1,0,DECELERATION_TIME,DECELERATION_TIME,0,0);//y轴电机运动距离
//				DeltMov(0,Z_MOTOR,1,dir,0,1000,LIMIT_SPEED,Z_COORDINATE_1,0,DECELERATION_TIME,DECELERATION_TIME,0,0);//Z轴电机运动距离
				
				AXIS_Interpolation3(0,X_MOTOR,Y_MOTOR,Z_MOTOR,0,dir,dir,dir,0,1000,LIMIT_SPEED,X_COORDINATE_1,Y_COORDINATE_1,Z_COORDINATE_1,0,DECELERATION_TIME,DECELERATION_TIME,0,0);
			break;
			
			case 2:
				debug_print("盛菜品2 动作编辑\n");
			break;
			
			case 3:
				debug_print("盛菜品3 动作编辑\n");
			break;
			
			case 4:
				debug_print("盛菜品4 动作编辑\n");
			break;
		}
	}
}

/*==================================================================================
* 函 数 名： udp_instance_init
* 参    数： None
* 功能描述:  udp初始化
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-10-28 170617
=================================================================================*/
void udp_instance_init(void)
{
	msg_queue_init(&udp_queue);
}

/*==================================================================================
* 函 数 名： udp_push_data_msg
* 参    数： None
* 功能描述:  udp 接收到数据后调用此函数把接收的数据添加的队列里面
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-09-29 170658
==================================================================================*/
void udp_push_data_msg(void* ret_msg)
{
	_pUdp_Msg pmsg = ret_msg;  
	
	msg_queue_push(&udp_queue, pmsg);
}

/*==================================================================================
* 函 数 名： udp_pop_one_frame
* 参    数： _pRet_Msg
* 功能描述:  udp数据接收
* 返 回 值： None
* 备    注： None
* 作    者： lc
* 创建时间： 2021-06-20 154449
==================================================================================*/ 
static uint8_t udp_pop_one_frame(void )
{
	int num;
	p_send_queue_t p_queue_buff = &udp_queue;
	
	//检查队列里面是否有数据
	num = msg_queue_num(p_queue_buff);
	if(num)
	{
		for(int i=0; i<MAX_CACHE_NUM; i++)
		{
			if(p_queue_buff->queue[i].byte_count)
			{
				//debug_print("队列里面的消息 =%s \n", p_queue_buff->queue[i].data);
				debug_print_hex(p_queue_buff->queue[i].data,10);
				
				udp_frame_parse(&(p_queue_buff->queue[i]));
				msg_queue_pop(p_queue_buff, 0);
				//return 0;
			}	
		}
		//debug_print("队列里面的消息个数 =%d \n",num);
	}
}

/*==================================================================================
* 函 数 名： udp_rec_decode_task
* 参    数： None
* 功能描述:  udp 接收数据解析任务
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-09-29 170658
==================================================================================*/
void udp_rec_decode_task(void* argv)
{
	udp_pop_one_frame();
}
