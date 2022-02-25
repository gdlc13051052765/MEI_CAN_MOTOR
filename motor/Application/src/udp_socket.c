
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
*@brief		UDP收发测试程序
*@param		无
*@return	无
*/
static void do_udp_test(void)
{                                                              
	uint16 len=0;
	uint8 buff[512];                                                          /*定义一个2KB的缓存*/	
	switch(getSn_SR(SOCK_UDPS))                                                /*获取socket的状态*/
	{
		case SOCK_CLOSED:                                                        /*socket处于关闭状态*/
			socket(SOCK_UDPS,Sn_MR_UDP,local_port,0);                              /*初始化socket*/
		break;

		case SOCK_UDP:                                                           /*socket初始化完成*/
			if(getSn_IR(SOCK_UDPS) & Sn_IR_RECV)
			{
				setSn_IR(SOCK_UDPS, Sn_IR_RECV);                                     /*清接收中断*/
			}
			if((len=getSn_RX_RSR(SOCK_UDPS))>0)                                    /*接收到数据*/
			{
				recvfrom(SOCK_UDPS,buff, len, remote_ip,&remote_port);               /*W5500接收计算机发送来的数据*/
				buff[len-8]=0x00;                                                    /*添加字符串结束符*/
				printf("%s\r\n",buff);                                               /*打印接收缓存*/ 
				sendto(SOCK_UDPS,buff,len-8, remote_ip, remote_port);                /*W5500把接收到的数据发送给Remote*/
			}
		break;
	}
}

/**
*@brief		UDP收发测试程序
*@param		无
*@return	无
*/
static void udp_socket_rec_data(void)
{                                                              
	uint16 len=0;
	uint8 buff[512]={0};  

	_Udp_Msg pmsg ;
                                                      /*定义一个2KB的缓存*/	
	switch(getSn_SR(SOCK_UDPS))                                                /*获取socket的状态*/
	{
		case SOCK_CLOSED:                                                        /*socket处于关闭状态*/
			socket(SOCK_UDPS,Sn_MR_UDP,local_port,0);                              /*初始化socket*/
		break;

		case SOCK_UDP:                                                           /*socket初始化完成*/
			if(getSn_IR(SOCK_UDPS) & Sn_IR_RECV)
			{
				setSn_IR(SOCK_UDPS, Sn_IR_RECV);                                     /*清接收中断*/
			}
			if((len=getSn_RX_RSR(SOCK_UDPS))>0)                                    /*接收到数据*/
			{
				recvfrom(SOCK_UDPS,buff, len, remote_ip,&remote_port);               /*W5500接收计算机发送来的数据*/
				pmsg.byte_count = len-8;//接收数据长度
				memcpy(pmsg.data,buff,len-8);
				udp_push_data_msg(&pmsg);//接收的数据添加到队列里面
				//printf("len = %d ,%s\r\n",len,pmsg.data);                                               /*打印接收缓存*/ 
				sendto(SOCK_UDPS,buff,len-8, remote_ip, 9000);                /*W5500把接收到的数据发送给Remote*/
			}
		break;
	}
}

/*==================================================================================
* 函 数 名： udp_socket_send_data
* 参    数： None
* 功能描述:  udp 发送数据
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-09-29 170658
==================================================================================*/
void udp_socket_send_data(uint8_t *data,uint16_t len)
{
	sendto(SOCK_UDPS,data,len, remote_ip, remote_port);  
}
/*==================================================================================
* 函 数 名： udp_socket_task
* 参    数： None
* 功能描述:  udp socket task
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-09-29 170658
==================================================================================*/
void udp_socket_task(void* argv)
{
	udp_socket_rec_data();
}
