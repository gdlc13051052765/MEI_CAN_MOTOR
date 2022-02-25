/*
**************************************************************************************************
* @file    		w5500_conf.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief  		配置MCU，移植W5500程序需要修改的文件，配置W5500的MAC和IP地址
**************************************************************************************************
*/
#include <stdio.h> 
#include <string.h>

#include "w5500_conf.h"
#include "utility.h"
#include "w5500.h"
//#include "dhcp.h"
#include "bsp_GeneralTIM.h"
//#include "i2c/bsp_EEPROM.h"

CONFIG_MSG  ConfigMsg;																	/*配置结构体*/
EEPROM_MSG_STR EEPROM_MSG;															/*EEPROM存储信息结构体*/
SPI_HandleTypeDef hspi_w5500;

/*定义MAC地址,如果多块W5500网络适配板在同一现场工作，请使用不同的MAC地址*/
uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};

/*定义默认IP信息*/
uint8 local_ip[4]  ={192,168,3,100};										/*定义W5500默认IP地址*/
uint8 subnet[4]    ={255,255,255,0};										/*定义W5500默认子网掩码*/
uint8 gateway[4]   ={192,168,3,1};											/*定义W5500默认网关*/
uint8 dns_server[4]={114,114,114,114};									/*定义W5500默认DNS*/

uint16 local_port=5001;	                       					/*定义本地端口*/

/*定义远端IP信息*/
uint8  remote_ip[4]={192,168,3,28};			  							/*远端IP地址*/
uint16 remote_port=8080;																/*远端端口号*/

/*IP配置方法选择，请自行选择*/
uint8	ip_from=IP_FROM_DEFINE;				

uint8         dhcp_ok   = 0;													   			/*dhcp成功获取IP*/
uint32	      ms        = 0;															  	/*毫秒计数*/
uint32	      dhcp_time = 0;															  	/*DHCP运行计数*/
__IO uint8_t  ntptimer  = 0;															  	/*NPT秒计数*/

/**
*@brief		硬件复位W5500
*@param		无
*@return	无
*/
void reset_w5500(void)
{
  HAL_GPIO_WritePin(WIZ_RESET_PORT, WIZ_RESET_PIN,GPIO_PIN_RESET);
  delay_ms(10);  
  HAL_GPIO_WritePin(WIZ_RESET_PORT, WIZ_RESET_PIN,GPIO_PIN_SET);
  delay_ms(1600);
}

/**
*@brief		配置W5500的IP地址
*@param		无
*@return	无
*/
void set_w5500_ip(void)
{		
  /*复制定义的配置信息到配置结构体*/
	memcpy(ConfigMsg.mac, mac, 6);
	memcpy(ConfigMsg.lip,local_ip,4);
	memcpy(ConfigMsg.sub,subnet,4);
	memcpy(ConfigMsg.gw,gateway,4);
	memcpy(ConfigMsg.dns,dns_server,4);
	if(ip_from==IP_FROM_DEFINE)	
		printf(" 使用定义的IP信息配置W5500\r\n");
	
	/*使用EEPROM存储的IP参数*/	
	if(ip_from==IP_FROM_EEPROM)
	{
		/*从EEPROM中读取IP配置信息*/
		read_config_from_eeprom();		
		
		/*如果读取EEPROM中MAC信息,如果已配置，则可使用*/		
		if( *(EEPROM_MSG.mac)==0x00&& *(EEPROM_MSG.mac+1)==0x08&&*(EEPROM_MSG.mac+2)==0xdc)		
		{
			printf(" IP from EEPROM\r\n");
			/*复制EEPROM配置信息到配置的结构体变量*/
			memcpy(ConfigMsg.lip,EEPROM_MSG.lip, 4);				
			memcpy(ConfigMsg.sub,EEPROM_MSG.sub, 4);
			memcpy(ConfigMsg.gw, EEPROM_MSG.gw, 4);
		}
		else
		{
			printf(" EEPROM未配置,使用定义的IP信息配置W5500,并写入EEPROM\r\n");
			write_config_to_eeprom();	/*使用默认的IP信息，并初始化EEPROM中数据*/
		}			
	}


	/*以下配置信息，根据需要选用*/	
	ConfigMsg.sw_ver[0]=FW_VER_HIGH;
	ConfigMsg.sw_ver[1]=FW_VER_LOW;	

	/*将IP配置信息写入W5500相应寄存器*/	
	setSUBR(ConfigMsg.sub);
	setGAR(ConfigMsg.gw);
	setSIPR(ConfigMsg.lip);
	
	getSIPR (local_ip);			
	printf(" W5500 IP地址   : %d.%d.%d.%d\r\n", local_ip[0],local_ip[1],local_ip[2],local_ip[3]);
	getSUBR(subnet);
	printf(" W5500 子网掩码 : %d.%d.%d.%d\r\n", subnet[0],subnet[1],subnet[2],subnet[3]);
	getGAR(gateway);
	printf(" W5500 网关     : %d.%d.%d.%d\r\n", gateway[0],gateway[1],gateway[2],gateway[3]);
}

/**
*@brief		配置W5500的MAC地址
*@param		无
*@return	无
*/
void set_w5500_mac(void)
{
	memcpy(ConfigMsg.mac, mac, 6);
	setSHAR(ConfigMsg.mac);	/**/
	//memcpy(DHCP_GET.mac, mac, 6);
}

/**
*@brief		配置W5500的GPIO接口
*@param		无
*@return	无
*/
void gpio_for_w5500_config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  WIZ_SPIx_RCC_CLK_ENABLE();
  WIZ_SPI_GPIO_ClK_ENABLE();
  WIZ_SPIx_SCS_CLK_ENABLE();
	WIZ_INT_CLK_ENABLE();
  WIZ_RESET_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = WIZ_SPIx_SCLK_PIN|WIZ_SPIx_MOSI_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(WIZ_SPIx_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = WIZ_SPIx_MISO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(WIZ_SPIx_GPIO_PORT, &GPIO_InitStruct);
  
  HAL_GPIO_WritePin(WIZ_SPIx_SCS_PORT, WIZ_SPIx_SCS_PIN, GPIO_PIN_SET);
  GPIO_InitStruct.Pin = WIZ_SPIx_SCS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(WIZ_SPIx_SCS_PORT, &GPIO_InitStruct);

  /* Disable the Serial Wire Jtag Debug Port SWJ-DP */
  __HAL_AFIO_REMAP_SWJ_DISABLE();

  hspi_w5500.Instance = WIZ_SPIx;
  hspi_w5500.Init.Mode = SPI_MODE_MASTER;
  hspi_w5500.Init.Direction = SPI_DIRECTION_2LINES;
  hspi_w5500.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi_w5500.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi_w5500.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi_w5500.Init.NSS = SPI_NSS_SOFT;
  hspi_w5500.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi_w5500.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi_w5500.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi_w5500.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi_w5500.Init.CRCPolynomial = 7;
  HAL_SPI_Init(&hspi_w5500);
	
  /*定义INT引脚*/	
  GPIO_InitStruct.Pin = WIZ_INT_PIN;                  
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(WIZ_SPIx_SCS_PORT, &GPIO_InitStruct);

  /*定义RESET引脚*/
  HAL_GPIO_WritePin(WIZ_RESET_PORT, WIZ_RESET_PIN, GPIO_PIN_SET);
  GPIO_InitStruct.Pin = WIZ_RESET_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(WIZ_RESET_PORT, &GPIO_InitStruct);
}

/**
*@brief		W5500片选信号设置函数
*@param		val: 为“0”表示片选端口为低，为“1”表示片选端口为高
*@return	无
*/
void wiz_cs(uint8_t val)
{
	if (val == LOW) 
	{
	  HAL_GPIO_WritePin(WIZ_SPIx_SCS_PORT, WIZ_SPIx_SCS_PIN, GPIO_PIN_RESET);
	}
	else if (val == HIGH)
	{
	  HAL_GPIO_WritePin(WIZ_SPIx_SCS_PORT, WIZ_SPIx_SCS_PIN, GPIO_PIN_SET);
	}
}

/**
*@brief		设置W5500的片选端口SCSn为低
*@param		无
*@return	无
*/
void iinchip_csoff(void)
{
	wiz_cs(LOW);
}

/**
*@brief		设置W5500的片选端口SCSn为高
*@param		无
*@return	无
*/
void iinchip_cson(void)
{	
   wiz_cs(HIGH);
}

uint8_t SPI_SendByte(uint8_t byte)
{
  uint8_t d_read,d_send=byte;
  if(HAL_SPI_TransmitReceive(&hspi_w5500,&d_send,&d_read,1,0xFFFFFF)!=HAL_OK)
    d_read=0XFF;
  
  return d_read; 
}

/**
*@brief		STM32 SPI1读写8位数据
*@param		dat：写入的8位数据
*@return	无
*/
uint8  IINCHIP_SpiSendData(uint8 dat)
{
   return(SPI_SendByte(dat));
}

/**
*@brief		写入一个8位数据到W5500
*@param		addrbsb: 写入数据的地址
*@param   data：写入的8位数据
*@return	无
*/
void IINCHIP_WRITE( uint32 addrbsb,  uint8 data)
{
	iinchip_csoff();                              		
	IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);	
	IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
	IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4);  
	IINCHIP_SpiSendData(data);                   
	iinchip_cson();                            
}

/**
*@brief		从W5500读出一个8位数据
*@param		addrbsb: 写入数据的地址
*@param   data：从写入的地址处读取到的8位数据
*@return	无
*/
uint8 IINCHIP_READ(uint32 addrbsb)
{
	uint8 data = 0;
	iinchip_csoff();                            
	IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
	IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
	IINCHIP_SpiSendData( (addrbsb & 0x000000F8))    ;
	data = IINCHIP_SpiSendData(0x00);            
	iinchip_cson();                               
	return data;    
}

/**
*@brief		向W5500写入len字节数据
*@param		addrbsb: 写入数据的地址
*@param   buf：写入字符串
*@param   len：字符串长度
*@return	len：返回字符串长度
*/
uint16 wiz_write_buf(uint32 addrbsb,uint8* buf,uint16 len)
{
	uint16 idx = 0;
	if(len == 0) printf("Unexpected2 length 0\r\n");
	iinchip_csoff();                               
	IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
	IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
	IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4); 
	for(idx = 0; idx < len; idx++)
	{
	 IINCHIP_SpiSendData(buf[idx]);
	}
	iinchip_cson();                           
	return len;  
}

/**
*@brief		从W5500读出len字节数据
*@param		addrbsb: 读取数据的地址
*@param 	buf：存放读取数据
*@param		len：字符串长度
*@return	len：返回字符串长度
*/
uint16 wiz_read_buf(uint32 addrbsb, uint8* buf,uint16 len)
{
	uint16 idx = 0;
	if(len == 0)
	{
		printf("Unexpected2 length 0\r\n");
	}
	iinchip_csoff();                                
	IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
	IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
	IINCHIP_SpiSendData( (addrbsb & 0x000000F8));    
	for(idx = 0; idx < len; idx++)                   
	{
		buf[idx] = IINCHIP_SpiSendData(0x00);
	}
	iinchip_cson();                                  
	return len;
}

/**
*@brief		写配置信息到EEPROM
*@param		无
*@return	无
*/
void write_config_to_eeprom(void)
{
//	uint16 dAddr=0;
//	EEPROM_WriteBytes(ConfigMsg.mac,dAddr,(uint8)EEPROM_MSG_LEN);				
//	delay_ms(10);																							
}

/**
*@brief		从EEPROM读配置信息
*@param		无
*@return	无
*/
void read_config_from_eeprom(void)
{
//  EEPROM_CheckOk();
//	EEPROM_ReadBytes(EEPROM_MSG.mac,0,EEPROM_MSG_LEN);
//	delay_us(10);
}

/**
*@brief		STM32定时器2初始化
*@param		无
*@return	无
*/
void timer2_init(void)
{
	GENERAL_TIMx_Init();		/* TIM2开始计时 */
  /* 在中断模式下启动定时器 */
  HAL_TIM_Base_Start_IT(&htimx);
}

/**
  * 函数功能: 非阻塞模式下定时器的回调函数
  * 输入参数: htim：定时器句柄
  * 返 回 值: 无
  * 说    明: 无
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  timer2_isr();
}

/**
*@brief		dhcp用到的定时器初始化
*@param		无
*@return	无
*/
void dhcp_timer_init(void)
{
  timer2_init();																	
}

/**
*@brief		ntp用到的定时器初始化
*@param		无
*@return	无
*/
void ntp_timer_init(void)
{
  timer2_init();																	
}

/**
*@brief		定时器2中断函数
*@param		无
*@return	无
*/
void timer2_isr(void)
{
  ms++;	
  if(ms>=1000)
  {  
    ms=0;
    dhcp_time++;																					/*DHCP定时加1S*/
	  #ifndef	__NTP_H__
	  ntptimer++;																						/*NTP重试时间加1S*/
	  #endif
  }
}
/**
*@brief		STM32系统软复位函数
*@param		无
*@return	无
*/
void reboot(void)
{
  pFunction Jump_To_Application;
  uint32 JumpAddress;
  printf(" 系统重启中……\r\n");
  JumpAddress = *(__IO uint32_t*) (0x00000004);
  Jump_To_Application = (pFunction) JumpAddress;
  Jump_To_Application();
}
