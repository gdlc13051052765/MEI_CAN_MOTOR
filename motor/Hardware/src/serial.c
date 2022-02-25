#include "serial.h"
#include "main.h"


static uint8_t rev_buff[1028] = {0};	//接收缓存
static uint8_t sed_buff[256] = {0};		//发送缓存

/*==================================================================================
* 函 数 名： serial_init
* 参    数： None
* 功能描述:  串口初始化
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-09-17 162343
==================================================================================*/ 
void serial_register_init(void *instance, void* pdev)
{ 
	_pSerial_Info pthis = (_pSerial_Info)instance;
	
	pthis->p_ffunc = pSerial_Fifo_Func;
	pthis->pDev = pdev;
	pthis->p_rev_fifo = pthis->p_ffunc->init_m(rev_buff, sizeof(rev_buff));
	pthis->p_sed_fifo = pthis->p_ffunc->init_m(sed_buff, sizeof(sed_buff)); 
}

/*==================================================================================
* 函 数 名： serial_sed_buff
* 参    数： None
* 功能描述:  串口发送数组
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-09-17 162343
==================================================================================*/ 
void serial_sed_byte(void *instance, uint8_t value)
{
	_pSerial_Info pthis = (_pSerial_Info)instance;
	USART_TypeDef * serial = 	(USART_TypeDef *) pthis->pDev;
	 
	serial->DR = value;
	while((serial->SR & UART_IT_TXE) == RESET);
//		USART1->DR = value;
//	while((USART1->SR & UART_IT_TXE) == RESET);
}

/*==================================================================================
* 函 数 名： serial_sed_buff
* 参    数： None
* 功能描述:  串口发送数组
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-09-17 162343
==================================================================================*/ 
void serial_sed_buff(void *instance, uint8_t *buff, uint16_t len)
{
	for(int i=0; i<len; i++)
	{
		serial_sed_byte(instance, buff[i]);
	}
}

/*==================================================================================
* 函 数 名： serial_sed_buff
* 参    数： None
* 功能描述:  串口发送数组
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-09-17 162343
==================================================================================*/ 
uint8_t serial_push_byte(void *instance, uint8_t value)
{
	_pSerial_Info pthis = (_pSerial_Info)instance;
	
	return pthis->p_ffunc->push(pthis->p_rev_fifo, &value);
}
