#include "gd25q_driver.h"

/*==================================================================================
* 函 数 名： delay_us
* 参    数： None
* 功能描述:  不精确延时
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2021-01-04 043651
==================================================================================*/
static void delay_us(uint16_t us)
{
	for(int i=9; i<us; i++)
		for(int j=0; j<72; j++)
			{
				__NOP();
			}
}

/*==================================================================================
* 函 数 名： spi1_sed_rev
* 参    数： None
* 功能描述:  SPI1接口发送数据
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2021-01-04 043646
==================================================================================*/
static uint8_t sed_rev_byte(uint8_t value)
{
	uint8_t ret = 0; 
	while((W25QXX_SPI->SR & SPI_FLAG_TXE) == RESET);	//等待发送缓冲区空
	W25QXX_SPI->DR = value; 	//发送一个字节  

	while((W25QXX_SPI->SR & SPI_FLAG_RXNE) == RESET);	 //等待是否接收到一个字节  
	ret = (W25QXX_SPI->DR); 													//获得该字节
	
	return ret; //返回收到的字节
}

//读取状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
uint8_t W25QXX_ReadSR(void)   
{  
	uint8_t byte=0;   
	W25QXX_CS_0;                            //使能器件
	sed_rev_byte(W25X_ReadStatusReg);    //发送读取状态寄存器命令 
	byte=sed_rev_byte(0Xff);             //读取一个字节命令
	W25QXX_CS_1;                                 
	return byte;   
} 
//写状态寄存器
void W25QXX_Write_SR(uint8_t sr)   
{   
	W25QXX_CS_0;                            //使能器件
	sed_rev_byte(W25X_WriteStatusReg);   //发送写取状态寄存器命令  
	sed_rev_byte(sr);               //写入一个字节
	W25QXX_CS_1;                            //取消片选  	      
}   
//写使能	   
void W25QXX_Write_Enable(void)   
{
	W25QXX_CS_0;                            
	sed_rev_byte(W25X_WriteEnable);       
	W25QXX_CS_1;                                	      
} 
//写失能
void W25QXX_Write_Disable(void)   
{  
	W25QXX_CS_0;                            
  sed_rev_byte(W25X_WriteDisable);     
	W25QXX_CS_1;                               
} 		
//读取芯片ID  
uint16_t W25QXX_ReadID(void)
{
	uint16_t Temp = 0;	  
	W25QXX_CS_0;				    
	sed_rev_byte(0x90);//读取芯片ID命令    
	sed_rev_byte(0x00); 	    
	sed_rev_byte(0x00); 	    
	sed_rev_byte(0x00); 	 			   
	Temp|=sed_rev_byte(0xFF)<<8;  
	Temp|=sed_rev_byte(0xFF);	 
	W25QXX_CS_1;				    
	return Temp;
}   		    
//读取SPI FLASH  
void W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
 	uint16_t i;   										    
	W25QXX_CS_0;                              
    sed_rev_byte(W25X_ReadData);          
    sed_rev_byte((uint8_t)((ReadAddr)>>16));      
    sed_rev_byte((uint8_t)((ReadAddr)>>8));   
    sed_rev_byte((uint8_t)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
		{ 
        pBuffer[i]=sed_rev_byte(0XFF);   
    }
	W25QXX_CS_1;  				    	      
}  
//SPI在一页(0~65535)内写入少于256字节的数据
//pBuffer：数据存储区
//WriteAddr：开始要写入的地址
//要写入的字节数(最大256字节)
void W25QXX_Write_Page(const uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
 	uint16_t i;  
	W25QXX_Write_Enable();                  
	W25QXX_CS_0;                            
	sed_rev_byte(W25X_PageProgram);        
	sed_rev_byte((uint8_t)((WriteAddr)>>16)); 
	sed_rev_byte((uint8_t)((WriteAddr)>>8));   
	sed_rev_byte((uint8_t)WriteAddr);   
	for(i=0;i<NumByteToWrite;i++)
	{	
		sed_rev_byte(pBuffer[i]);
	}
		
	W25QXX_CS_1;                           
	W25QXX_Wait_Busy();					  
} 

//无检验写SPI_FLASH
//参数同上,NumByteToWrite最大为65535
void W25QXX_Write_NoCheck(const uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256; 	 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  
			if(NumByteToWrite>256)pageremain=256; 
			else pageremain=NumByteToWrite; 	  
		}
	};	    
} 
//写SPI FLASH  
//参数同上，NumByteToWrite最大为65535
static uint8_t W25QXX_BUFFER[4096];		 
void W25QXX_Write(const uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
	uint16_t i;    
	uint8_t * W25QXX_BUF;	  
	W25QXX_BUF=W25QXX_BUFFER;	     
	secpos=WriteAddr/4096;
	secoff=WriteAddr%4096;
	secremain=4096-secoff;
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;
	while(1) 
	{	
		W25QXX_Read(W25QXX_BUF,secpos*4096,4096);
		for(i=0;i<secremain;i++)
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;  
		}
		if(i<secremain)
		{
			W25QXX_Erase_Sector(secpos);
			for(i=0;i<secremain;i++)	 
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096); 

		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);			   
		if(NumByteToWrite==secremain)break;
		else
		{
			secpos++;
			secoff=0;	 

			pBuffer+=secremain;  
			WriteAddr+=secremain;   
			NumByteToWrite-=secremain;				
			if(NumByteToWrite>4096)secremain=4096;	
			else secremain=NumByteToWrite;			
		}
	}
}

//擦除整片芯片
void W25QXX_Erase_Chip(void)   
{                                   
	W25QXX_Write_Enable();                 
	W25QXX_Wait_Busy();   
	W25QXX_CS_0;                           
	sed_rev_byte(W25X_ChipErase);        
	W25QXX_CS_1;                                 
	W25QXX_Wait_Busy();   				   
}   
//擦除一个扇区
void W25QXX_Erase_Sector(uint32_t Dst_Addr)   
{   
	Dst_Addr*=4096;
	W25QXX_Write_Enable();                
	W25QXX_Wait_Busy();   
	W25QXX_CS_0;                            
	sed_rev_byte(W25X_SectorErase);      
	sed_rev_byte((uint8_t)((Dst_Addr)>>16));     
	sed_rev_byte((uint8_t)((Dst_Addr)>>8));   
	sed_rev_byte((uint8_t)Dst_Addr);  
	W25QXX_CS_1;                                
	W25QXX_Wait_Busy();   				   
}  

//擦除一个扇区
void W25QXX_Erase(uint32_t Dst_Addr, uint32_t length)   
{   
	uint32_t sector_num = (uint32_t)((length/4096)+1);
	
	for(int i=0; i<sector_num; i++)
	{
		W25QXX_Erase_Sector(Dst_Addr+(i*4096));
	}
	
}  

//等待空闲
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);   
}  
//进入掉电模式
void W25QXX_PowerDown(void)   
{ 
	W25QXX_CS_0;                            
	sed_rev_byte(W25X_PowerDown);        
	W25QXX_CS_1;                                	      
	delay_us(3);                               
}   
//唤醒
void W25QXX_WAKEUP(void)   
{  
	W25QXX_CS_0;                            
	sed_rev_byte(W25X_ReleasePowerDown);     
	W25QXX_CS_1;                            	      
	delay_us(3);                               
}   

