
#include <stdio.h>
#include <string.h>
#include "w5500_conf.h"
#include "w5500.h"
#include "socket.h"
#include "utility.h"
#include "udp_socket.h"
#include "udp_fifo.h"
#include "command.h"

/**
*@brief		UDP�շ����Գ���
*@param		��
*@return	��
*/
static void do_udp_test(void)
{                                                              
	uint16 len=0;
	uint8 buff[512];                                                          /*����һ��2KB�Ļ���*/	
	switch(getSn_SR(SOCK_UDPS))                                                /*��ȡsocket��״̬*/
	{
		case SOCK_CLOSED:                                                        /*socket���ڹر�״̬*/
			socket(SOCK_UDPS,Sn_MR_UDP,local_port,0);                              /*��ʼ��socket*/
		break;

		case SOCK_UDP:                                                           /*socket��ʼ�����*/
			if(getSn_IR(SOCK_UDPS) & Sn_IR_RECV)
			{
				setSn_IR(SOCK_UDPS, Sn_IR_RECV);                                     /*������ж�*/
			}
			if((len=getSn_RX_RSR(SOCK_UDPS))>0)                                    /*���յ�����*/
			{
				recvfrom(SOCK_UDPS,buff, len, remote_ip,&remote_port);               /*W5500���ռ����������������*/
				buff[len-8]=0x00;                                                    /*����ַ���������*/
				printf("%s\r\n",buff);                                               /*��ӡ���ջ���*/ 
				sendto(SOCK_UDPS,buff,len-8, remote_ip, remote_port);                /*W5500�ѽ��յ������ݷ��͸�Remote*/
			}
		break;
	}
}

/**
*@brief		UDP�շ����Գ���
*@param		��
*@return	��
*/
static void udp_socket_rec_data(void)
{                                                              
	uint16 len=0;
	uint8 buff[512]={0};  

	_Udp_Msg pmsg ;
                                                      /*����һ��2KB�Ļ���*/	
	switch(getSn_SR(SOCK_UDPS))                                                /*��ȡsocket��״̬*/
	{
		case SOCK_CLOSED:                                                        /*socket���ڹر�״̬*/
			socket(SOCK_UDPS,Sn_MR_UDP,local_port,0);                              /*��ʼ��socket*/
		break;

		case SOCK_UDP:                                                           /*socket��ʼ�����*/
			if(getSn_IR(SOCK_UDPS) & Sn_IR_RECV)
			{
				setSn_IR(SOCK_UDPS, Sn_IR_RECV);                                     /*������ж�*/
			}
			if((len=getSn_RX_RSR(SOCK_UDPS))>0)                                    /*���յ�����*/
			{
				recvfrom(SOCK_UDPS,buff, len, remote_ip,&remote_port);               /*W5500���ռ����������������*/
				pmsg.byte_count = len-8;//�������ݳ���
				memcpy(pmsg.data,buff,len-8);
				udp_push_data_msg(&pmsg);//���յ�������ӵ���������
				//printf("len = %d ,%s\r\n",len,pmsg.data);                                               /*��ӡ���ջ���*/ 
				sendto(SOCK_UDPS,buff,len-8, remote_ip, 9000);                /*W5500�ѽ��յ������ݷ��͸�Remote*/
			}
		break;
	}
}

/*==================================================================================
* �� �� ���� udp_socket_send_data
* ��    ���� None
* ��������:  udp ��������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-09-29 170658
==================================================================================*/
void udp_socket_send_data(uint8_t *data,uint16_t len)
{
	sendto(SOCK_UDPS,data,len, remote_ip, remote_port);  
}
/*==================================================================================
* �� �� ���� udp_socket_task
* ��    ���� None
* ��������:  udp socket task
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-09-29 170658
==================================================================================*/
void udp_socket_task(void* argv)
{
	udp_socket_rec_data();
}
