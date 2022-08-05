#include "command.h"
#include "includes.h"
#include "struct_type.h"
#include "motor_control.h"
#include "udp_socket.h"
#include "hmc80x5_api.h"
#include "hwioconfig.h"

//����X Y Z �����
#define  X_MOTOR 0
#define  Y_MOTOR 2
#define  Z_MOTOR 3

//ʢ�˶���1 �˶�����
#define X_COORDINATE_1   30000
#define Y_COORDINATE_1   60000
#define Z_COORDINATE_1   30000
//ʢ�˶���2 �˶�����
#define X_COORDINATE_2   30000
#define Y_COORDINATE_2   40000
#define Z_COORDINATE_2   30000
//ʢ�˶���3 �˶�����
#define X_COORDINATE_3   30000
#define Y_COORDINATE_3   20000
#define Z_COORDINATE_3   30000
//ʢ�˶���4 �˶�����
#define X_COORDINATE_4   30000
#define Y_COORDINATE_4   1000
#define Z_COORDINATE_4   30000

//Z������ȡ��λ��
#define Z_DOWN_TRANCE   50000

//������
#define Y_SPACE	18000

//����ʱ��
#define DECELERATION_TIME 1000 //1000ms

//����������
#define CHECK_ZERO_POSTION 150

//
send_queue_t	udp_queue;//udp���ն���

extern motor_queue_t motorMsgSt;//��������ṹ��

//�����λָ��
static uint8_t motor_reset_cmd[] =     {0x55,0x55,0x55};
static uint8_t motor_reset_ack_cmd[] = {0x55,0xAA,0x00,0x00,0x12,0x02,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00};
//�����ָͣ��
static uint8_t motor_stop_cmd[] =      {0x55,0xAA,0x00,0x00,0x12,0x01,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x90,0x00,0x01,0x00,0x01};
static uint8_t motor_stop_ack_cmd[] =  {0x55,0xAA,0x00,0x00,0x12,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x90,0x00,0x01,0x00,0x01};

//3�����ʱ�䲥��
static uint8_t motor3_run_cmd[] =      {0x55,0xAA,0x00,0x00,0x14,0x01,0x00,0x00};
static uint8_t motor3_run_ack_cmd[] =  {0x55,0xAA,0x00,0x00,0x14,0x02};

//6�����ʱ�䲥��
static uint8_t motor4_run_cmd[] =      {0x55,0xAA,0x00,0x00,0x14,0x01,0x00,0x01};
static uint8_t motor4_run_ack_cmd[] =  {0x55,0xAA,0x00,0x00,0x14,0x02};


//���ֽڴ�С��ת��
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
* �� �� ���� motor_delt_move
* ��    ���� dir = 0�����˶� dir = 1�����˶�
* ��������:  ��������ٶȣ�λ��
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-06-20 025540
==================================================================================*/
static void motor_delt_move(uint8_t axs,uint8_t dir,int runspeed,int runpostion)
{
	DeltMov(0,axs,0,dir,0,1000,runspeed,runpostion,0,500,500,0,0);
}

/*==================================================================================
* �� �� ���� motor_relative_time_run
* ��    ���� None
* ��������:  ������ʱ������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-06-20 025540
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
* �� �� ���� make_rice_move_run
* ��    ���� num ����λ��
* ��������:  ȡ������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-06-20 025540
==================================================================================*/
static void make_rice_move_run(int num)
{
	uint8_t buffer[8] = {0};
	uint16_t ret_id = 0;
	uint8_t ret_s = 0,i;
	uint32_t temp,X_COORDINATE,Y_COORDINATE,Z_COORDINATE;
	int dir = 0;
	motor_queue_t pMotorMsgSt;
	uint8_t SyncIO;
	int runPlus[6];
	uint8_t RunDataBuf[32];
		
	switch(num)
	{
		case 1:
			X_COORDINATE = X_COORDINATE_1;
			Y_COORDINATE = Y_COORDINATE_1;
			Z_COORDINATE = Z_COORDINATE_1;
		break;
		
		case 2:
			X_COORDINATE = X_COORDINATE_2;
			Y_COORDINATE = Y_COORDINATE_2;
			Z_COORDINATE = Z_COORDINATE_2;
		break;
		
		case 3:
			X_COORDINATE = X_COORDINATE_3;
			Y_COORDINATE = Y_COORDINATE_3;
			Z_COORDINATE = Z_COORDINATE_3;
		break;
		
		case 4:
			X_COORDINATE = X_COORDINATE_4;
			Y_COORDINATE = Y_COORDINATE_4;
			Z_COORDINATE = Z_COORDINATE_4;
		break;
	}
	//�������ȹ���У׼
	motor_position_zero();
	//��һ��������
	control_spoon_action(0); 
	debug_print("�����ſ�\n");
	//�����˶�������λ��
	dir = 0;//�˶�����
	AXIS_Interpolation3(0,X_MOTOR,Y_MOTOR,Z_MOTOR,0,dir,dir,dir,0,1000,LIMIT_SPEED,X_COORDINATE,Y_COORDINATE, Z_COORDINATE,0,DECELERATION_TIME,DECELERATION_TIME,0,0);
	//�ȴ��������ᵽ��ָ��λ��
	while(1)
	{
		for(i=0; i<MOTOR_NUM; i++)
		{
			Read_Position(0, i, &pMotorMsgSt.queue[i].position, &pMotorMsgSt.queue[i].run_state, &pMotorMsgSt.queue[i].Io_state, &SyncIO);
		}
		if(pMotorMsgSt.queue[0].position >=X_COORDINATE &&  pMotorMsgSt.queue[2].position >=Y_COORDINATE && pMotorMsgSt.queue[3].position >=Z_COORDINATE)
		{
			break;
		}	
	}
	//HAL_Delay(4000);
	get_motor_current_state();
	debug_print("������һ�����\n");
	//z������һ���������뵽������
	DeltMov(0,Z_MOTOR,1,dir,0,1000,LIMIT_SPEED,Z_DOWN_TRANCE,0,DECELERATION_TIME,DECELERATION_TIME,0,0);//Z�����˶�����
	//HAL_Delay(4000);
	//�ȴ��������ᵽ��ָ��λ��
	while(1)
	{
		for(i=0; i<MOTOR_NUM; i++)
		{
			Read_Position(0, i, &pMotorMsgSt.queue[i].position, &pMotorMsgSt.queue[i].run_state, &pMotorMsgSt.queue[i].Io_state, &SyncIO);
		}
		if(pMotorMsgSt.queue[3].position >=Z_COORDINATE + Z_DOWN_TRANCE)
		{
			break;
		}	
	}
	//����һ�߹رգ�Z����������
	dir = 1;//�˶�����
	control_spoon_action(1); 
	debug_print("���ӹر�\n");
	DeltMov(0,Z_MOTOR,1,dir,0,100,LIMIT_SPEED/3,Z_DOWN_TRANCE,0,DECELERATION_TIME,DECELERATION_TIME,0,0);//Z�����˶�����
	//HAL_Delay(6000);
	while(1)
	{
		for(i=0; i<MOTOR_NUM; i++)
		{
			Read_Position(0, i, &pMotorMsgSt.queue[i].position, &pMotorMsgSt.queue[i].run_state, &pMotorMsgSt.queue[i].Io_state, &SyncIO);
		}
		if(pMotorMsgSt.queue[3].position <=Z_COORDINATE)
		{
			break;
		}	
	}

	//X Y��ص���ʼλ��
	for(i=0; i<MOTOR_NUM; i++)
	{
		Read_Position(0, i, &pMotorMsgSt.queue[i].position, &pMotorMsgSt.queue[i].run_state, &pMotorMsgSt.queue[i].Io_state, &SyncIO);
	}
	dir = 1;//�˶�����
	AXIS_Interpolation2(0,X_MOTOR,Y_MOTOR,0,dir,dir,0,1000,LIMIT_SPEED,pMotorMsgSt.queue[X_MOTOR].position,pMotorMsgSt.queue[Y_MOTOR].position,0,DECELERATION_TIME,DECELERATION_TIME,0,0);

	//Z�����·Ų�
	HAL_Delay(3000);
	dir = 0;//�˶�����
	DeltMov(0,Z_MOTOR,1,dir,0,1000,LIMIT_SPEED,35000,0,DECELERATION_TIME,DECELERATION_TIME,0,0);//Z�����˶�����
	HAL_Delay(3000);
	control_spoon_action(0); 
	debug_print("�����ſ�\n");

	//����˺�Z��ص���λ
	HAL_Delay(3000);
	debug_print("���ӹر�\n");
	control_spoon_action(1); 
	dir = 1;//�˶�����
	DeltMov(0,Z_MOTOR,1,dir,0,1000,LIMIT_SPEED,Z_COORDINATE+35000,0,DECELERATION_TIME,DECELERATION_TIME,0,0);//Z�����˶�����
}
/*==================================================================================
* �� �� ���� udp_frame_parse
* ��    ���� None
* ��������:  udp socket ���յ������ݽ���
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-06-20 025540
==================================================================================*/
uint32_t motorSpeed = ZERO_SPEED;
void udp_frame_parse(void* ret_msg)
{
	_pRet_Msg pmsg = ret_msg;
	uint8_t buffer[8] = {0};
	uint16_t ret_id = 0;
	uint8_t ret_s = 0,i;
	uint32_t temp,X_COORDINATE,Y_COORDINATE,Z_COORDINATE;
	int dir = 0;
	motor_queue_t pMotorMsgSt;
	uint8_t SyncIO;

	int runPlus[6];
	uint8_t RunDataBuf[32];
	
	memset(pmsg->data, MAX_MUTIL_LEN, 0);
	debug_print("udp rec data = ");
	debug_print_hex(pmsg->data, pmsg->byte_count);
	debug_print("\r\n");
	
	//����Э�����
	//�����λ
	if(!memcmp(pmsg->data,motor_reset_cmd,sizeof(motor_reset_cmd)))
	{
		debug_print("�����λָ�� \r\n");
		motor_position_zero();
		
		//�����λ���Ӧ��ظ�
		debug_print("motor_reset_ack_cmd = %d \r\n ",sizeof(motor_reset_ack_cmd));
		udp_socket_send_data(motor_reset_ack_cmd,sizeof(motor_reset_ack_cmd));	
	}
	//�����ͣ
	if(!memcmp(pmsg->data,motor_stop_cmd,sizeof(motor_stop_cmd)))
	{
		debug_print("�����ָͣ�� \r\n");
		motor_emergency_stop();
		
		//�����ͣ���Ӧ��ظ�
		debug_print("motor_stop_ack_cmd = %d \r\n ",sizeof(motor_stop_ack_cmd));
		udp_socket_send_data(motor_stop_ack_cmd,sizeof(motor_stop_ack_cmd));
	}
	//3�������ʱ�䲥��
	if(!memcmp(pmsg->data,motor3_run_cmd,6))
	{
		debug_print("���3��������ָ�� \r\n");
		memcpy(RunDataBuf,pmsg->data+20,12);
		//���ֹͣ����
		//��ȡ�����ǰλ��
		motorMsgSt = get_motor_current_state();
		for(i=0;i<MOTOR_NUM;i++)
		{
			motorMsgSt.queue[i].tagetPosition = *(uint32_t *)(RunDataBuf+i*4);
			motorMsgSt.queue[i].tagetPosition = FourBigToSmall(motorMsgSt.queue[i].tagetPosition);
			//motorMsgSt.queue[i].tagetPosition -= ZREO_PLUS;
		}
		//���������ڶ���(�ڶ����˶����������������в�һ�£��õ��������ڶ���)
		motorMsgSt.queue[3].tagetPosition = motorMsgSt.queue[1].tagetPosition;
		motor_relative_time_run(&motorMsgSt);
	}
	
	//�������ָ��
	//pmsg->data[1]�˶��᣻pmsg->data[2]�˶�����
	if(pmsg->data[0] == 0xAA &pmsg->data[1] != 0xFF)
	{
		switch(pmsg->data[1])
		{
			case 0:
				//MovToOrg(0,pmsg->data[1],pmsg->data[2],0,motorSpeed);//���������ٶȳ����˶�
				FH_ContinueMov(0,pmsg->data[1],pmsg->data[2],0,100,motorSpeed,1);
			break;
			
			case 1:
				FH_ContinueMov(0,pmsg->data[1],pmsg->data[2],0,100,motorSpeed,1);
			break;
			
			case 2:
				FH_ContinueMov(0,pmsg->data[1],pmsg->data[2],0,1000,motorSpeed,1);
			break;
			
			case 3:
				FH_ContinueMov(0,pmsg->data[1],pmsg->data[2],0,motorSpeed/2,motorSpeed,1);
			break;
		}
	} 
	else if(pmsg->data[0] == 0xAA &pmsg->data[1] == 0xFF)
	{//���е��ͳһ����
		debug_print("����ͳһ����ָ�� \r\n");
		if(pmsg->data[2] != 0xFF)
		{
			for(i=0;i<MOTOR_NUM;i++)
			{
				MovToOrg(0,i,pmsg->data[2],0,ZERO_SPEED);
			}
		} else {//���ȫ��ֹͣ�˶�
			for(i=0;i<MOTOR_NUM;i++)
			{
				AxsStop(0,i);
			}
		}
	}
	//����Ӽ���
	if(pmsg->data[0] == 0xA0 &pmsg->data[1] == 0)//����
	{
		motorSpeed += 100;
		debug_print("��������ٶ� = %d \n", motorSpeed);
	}
	if(pmsg->data[0] == 0xA0 &pmsg->data[1] == 1)//����
	{
		if(motorSpeed)
			motorSpeed -= 100;
		debug_print("��������ٶ� = %d \n", motorSpeed);
	}
	
	//���λ�ÿ���ģʽ �����϶���
	if(pmsg->data[0] == 0xBB)
	{
		switch(pmsg->data[1])//��Ʒ���
		{
			case 0://�����λ
				debug_print("���궯��������λ\n");
				for(i=0; i<MOTOR_NUM; i++)
				{
					Read_Position(0, i, &pMotorMsgSt.queue[i].position, &pMotorMsgSt.queue[i].run_state, &pMotorMsgSt.queue[i].Io_state, &SyncIO);
				}
				dir = 1;//�˶�����
				
//				DeltMov(0,X_MOTOR,0,dir,0,1000,LIMIT_SPEED,pMotorMsgSt.queue[X_MOTOR].position,0,DECELERATION_TIME,DECELERATION_TIME,0,0);//X�����˶�����
//				DeltMov(0,Y_MOTOR,0,dir,0,1000,LIMIT_SPEED,pMotorMsgSt.queue[Y_MOTOR].position,0,DECELERATION_TIME,DECELERATION_TIME,0,0);//y�����˶�����
//				DeltMov(0,Z_MOTOR,0,dir,0,1000,LIMIT_SPEED,pMotorMsgSt.queue[Z_MOTOR].position,0,DECELERATION_TIME,DECELERATION_TIME,0,0);//Z�����˶�����
				
				AXIS_Interpolation3(0,X_MOTOR,Y_MOTOR,Z_MOTOR,0,dir,dir,dir,0,1000,LIMIT_SPEED,pMotorMsgSt.queue[X_MOTOR].position,pMotorMsgSt.queue[Y_MOTOR].position,pMotorMsgSt.queue[Z_MOTOR].position,0,DECELERATION_TIME,DECELERATION_TIME,0,0);
				break;
			//��Ʒ����	
			case 1:
			case 2:
			case 3:
			case 4:
				debug_print("ʢ��Ʒ %d �����༭\n",pmsg->data[1]);
				//make_rice_move_run(pmsg->data[1]);
				//��������λ��
				switch(pmsg->data[1])
				{
					case 1:
						X_COORDINATE = X_COORDINATE_1;
						Y_COORDINATE = Y_COORDINATE_1;
						Z_COORDINATE = Z_COORDINATE_1;
					break;
					
					case 2:
						X_COORDINATE = X_COORDINATE_2;
						Y_COORDINATE = Y_COORDINATE_2;
						Z_COORDINATE = Z_COORDINATE_2;
					break;
					
					case 3:
						X_COORDINATE = X_COORDINATE_3;
						Y_COORDINATE = Y_COORDINATE_3;
						Z_COORDINATE = Z_COORDINATE_3;
					break;
					
					case 4:
						X_COORDINATE = X_COORDINATE_4;
						Y_COORDINATE = Y_COORDINATE_4;
						Z_COORDINATE = Z_COORDINATE_4;
					break;
				}
				//�������ȹ���У׼
				motor_position_zero();
				//��һ��������
				control_spoon_action(0); 
				debug_print("�����ſ�\n");
				//�����˶�������λ��
				dir = 0;//�˶�����
				AXIS_Interpolation3(0,X_MOTOR,Y_MOTOR,Z_MOTOR,0,dir,dir,dir,0,1000,LIMIT_SPEED,X_COORDINATE,Y_COORDINATE, Z_COORDINATE,0,DECELERATION_TIME,DECELERATION_TIME,0,0);
				//�ȴ��������ᵽ��ָ��λ��
				while(1)
				{
					for(i=0; i<MOTOR_NUM; i++)
					{
						Read_Position(0, i, &pMotorMsgSt.queue[i].position, &pMotorMsgSt.queue[i].run_state, &pMotorMsgSt.queue[i].Io_state, &SyncIO);
					}
//					if(pMotorMsgSt.queue[X_MOTOR].position >=X_COORDINATE &&  pMotorMsgSt.queue[Y_MOTOR].position >=Y_COORDINATE && pMotorMsgSt.queue[Z_MOTOR].position >=Z_COORDINATE)
//					{
//						break;
//					}	
					HAL_Delay(100);
					//�жϵ���Ƿ�ֹͣ�˶�״̬
					if((pMotorMsgSt.queue[X_MOTOR].run_state ==0) & (pMotorMsgSt.queue[Y_MOTOR].run_state == 0) & (pMotorMsgSt.queue[Z_MOTOR].run_state == 0))
					{
						break;
					}	
				}
				//get_motor_current_state();
				debug_print("������һ�����\n");
				//z������һ���������뵽������
				DeltMov(0,Z_MOTOR,1,dir,0,1000,LIMIT_SPEED,Z_DOWN_TRANCE,0,DECELERATION_TIME,DECELERATION_TIME,0,0);//Z�����˶�����
				//�ȴ��������ᵽ��ָ��λ��
				while(1)
				{
					for(i=0; i<MOTOR_NUM; i++)
					{
						Read_Position(0, i, &pMotorMsgSt.queue[i].position, &pMotorMsgSt.queue[i].run_state, &pMotorMsgSt.queue[i].Io_state, &SyncIO);
					}
					HAL_Delay(100);
					if(pMotorMsgSt.queue[Z_MOTOR].run_state == 0)
					{
						break;
					}
				}
				//����һ�߹رգ�Z����������
				dir = 1;//�˶�����
				control_spoon_action(1); 
				debug_print("���ӹر�\n");
				DeltMov(0,Z_MOTOR,1,dir,0,100,LIMIT_SPEED/3,Z_DOWN_TRANCE,0,DECELERATION_TIME,DECELERATION_TIME,0,0);//Z�����˶�����
				//�ȴ��������ᵽ��ָ��λ��
				while(1)
				{
					for(i=0; i<MOTOR_NUM; i++)
					{
						Read_Position(0, i, &pMotorMsgSt.queue[i].position, &pMotorMsgSt.queue[i].run_state, &pMotorMsgSt.queue[i].Io_state, &SyncIO);
					}
					HAL_Delay(100);
					//�жϵ���Ƿ�ֹͣ�˶�״̬
					if(pMotorMsgSt.queue[Z_MOTOR].run_state == 0)
					{
						break;
					}
				}
				//X Y��ص���ʼλ��
				for(i=0; i<MOTOR_NUM; i++)
				{
					Read_Position(0, i, &pMotorMsgSt.queue[i].position, &pMotorMsgSt.queue[i].run_state, &pMotorMsgSt.queue[i].Io_state, &SyncIO);
				}
				dir = 1;//�˶�����
				AXIS_Interpolation2(0,X_MOTOR,Y_MOTOR,0,dir,dir,0,1000,LIMIT_SPEED,pMotorMsgSt.queue[X_MOTOR].position,pMotorMsgSt.queue[Y_MOTOR].position,0,DECELERATION_TIME,DECELERATION_TIME,0,0);
				//�ȴ��������ᵽ��ָ��λ��
				while(1)
				{
					for(i=0; i<MOTOR_NUM; i++)
					{
						Read_Position(0, i, &pMotorMsgSt.queue[i].position, &pMotorMsgSt.queue[i].run_state, &pMotorMsgSt.queue[i].Io_state, &SyncIO);
					}
					debug_print("X Y ��ص�ԭ��");
					HAL_Delay(100);
					//�жϵ���Ƿ�ֹͣ�˶�״̬
					if(((pMotorMsgSt.queue[X_MOTOR].run_state ==0) & (pMotorMsgSt.queue[Y_MOTOR].run_state == 0)))
					{
						break;
					}	
					//HAL_Delay(1000);
				}
				//Z�����·Ų�
				dir = 0;//�˶�����
				DeltMov(0,Z_MOTOR,1,dir,0,1000,LIMIT_SPEED,35000,0,DECELERATION_TIME,DECELERATION_TIME,0,0);//Z�����˶�����
				//�ȴ��������ᵽ��ָ��λ��
				while(1)
				{
					for(i=0; i<MOTOR_NUM; i++)
					{
						Read_Position(0, i, &pMotorMsgSt.queue[i].position, &pMotorMsgSt.queue[i].run_state, &pMotorMsgSt.queue[i].Io_state, &SyncIO);
					}
					HAL_Delay(100);
					//�жϵ���Ƿ�ֹͣ�˶�״̬
					if(pMotorMsgSt.queue[Z_MOTOR].run_state == 0)
					{
						break;
					}
				}
				control_spoon_action(0); 
				debug_print("�����ſ�\n");

				//����˺�Z��ص���λ
				HAL_Delay(3000);
				debug_print("���ӹر�\n");
				control_spoon_action(1); 
				dir = 1;//�˶�����
				DeltMov(0,Z_MOTOR,1,dir,0,1000,LIMIT_SPEED,Z_COORDINATE+35000,0,DECELERATION_TIME,DECELERATION_TIME,0,0);//Z�����˶�����
				//�ȴ��������ᵽ��ָ��λ��
				while(1)
				{
					for(i=0; i<MOTOR_NUM; i++)
					{
						Read_Position(0, i, &pMotorMsgSt.queue[i].position, &pMotorMsgSt.queue[i].run_state, &pMotorMsgSt.queue[i].Io_state, &SyncIO);
					}
					HAL_Delay(100);
					//�жϵ���Ƿ�ֹͣ�˶�״̬
					if(pMotorMsgSt.queue[Z_MOTOR].run_state == 0)
					{
						break;
					}
				}
			break;
			
			case 5://���ӹر�
				control_spoon_action(0);
				debug_print("���ӹر�\n");
			break;
			
			case 6://�����ſ�
				control_spoon_action(1); 
				debug_print("�����ſ�\n");
			break;
		}
	}
}

/*==================================================================================
* �� �� ���� udp_instance_init
* ��    ���� None
* ��������:  udp��ʼ��
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-10-28 170617
=================================================================================*/
void udp_instance_init(void)
{
	msg_queue_init(&udp_queue);
}

/*==================================================================================
* �� �� ���� udp_push_data_msg
* ��    ���� None
* ��������:  udp ���յ����ݺ���ô˺����ѽ��յ�������ӵĶ�������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-09-29 170658
==================================================================================*/
void udp_push_data_msg(void* ret_msg)
{
	_pUdp_Msg pmsg = ret_msg;  
	
	msg_queue_push(&udp_queue, pmsg);
}

/*==================================================================================
* �� �� ���� udp_pop_one_frame
* ��    ���� _pRet_Msg
* ��������:  udp���ݽ���
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� lc
* ����ʱ�䣺 2021-06-20 154449
==================================================================================*/ 
static uint8_t udp_pop_one_frame(void )
{
	int num;
	p_send_queue_t p_queue_buff = &udp_queue;
	
	//�����������Ƿ�������
	num = msg_queue_num(p_queue_buff);
	if(num)
	{
		for(int i=0; i<MAX_CACHE_NUM; i++)
		{
			if(p_queue_buff->queue[i].byte_count)
			{
				//debug_print("�����������Ϣ =%s \n", p_queue_buff->queue[i].data);
				debug_print_hex(p_queue_buff->queue[i].data,10);
				
				udp_frame_parse(&(p_queue_buff->queue[i]));
				msg_queue_pop(p_queue_buff, 0);
				//return 0;
			}	
		}
		//debug_print("�����������Ϣ���� =%d \n",num);
	}
}

/*==================================================================================
* �� �� ���� udp_rec_decode_task
* ��    ���� None
* ��������:  udp �������ݽ�������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-09-29 170658
==================================================================================*/
void udp_rec_decode_task(void* argv)
{
	udp_pop_one_frame();
}
