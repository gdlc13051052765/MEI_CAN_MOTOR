#ifndef  __SPI_CLASS_H
#define  __SPI_CLASS_H
#include <stdint.h>
#include "includes.h"
 
#define pSPI_TYPE				SPI_TypeDef *	
#define pGPIO_TYPE				GPIO_TypeDef *	

#define mSPI1_CS_LOW()				do{}while(0)
#define mSPI1_CS_HIGH()				do{}while(0)
	
#define mSPI2_CS_LOW()				do{}while(0)
#define mSPI2_CS_HIGH()				do{}while(0)

#define mSPI1_DSN_LOW()				do{}while(0)
#define mSPI1_DSN_LOW()				do{}while(0)
	
#define mSPI2_DSN_LOW()				do{}while(0)
#define mSPI2_DSN_LOW()				do{}while(0)


#define SPI_CS_NUM(type, level)  	do{(level==0)?(type##_CS_LOW()):(type##_CS_HIGH())}while(0)
#define SPI_DSN_NUM(type, level)  	do{(level==0)?(type##_CS_LOW()):(type##_CS_HIGH())}while(0)
#define SPI_SED_REV(type)  			do{(level==0)?(type##_CS_LOW()):(type##_CS_HIGH())}while(0)


typedef struct
{
	uint8_t (*cs_set)(void*, uint8_t);					//pipe   level
	uint8_t (*sed_rev_byte)(void*, uint8_t);			//发送接收一个字节
	uint8_t (*sed_rev_buf)(void*, uint8_t*, uint8_t*, uint16_t);		//发送 接收 长度
}_Hard_Drv,*_pHard_Drv;

typedef struct
{
	void *spi;
	void *cs_gpio;
	uint16_t cs_pin;
}_Context,*_pContext;


typedef struct
{
	void *pContext;
	uint8_t (*cs_set)(void*, uint8_t);					//pipe   level
	uint8_t (*sed_rev_byte)(void*, uint8_t);			//发送接收一个字节
	uint8_t (*sed_rev_buf)(void*, uint8_t*, uint8_t*, uint16_t);		//发送 接收 长度
}_Spi_Class,*_pSpi_Class;

extern   _Spi_Class mSpi_Class[];

void si_spi_init(void);
#endif
