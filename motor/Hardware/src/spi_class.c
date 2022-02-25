//#include "includes.h"
//#include "spi_class.h"


//_Context mSpi_Context[] = 
//{
//	{
//	  .spi = (void*)SPI1,
//	  .cs_gpio = CS_FLASH_GPIO_Port,
//	  .cs_pin  = CS_FLASH_Pin,
//	},

//	{
//	  .spi = (void*)SPI3,
//	  .cs_gpio = SPI3_NSS1_GPIO_Port,
//	  .cs_pin  = SPI3_NSS1_Pin,
//	},
//	{
//	  .spi = (void*)SPI3,
//	  .cs_gpio = SPI3_NSS2_GPIO_Port,
//	  .cs_pin  = SPI3_NSS2_Pin,
//	}
//};

//static uint8_t cs_gpio_set(void* context, uint8_t level);
//static uint8_t sed_rev_byte(void* context, uint8_t value);
//static uint8_t sed_rev_buff(void* context, uint8_t *s_buff, uint8_t *r_buff, uint16_t len);

//_Spi_Class mSpi_Class[] = 
//{
//	{
//	  .pContext   = &mSpi_Context[0],
//	  .cs_set	  	= cs_gpio_set,
//	  .sed_rev_byte = sed_rev_byte,
//	  .sed_rev_buf  = sed_rev_buff,
//	},
//	{
//	  .pContext   = &mSpi_Context[1],
//	  .cs_set	  	= cs_gpio_set,
//	  .sed_rev_byte = sed_rev_byte,
//	  .sed_rev_buf  = sed_rev_buff,
//	},
//	{
//	  .pContext   = &mSpi_Context[2],
//	  .cs_set	  	= cs_gpio_set,
//	  .sed_rev_byte = sed_rev_byte,
//	  .sed_rev_buf  = sed_rev_buff,
//	}
//};

///*==================================================================================
//* 函 数 名： spi1_sed_rev
//* 参    数： None
//* 功能描述:  SPI1接口发送数据
//* 返 回 值： None
//* 备    注： 
//* 作    者： xiaozh
//* 创建时间： 2019-09-05 143321
//==================================================================================*/
//static uint8_t sed_rev_byte(void* context, uint8_t value)
//{
//	_pContext pthis = (_pContext)context;
//	
//	uint8_t ret = 0; 
//	while((((pSPI_TYPE)pthis->spi)->SR & SPI_FLAG_TXE) == RESET);	//等待发送缓冲区空

//	((pSPI_TYPE)pthis->spi)->DR = value; 	//发送一个字节  

//	while((((pSPI_TYPE)pthis->spi)->SR & SPI_FLAG_RXNE) == RESET);	 //等待是否接收到一个字节  
//	
//	ret = ((((pSPI_TYPE)pthis->spi)->DR)); 													//获得该字节

//	return ret; //返回收到的字节
//}

///*==================================================================================
//* 函 数 名： sed_rev_buff
//* 参    数： None
//* 功能描述:  SPI发送数组
//* 返 回 值： None
//* 备    注： 
//* 作    者： xiaozh
//* 创建时间： 2019-09-05 190852
//==================================================================================*/
//static uint8_t sed_rev_buff(void* context, uint8_t *s_buff, uint8_t *r_buff, uint16_t len)
//{
//	for(int i=0; i<len; i++)
//	{
//		r_buff[i] = sed_rev_byte(context, s_buff[i]);
//	}
//	
//	return 0x01;
//}

///*==================================================================================
//* 函 数 名： cs_gpio_set
//* 参    数： None
//* 功能描述:  设置cs引脚电平
//* 返 回 值： None
//* 备    注： 
//* 作    者： xiaozh
//* 创建时间： 2019-09-05 190852
//==================================================================================*/
//static uint8_t cs_gpio_set(void* context, uint8_t level)
//{
//	_pContext pthis = (_pContext)context;

//	HAL_GPIO_WritePin(((pGPIO_TYPE)pthis->cs_gpio), pthis->cs_pin, (GPIO_PinState)level);

//	return 0x01;
//}
