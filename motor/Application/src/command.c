#include "command.h"
#include "includes.h"
#include "struct_type.h"
#include "motor_control.h"
#include "udp_socket.h"
#include "hmc80x5_api.h"




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

//16��������ת�ַ���
void HexGroupToHexString(uint8_t *data,uint8_t *dst,uint8_t len)
{
	uint8_t i;
	uint8_t str[250];
	//uchar *dst;
	for(i=0;i<len;i++)
	{
		 str[2*i] = data[i]>>4;
     str[2*i+1] = data[i]&0xf;
	}
	for(i=0;i<len*2;i++)
	{
		sprintf(&dst[i],"%X",str[i]);
	}
}
//16�����ַ���ת16��������
void HexStringToHexGroup(char* source, uint8_t* dest, int sourceLen)
{
    int i;  
    uint8_t highByte, lowByte;  
      
    for (i = 0; i < sourceLen; i += 2)  
    {  
			highByte = source[i];  
			lowByte  = source[i + 1];  
  
      if (highByte >= 'A' && highByte <= 'F')
			{
				highByte -= 0x37;
			}
			else if(highByte >= 'a' && highByte <= 'f')
			{
				highByte -= 0x57;
			}
			else 
			{
				highByte -= '0';  
			}
			
			if (lowByte >= 'A' && lowByte <= 'F')
			{
				lowByte -= 0x37;
			}
			else if(lowByte >= 'a' && lowByte <= 'f')
			{
				lowByte -= 0x57;
			}
			else 
			{
				lowByte -= '0';  
			}
		
      dest[i / 2] = (highByte << 4) | lowByte;  
    }  
    return ;  
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
	//6�������ʱ�䲥��
	if(!memcmp(pmsg->data,motor4_run_cmd,8))
	{
	
	}	
	
	//�������ָ��
	//pmsg->data[1]�˶��᣻pmsg->data[2]�˶�����
	if(pmsg->data[0] == 0xAA &pmsg->data[1] != 0xFF)
	{
		for(i=0;i<MOTOR_NUM ;i++)
		{
			Set_Axs(0,i,0,0,0,0);//�Ƚ���ʹ��
		}
		for(i=0;i<MOTOR_NUM ;i++)
		{
			Set_Axs(0,i,1,1,1,1);//������ʹ�ܣ��߼�λ�û�0
		}
		debug_print("������������ָ�� \r\n");
		
		//MovToOrg(0,pmsg->data[1],pmsg->data[2],0,motorSpeed);//���������ٶȳ����˶�
		switch(pmsg->data[1])
		{
			case 0:
				//MovToOrg(0,pmsg->data[1],pmsg->data[2],0,motorSpeed);//���������ٶȳ����˶�
				FH_ContinueMov(0,pmsg->data[1],pmsg->data[2],0,100,motorSpeed,1);
			break;
			case 1:
				//MovToOrg(0,pmsg->data[1],pmsg->data[2],0,motorSpeed);//���������ٶȳ����˶�
				FH_ContinueMov(0,pmsg->data[1],pmsg->data[2],0,100,motorSpeed,1);
			break;
			case 3:
				//MovToOrg(0,pmsg->data[1],pmsg->data[2],0,7000);//���������ٶȳ����˶�
				FH_ContinueMov(0,pmsg->data[1],pmsg->data[2],0,motorSpeed/2,motorSpeed,1);
			break;
			case 2:
				FH_ContinueMov(0,pmsg->data[1],pmsg->data[2],0,1000,motorSpeed,1);
			break;
		}
	} 
	else if(pmsg->data[0] == 0xAA &pmsg->data[1] == 0xFF)
	{//���е��ͳһ����
		debug_print("����ͳһ����ָ�� \r\n");
		for(i=0;i<MOTOR_NUM ;i++)
		{
			Set_Axs(0,i,0,0,0,0);//�Ƚ���ʹ��
		}
		for(i=0;i<MOTOR_NUM ;i++)
		{
			Set_Axs(0,i,1,1,1,1);//������ʹ�ܣ��߼�λ�û�0
		}
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
	
	//���λ�ÿ���ģʽ 
	//Y��λ�Ƴ���170000����
	if(pmsg->data[0] == 0xBB)
	{
		switch(pmsg->data[1])//��Ʒ���
		{
			case 0://�����λ
				for(i=0; i<MOTOR_NUM; i++)
				{
					Read_Position(0, i, &pMotorMsgSt.queue[i].position, &pMotorMsgSt.queue[i].run_state, &pMotorMsgSt.queue[i].Io_state, &SyncIO);
				}
				DeltMov(0,0,0,1,0,1000,LIMIT_SPEED,pMotorMsgSt.queue[0].position,0,1000,1000,0,0);//X�����˶�����
				DeltMov(0,2,0,1,0,1000,LIMIT_SPEED,pMotorMsgSt.queue[2].position,0,1000,1000,0,0);//y�����˶�����
				DeltMov(0,3,0,1,0,1000,LIMIT_SPEED,pMotorMsgSt.queue[3].position,0,1000,1000,0,0);//Z�����˶�����
			break;
			case 1:
				debug_print("ʢ��Ʒ1 �����༭\n");

				DeltMov(0,0,1,0,0,1000,LIMIT_SPEED,40000,0,1000,1000,0,0);//X�����˶�����
				DeltMov(0,2,1,0,0,1000,LIMIT_SPEED,65000,0,1000,1000,0,0);//y�����˶�����
				DeltMov(0,3,1,0,0,1000,LIMIT_SPEED,65000,0,1000,1000,0,0);//Z�����˶�����
//				AXIS_Interpolation2(0,1,2,1,0,0,0,1000,LIMIT_SPEED,70000,70000,0,100,100,0,0);
			break;
			
			case 2:
				debug_print("ʢ��Ʒ2 �����༭\n");
			
			break;
			
			case 3:
			break;
			
			case 4:
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
