#include "stm32f1xx_hal.h"
#include "crc.h"
#include "hmc80x5_api.h"
#include "application.h"

void GPIO_Write(GPIO_TypeDef* GPIOx,unsigned char PinState)
{
	GPIOx->ODR = PinState;
}
void GPIO_WriteBit(GPIO_TypeDef* GPIOx,uint16_t GPIO_PIN,unsigned char PinState)
{
	if(PinState)
		HAL_GPIO_WritePin(GPIOx, GPIO_PIN ,	GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOx, GPIO_PIN ,	GPIO_PIN_RESET);
}

uint32_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx)
{
	return GPIOx->IDR;
}
void HMC_IO_init(void)
{
	//MX_GPIO_Init();

}

void GPIOE_Set(unsigned char pinstate)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if(pinstate)	//data out
	{
		/*Configure GPIO pins : PE2 PE3 PE4 PE5 
													 PE6 PE7 PE0 PE1 */
		GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5 
														|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_0|GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	}
	else  //data in
	{
		GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5 
													|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_0|GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	}
}
int HMC_outpb(int Device_Number,unsigned char address,unsigned char out_port)
{
  /*
  Device_Number 参数为片选，如果有多片芯片，自行添加片选程序
  */
	GPIO_WriteBit(GPIOB, GPIO_PIN_13, 1);//cs 1 
	GPIO_WriteBit(GPIOB, GPIO_PIN_14, 1);//rd 1
	GPIO_WriteBit(GPIOB, GPIO_PIN_15, 1);//wr 1
	GPIO_WriteBit(GPIOB, GPIO_PIN_13, 0);//cs
	GPIOE_Set(1);
	//MX_GPIO_Init();
  GPIO_Write(GPIOD, address);
  GPIO_Write(GPIOE, out_port);
  GPIO_WriteBit(GPIOB, GPIO_PIN_15, 0);//wr 0
  GPIO_WriteBit(GPIOB, GPIO_PIN_15, 1);//wr 1
  GPIO_WriteBit(GPIOB, GPIO_PIN_13, 1);//cs 1 
	GPIOE_Set(0);
	//MX_GPIO_Init();
	return 0; 
} 
 
int HMC_outpw(int Device_Number,unsigned char address,unsigned short out_port)
{
  /*
	Device_Number 参数为片选，如果有多片芯片，自行添加片选程序
	*/
	GPIO_WriteBit(GPIOB, GPIO_PIN_13, 1);//cs 1 
	GPIO_WriteBit(GPIOB, GPIO_PIN_14, 1);//rd 1
	GPIO_WriteBit(GPIOB, GPIO_PIN_15, 1);//wr 1
	GPIO_WriteBit(GPIOB, GPIO_PIN_13, 0);//cs

	GPIOE_Set(1);

	GPIO_Write(GPIOD, address);
	GPIO_Write(GPIOE, (unsigned char)out_port);
	GPIO_WriteBit(GPIOB, GPIO_PIN_15, 0);//wr 0
	GPIO_WriteBit(GPIOB, GPIO_PIN_15, 1);//wr 1

	GPIO_Write(GPIOD, address+1);
	GPIO_Write(GPIOE, (unsigned char)(out_port>>8));
	GPIO_WriteBit(GPIOB, GPIO_PIN_15, 0);//wr 0
	GPIO_WriteBit(GPIOB, GPIO_PIN_15, 1);//wr 1

	GPIO_WriteBit(GPIOB, GPIO_PIN_13, 1);//cs 1 
	GPIOE_Set(0);
	return 0;
} 
int HMC_outpLw(int Device_Number,unsigned char address,unsigned int out_port)
{
	/*
	Device_Number 参数为片选，如果有多片芯片，自行添加片选程序
	*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_WriteBit(GPIOB, GPIO_PIN_13, 1);//cs 1 
	GPIO_WriteBit(GPIOB, GPIO_PIN_14, 1);//rd 1
	GPIO_WriteBit(GPIOB, GPIO_PIN_15, 1);//wr 1
	GPIO_WriteBit(GPIOB, GPIO_PIN_13, 0);//cs

	GPIOE_Set(1);

	GPIO_Write(GPIOD, address);
	GPIO_Write(GPIOE, (unsigned char)out_port);
	GPIO_WriteBit(GPIOB, GPIO_PIN_15, 0);//wr 0
	GPIO_WriteBit(GPIOB, GPIO_PIN_15, 1);//wr 1

	GPIO_Write(GPIOD, address+1);
	GPIO_Write(GPIOE, (unsigned char)(out_port>>8));
	GPIO_WriteBit(GPIOB, GPIO_PIN_15, 0);//wr 0
	GPIO_WriteBit(GPIOB, GPIO_PIN_15, 1);//wr 1

	GPIO_Write(GPIOD, address+2);
	GPIO_Write(GPIOE, (unsigned char)(out_port>>16));
	GPIO_WriteBit(GPIOB, GPIO_PIN_15, 0);//wr 0
	GPIO_WriteBit(GPIOB, GPIO_PIN_15, 1);//wr 1

	GPIO_Write(GPIOD, address+2);
	GPIO_Write(GPIOE, (unsigned char)(out_port>>24));
	GPIO_WriteBit(GPIOB, GPIO_PIN_15, 0);//wr 0
	GPIO_WriteBit(GPIOB, GPIO_PIN_15, 1);//wr 1

	GPIO_WriteBit(GPIOB, GPIO_PIN_13, 1);//cs 1 
	GPIOE_Set(0);  
	return 0;
}
unsigned char HMC_inpb(int Device_Number,unsigned char address)
{
	/*
	Device_Number 参数为片选，如果有多片芯片，自行添加片选程序
	*/
	unsigned char   data;
	GPIO_WriteBit(GPIOB, GPIO_PIN_13, 1);//cs 1 
	GPIO_WriteBit(GPIOB, GPIO_PIN_14, 1);//rd 1
	GPIO_WriteBit(GPIOB, GPIO_PIN_15, 1);//wr 1
	GPIO_WriteBit(GPIOB, GPIO_PIN_13, 0);//cs

	GPIO_Write(GPIOD, address);
	GPIO_WriteBit(GPIOB, GPIO_PIN_14, 0);//rd 0 
	data=(unsigned char)GPIO_ReadInputData(GPIOE);
	GPIO_WriteBit(GPIOB, GPIO_PIN_14, 1);//rd 1

	GPIO_WriteBit(GPIOB, GPIO_PIN_13, 1);//cs 1 
	return (data);
	 
}
unsigned short HMC_inpw(int Device_Number,unsigned char address)
{
	/*
	Device_Number 参数为片选，如果有多片芯片，自行添加片选程序
	*/
	unsigned char   data,data1;
	GPIO_WriteBit(GPIOB, GPIO_PIN_13, 1);//cs 1 
	GPIO_WriteBit(GPIOB, GPIO_PIN_14, 1);//rd 1
	GPIO_WriteBit(GPIOB, GPIO_PIN_15, 1);//wr 1
	GPIO_WriteBit(GPIOB, GPIO_PIN_13, 0);//cs

	GPIO_Write(GPIOD, address);
	GPIO_WriteBit(GPIOB, GPIO_PIN_14, 0);//rd 0 
	data=(unsigned char)GPIO_ReadInputData(GPIOE);
	GPIO_WriteBit(GPIOB, GPIO_PIN_14, 1);//rd 1

	GPIO_Write(GPIOD, address+1);
	GPIO_WriteBit(GPIOB, GPIO_PIN_14, 0);//rd 0 
	data1=(unsigned char)GPIO_ReadInputData(GPIOE);
	GPIO_WriteBit(GPIOB, GPIO_PIN_14, 1);//rd 1

	GPIO_WriteBit(GPIOB, GPIO_PIN_13, 1);//cs 1 
	return (data+((unsigned short)data1<<8));
}
unsigned int HMC_inpLw(int Device_Number,unsigned char address)
{ 
	/*
	Device_Number 参数为片选，如果有多片芯片，自行添加片选程序
	*/
	unsigned char data,data1,data2,data3;
	GPIO_WriteBit(GPIOB, GPIO_PIN_13, 1);//cs 1 
	GPIO_WriteBit(GPIOB, GPIO_PIN_14, 1);//rd 1
	GPIO_WriteBit(GPIOB, GPIO_PIN_15, 1);//wr 1
	GPIO_WriteBit(GPIOB, GPIO_PIN_13, 0);//cs

	GPIO_Write(GPIOD, address);
	GPIO_WriteBit(GPIOB, GPIO_PIN_14, 0);//rd 0 
	data=(unsigned char)GPIO_ReadInputData(GPIOE);
	GPIO_WriteBit(GPIOB, GPIO_PIN_14, 1);//rd 1

	GPIO_Write(GPIOD, address+1);
	GPIO_WriteBit(GPIOB, GPIO_PIN_14, 0);//rd 0 
	data1=(unsigned char)GPIO_ReadInputData(GPIOE);
	GPIO_WriteBit(GPIOB, GPIO_PIN_14, 1);//rd 1

	GPIO_Write(GPIOD, address+2);
	GPIO_WriteBit(GPIOB, GPIO_PIN_14, 0);//rd 0 
	data2=(unsigned char)GPIO_ReadInputData(GPIOE);
	GPIO_WriteBit(GPIOB, GPIO_PIN_14, 1);//rd 1

	GPIO_Write(GPIOD, address+3);
	GPIO_WriteBit(GPIOB, GPIO_PIN_14, 0);//rd 0 
	data3=(unsigned char)GPIO_ReadInputData(GPIOE);
	GPIO_WriteBit(GPIOB, GPIO_PIN_14, 1);//rd 1

	GPIO_WriteBit(GPIOB, GPIO_PIN_13, 1);//cs 1 	 
	return (data+((unsigned int)data1<<8)+((unsigned int)data2<<16)+((unsigned int)data3<<24));
}
  
int Read_Position(int dev,unsigned int Axs,unsigned int* Pos,unsigned char* RunState,unsigned char* IOState,unsigned char* SyncIO)
{
	unsigned char st;
	if(Axs==0)
	{
		*Pos=HMC_inpLw(dev,0);
		st=HMC_inpb(dev,32);
	}
	else
	if(Axs==1)
	{	
		* Pos=HMC_inpLw(dev,4);
			st=HMC_inpb(dev,33);
	}
	else
	if(Axs==2)
	{	
		* Pos=HMC_inpLw(dev,8);
			st=HMC_inpb(dev,34);
	}
	else if(Axs==3)
	{
		* Pos=HMC_inpLw(dev,12);
			st=HMC_inpb(dev,35);
	}
	else if(Axs==4)
	{
		* Pos=HMC_inpLw(dev,16);
			st=HMC_inpb(dev,36);
	}
	else 
	{
		* Pos=HMC_inpLw(dev,20);
			st=HMC_inpb(dev,37);
	}
	*RunState=st&0x0f;
	*IOState=(st>>4)&0x0f;
  * SyncIO=HMC_inpb(dev,38);
  return 0;
}
 
 
int Read_Speed(int dev,unsigned int Axs,unsigned int* speed)
{
	unsigned char st;unsigned long RE,rmg;unsigned int ss;
	if(Axs==0)
	{
		RE=HMC_inpw(dev,40);
		rmg=HMC_inpw(dev,42);
		st=HMC_inpb(dev,32);
	}
	else
	if(Axs==1)
	{	
		RE=HMC_inpw(dev,44);
		rmg=HMC_inpw(dev,46);
		st=HMC_inpb(dev,33);
	}
	else
	if(Axs==2)
	{	
		RE=HMC_inpw(dev,48);
		rmg=HMC_inpw(dev,50);
		st=HMC_inpb(dev,34);
	}
	else if(Axs==3)
	{
		RE=HMC_inpw(dev,52);
		rmg=HMC_inpw(dev,54);
		st=HMC_inpb(dev,35);
	}
	else if(Axs==4)
	{
		RE=HMC_inpw(dev,56);
		rmg=HMC_inpw(dev,58);
		st=HMC_inpb(dev,36);
	}
	else  
	{
		RE=HMC_inpw(dev,60);
		rmg=HMC_inpw(dev,62);
		st=HMC_inpb(dev,37);
	}
	st=st&0x0f;
	if(st>1)
	{
		ss=19660800.0*RE/65536/(rmg+1);
		* speed=ss;
	}
	else
	{
		* speed=0;
	}

	return 0;
}

int  YNC_Start(int dev)
{
	HMC_outpb(dev,5,1);
	if(HMC_outpb(dev,5,0)==-1)return -1;		
	return 0;
}
int SYNC_Stop(int dev)
{ 
	HMC_outpb(dev,6,1);
	if(HMC_outpb(dev,6,0)==-1)return -1;		
	return 0;
}

int Set_Axs(int dev,unsigned int Axs,unsigned int Run_EN,unsigned int Csta_EN,unsigned int Cstp_EN,unsigned int Csd_EN)
{
	unsigned char  Dir=0,temp = 0;

	if(Run_EN)Dir=Dir+1;
	if(Csta_EN)Dir=Dir+2;
	if(Cstp_EN)Dir=Dir+4;
	if(Csd_EN)Dir=Dir+8;
	if(Axs==0)
	{
		if(HMC_outpb(dev,20,Dir)==-1)return -1;
		temp = HMC_inpb(dev,20);
		//debug_print("Dir = %2x,temp = %2x\n",Dir,temp);
	}
	else
	if(Axs==1)
	{
		if(HMC_outpb(dev,50,Dir)==-1)return -1;
	}
	else
	if(Axs==2)
	{
		if(HMC_outpb(dev,80,Dir)==-1)return -1;
	}
	else
	if(Axs==3)
	{
		if(HMC_outpb(dev,110,Dir)==-1)return -1;
	}
	return 0;
}
int AxsStop(int dev,unsigned int Axs)
{
	if(Axs==0)
	{
		HMC_outpb(dev,22,1);
		if(HMC_outpb(dev,22,0)==-1)return -1;
	}
	else
	if(Axs==1)
	{
		HMC_outpb(dev,52,1);
		if(HMC_outpb(dev,52,0)==-1)return -1;
	}
	else
	if(Axs==2)
	{
		HMC_outpb(dev,82,1);
		if(HMC_outpb(dev,82,0)==-1)return -1;
	}
	else
	if(Axs==3)
	{
		HMC_outpb(dev,112,1);
		if(HMC_outpb(dev,112,0)==-1)return -1;
	}
	return 0;
}
int MovToOrg(int dev,unsigned int Axs,unsigned int Dir,unsigned char Outmod,unsigned int Speed)
{
	unsigned int  Pos;unsigned char  RunState;unsigned char IOState;unsigned char  SyncIO;
	Read_Position(dev, Axs,&Pos,&RunState,&IOState,&SyncIO);
	if(RunState) return -2;

	unsigned int AX_REL_V,AX_REH_V,rmgdt,AX_RMD_V,AX_RDP_V;
	unsigned short AX_RUR_V;//加速率
	unsigned short AX_RDR_V;//减速率
	unsigned char  AX_PMD_V; //输出模式寄存器
	double buf,buf1,AX_RUR_S,AX_RDR_S,rdp,rup; 
	int erro=0;
	rmgdt=2999;	
	if(Speed>3276)rmgdt=1499;	//0.1倍放大模式   0.1--6553.5 参考时钟是12M 倍率=参考时钟/65536/（prmg+1) 
	if(Speed>6553)rmgdt=599;										    //0.2倍放大模式   0.2--13107                
	if(Speed>13107)rmgdt=299;										    //0.5倍放大模式   0.5--32767                
	if(Speed>32767)rmgdt=149;		 						  		    // 1倍放大模式
	if(Speed>65535)rmgdt=59;										    // 2倍放大模式
	if(Speed>131070)rmgdt=29;										    // 5倍放大模式
	if(Speed>327675)rmgdt=14;		 							   	    //10倍放大模式
	if(Speed>655350)rmgdt=5;		 								    //20倍放大模式
	if(Speed>1310700)rmgdt=2;		  								    //50倍放大模式
	if(Speed>3276750)rmgdt=1;		  								    //100倍放大模式

	AX_REL_V=Speed/(300.0/(rmgdt+1));	  							        //计算实际的FL速度	 

	if(Dir) //负方向
		AX_RMD_V=0x0080;
	else  //正方向
		AX_RMD_V=0;

	AX_PMD_V= Outmod;
	unsigned short base_addr;
	if(Axs==0)
	{
		base_addr=20;	
	}
	else
	if(Axs==1)
	{ 
		base_addr=50;
	} 
	else
	if(Axs==2)
	{ 
		base_addr=80;
	} 
	else
	if(Axs==3)
	{ 
		base_addr=110;
	} 
	if(HMC_outpb(dev,base_addr+1,0)==-1) return -1;
	//// HMC_outpLw(dev,base_addr+3,Length);//RMV
	//HMC_outpLw(dev,base_addr+17,AX_RDP_V);//RDP
	//HMC_outpw(dev,base_addr+11,AX_RUR_V);//RUR
	//HMC_outpw(dev,base_addr+15,AX_RDR_V);//RDR
	HMC_outpw(dev,base_addr+13,rmgdt);//RMG
	HMC_outpw(dev,base_addr+7,AX_REL_V);//REL
	//HMC_outpw(dev,base_addr+9,AX_REH_V);//REH

	HMC_outpb(dev,base_addr+27,AX_PMD_V);//PMD
	HMC_outpw(dev,base_addr+21,AX_RMD_V);//RMD
	HMC_outpb(dev,base_addr+1,1);//start
	HMC_outpb(dev,base_addr+1,0);//start to 0
	return erro;
}


int FL_ContinueMov(int dev,unsigned int Axs,unsigned int Dir,unsigned char Outmod,unsigned int Vo,unsigned int Vt)
{
	unsigned int  Pos;unsigned char  RunState;unsigned char IOState;unsigned char  SyncIO;
	Read_Position(dev, Axs,&Pos,&RunState,&IOState,&SyncIO);
	if(RunState) return -2;

	unsigned int AX_REL_V,AX_REH_V,rmgdt,AX_RMD_V;
	unsigned char  AX_PMD_V; //输出模式寄存器
	int erro=0;
	Circular_Reset(dev);  //round reset
	rmgdt=2999;	
	if(Vt>3276)rmgdt=1499;	//0.1倍放大模式   0.1--6553.5 参考时钟是12M 倍率=参考时钟/65536/（prmg+1) 
	if(Vt>6553)rmgdt=599;										    //0.2倍放大模式   0.2--13107                
	if(Vt>13107)rmgdt=299;										    //0.5倍放大模式   0.5--32767                
	if(Vt>32767)rmgdt=149;		 						  		    // 1倍放大模式
	if(Vt>65535)rmgdt=59;										    // 2倍放大模式
	if(Vt>131070)rmgdt=29;										    // 5倍放大模式
	if(Vt>327675)rmgdt=14;		 							   	    //10倍放大模式
	if(Vt>655350)rmgdt=5;		 								    //20倍放大模式
	if(Vt>1310700)rmgdt=2;		  								    //50倍放大模式
	if(Vt>3276750)rmgdt=1;		  								    //100倍放大模式


	AX_REL_V=Vo/(300.0/(rmgdt+1));	  							        //计算实际的FL速度
	AX_REH_V=Vt/(300.0/(rmgdt+1));	  							        //计算实际的FH速度
	if(Dir) //负方向
	AX_RMD_V=0x0082;
	else  //正方向
	AX_RMD_V=0x0002;
	AX_PMD_V= Outmod;
	unsigned short base_addr;
	if(Axs==0)
	{
	 base_addr=20;	
	}
	else
	if(Axs==1)
	{ 
	base_addr=50;
	} 
	else	
	if(Axs==2)
	{ 
	base_addr=80;
	} 
	else
	if(Axs==3)
	{ 
	base_addr=110;
	} 
	if(HMC_outpb(dev,base_addr+1,0)==-1) return -1;
	//// HMC_outpLw(dev,base_addr+3,Length);//RMV
	//HMC_outpLw(dev,base_addr+17,AX_RDP_V);//RDP
	//HMC_outpw(dev,base_addr+11,AX_RUR_V);//RUR
	//HMC_outpw(dev,base_addr+15,AX_RDR_V);//RDR
	HMC_outpw(dev,base_addr+13,rmgdt);//RMG
	HMC_outpw(dev,base_addr+7,AX_REL_V);//REL
	HMC_outpw(dev,base_addr+9,AX_REH_V);//REH

	HMC_outpb(dev,base_addr+27,AX_PMD_V);//PMD
	HMC_outpw(dev,base_addr+21,AX_RMD_V);//RMD
	HMC_outpb(dev,base_addr+1,1);//start
	HMC_outpb(dev,base_addr+1,0);//start to 0

	return erro;
 }

int FH_ContinueMov(int dev,unsigned int Axs,unsigned int Dir,unsigned char Outmod,unsigned int Vo,unsigned int Vt,unsigned int SD_EN)
{
	unsigned int  Pos;unsigned char  RunState;unsigned char IOState;unsigned char  SyncIO;
	Read_Position(dev, Axs,&Pos,&RunState,&IOState,&SyncIO);
	if(RunState) return -2;
	unsigned int AX_REL_V,AX_REH_V,rmgdt,AX_RMD_V;
	unsigned char  AX_PMD_V; //输出模式寄存器
	int erro=0;
	Circular_Reset(dev);  //round reset
	rmgdt=2999;	
	if(Vt>3276)rmgdt=1499;	//0.1倍放大模式   0.1--6553.5 参考时钟是12M 倍率=参考时钟/65536/（prmg+1) 
	if(Vt>6553)rmgdt=599;										    //0.2倍放大模式   0.2--13107                
	if(Vt>13107)rmgdt=299;										    //0.5倍放大模式   0.5--32767                
	if(Vt>32767)rmgdt=149;		 						  		    // 1倍放大模式
	if(Vt>65535)rmgdt=59;										    // 2倍放大模式
	if(Vt>131070)rmgdt=29;										    // 5倍放大模式
	if(Vt>327675)rmgdt=14;		 							   	    //10倍放大模式
	if(Vt>655350)rmgdt=5;		 								    //20倍放大模式
	if(Vt>1310700)rmgdt=2;		  								    //50倍放大模式
	if(Vt>3276750)rmgdt=1;		  								    //100倍放大模式


	AX_REL_V=Vo/(300.0/(rmgdt+1));	  							        //计算实际的FL速度
	AX_REH_V=Vt/(300.0/(rmgdt+1));	  							        //计算实际的FH速度
	if(Dir) //负方向
	AX_RMD_V=0x0083;
	else  //正方向
	AX_RMD_V=0x0003;

	if(SD_EN)AX_RMD_V=AX_RMD_V+0x0100;
	AX_PMD_V= Outmod;
	unsigned short base_addr;
	if(Axs==0)
	{
	 base_addr=20;	
	}
	else
	if(Axs==1)
	{ 
	base_addr=50;
	} 
	else
	if(Axs==2)
	{ 
	base_addr=80;
	} 
	else
	if(Axs==3)
	{ 
	base_addr=110;
	} 
	if(HMC_outpb(dev,base_addr+1,0)==-1) return -1;
	//// HMC_outpLw(dev,base_addr+3,Length);//RMV
	//HMC_outpLw(dev,base_addr+17,AX_RDP_V);//RDP
	//HMC_outpw(dev,base_addr+11,AX_RUR_V);//RUR
	//HMC_outpw(dev,base_addr+15,AX_RDR_V);//RDR
	HMC_outpw(dev,base_addr+13,rmgdt);//RMG
	HMC_outpw(dev,base_addr+7,AX_REL_V);//REL
	HMC_outpw(dev,base_addr+9,AX_REH_V);//REH

	HMC_outpb(dev,base_addr+27,AX_PMD_V);//PMD
	HMC_outpw(dev,base_addr+21,AX_RMD_V);//RMD
	HMC_outpb(dev,base_addr+1,1);//start
	HMC_outpb(dev,base_addr+1,0);//start to 0
	return erro;
 }

int FH_ContinueAdjustSpeed(int dev,unsigned int Axs,unsigned int Vo,unsigned int Vt)
{
	unsigned int  Pos;unsigned char  RunState;unsigned char IOState;unsigned char  SyncIO;
	Read_Position(dev, Axs,&Pos,&RunState,&IOState,&SyncIO);
	if(RunState!=7) return -2;
	unsigned int AX_REL_V,AX_REH_V,rmgdt,AX_RMD_V;
	unsigned char  AX_PMD_V; //输出模式寄存器
	int erro=0;

	rmgdt=2999;	
	if(Vt>3276)rmgdt=1499;	//0.1倍放大模式   0.1--6553.5 参考时钟是12M 倍率=参考时钟/65536/（prmg+1) 
	if(Vt>6553)rmgdt=599;										    //0.2倍放大模式   0.2--13107                
	if(Vt>13107)rmgdt=299;										    //0.5倍放大模式   0.5--32767                
	if(Vt>32767)rmgdt=149;		 						  		    // 1倍放大模式
	if(Vt>65535)rmgdt=59;										    // 2倍放大模式
	if(Vt>131070)rmgdt=29;										    // 5倍放大模式
	if(Vt>327675)rmgdt=14;		 							   	    //10倍放大模式
	if(Vt>655350)rmgdt=5;		 								    //20倍放大模式
	if(Vt>1310700)rmgdt=2;		  								    //50倍放大模式
	if(Vt>3276750)rmgdt=1;		  								    //100倍放大模式


	AX_REL_V=Vo/(300.0/(rmgdt+1));	  							        //计算实际的FL速度
	AX_REH_V=Vt/(300.0/(rmgdt+1));	  							        //计算实际的FH速度
	unsigned short base_addr;
	if(Axs==0)
	{
	 base_addr=20;	
	}
	else
	if(Axs==1)
	{ 
	base_addr=50;
	} 
	else
	if(Axs==2)
	{ 
	base_addr=80;
	} 
	else
	if(Axs==3)
	{ 
	base_addr=110;
	} 
	// if(HMC_outpb(dev,base_addr+1,0)==-1) return -1;
	//// HMC_outpLw(dev,base_addr+3,Length);//RMV
	//HMC_outpLw(dev,base_addr+17,AX_RDP_V);//RDP
	//HMC_outpw(dev,base_addr+11,AX_RUR_V);//RUR
	//HMC_outpw(dev,base_addr+15,AX_RDR_V);//RDR
	HMC_outpw(dev,base_addr+13,rmgdt);//RMG
	HMC_outpw(dev,base_addr+7,AX_REL_V);//REL
	HMC_outpw(dev,base_addr+9,AX_REH_V);//REH

	//HMC_outpb(dev,base_addr+27,AX_PMD_V);//PMD
	//HMC_outpw(dev,base_addr+21,AX_RMD_V);//RMD
	// HMC_outpb(dev,base_addr+1,1);//start
	//  HMC_outpb(dev,base_addr+1,0);//start to 0
	return erro;
 }

int DeltMov(int dev,unsigned int Axs,unsigned int curve,unsigned int Dir,unsigned char Outmod,unsigned int Vo,unsigned int Vt,unsigned int Length,unsigned int StartDec,unsigned int Acctime,unsigned int Dectime,unsigned int SD_EN,unsigned int WaitSYNC)
{ 
	unsigned int  Pos;unsigned char  RunState;unsigned char IOState;unsigned char  SyncIO;
	Read_Position(dev, Axs,&Pos,&RunState,&IOState,&SyncIO);
	debug_print("RunState =%d\n",RunState);
	if(RunState) return -1;

	unsigned int AX_REL_V,AX_REH_V,rmgdt,AX_RMD_V,AX_RDP_V;
	unsigned short AX_RUR_V;//加速率
	unsigned short AX_RDR_V;//减速率
	unsigned char  AX_PMD_V; //输出模式寄存器
	double buf,buf1,AX_RUR_S,AX_RDR_S,rdp,rup; 
	int erro=0;
	rmgdt=2999;	
	if(Vt>3276)rmgdt=1499;	//0.1倍放大模式   0.1--6553.5 参考时钟是12M 倍率=参考时钟/65536/（prmg+1) 
	if(Vt>6553)rmgdt=599;										    //0.2倍放大模式   0.2--13107                
	if(Vt>13107)rmgdt=299;										    //0.5倍放大模式   0.5--32767                
	if(Vt>32767)rmgdt=149;		 						  		    // 1倍放大模式
	if(Vt>65535)rmgdt=59;										    // 2倍放大模式
	if(Vt>131070)rmgdt=29;										    // 5倍放大模式
	if(Vt>327675)rmgdt=14;		 							   	    //10倍放大模式
	if(Vt>655350)rmgdt=5;		 								    //20倍放大模式
	if(Vt>1310700)rmgdt=2;		  								    //50倍放大模式
	if(Vt>3276750)rmgdt=1;		  								    //100倍放大模式
	Circular_Reset(dev);  //round reset
	AX_REL_V=Vo/(300.0/(rmgdt+1));	  							        //计算实际的FL速度
	AX_REH_V=Vt/(300.0/(rmgdt+1));	  							        //计算实际的FH速度
	buf= AX_REH_V-AX_REL_V;
	buf=buf/19660800.0;
	if(curve)
	{
		if(Dir) //负方向
		AX_RMD_V=0x0481;
		else  //正方向
		AX_RMD_V=0x0401;


		buf1=Acctime;
		AX_RUR_V=buf1/1000/buf/2;
		buf1=Dectime;
		AX_RDR_V=buf1/1000/buf/2; 
	}
	else
	{
		if(Dir) //负方向
		AX_RMD_V=0x0081;
		else  //正方向
		AX_RMD_V=0x0001;

		buf1=Acctime;
		AX_RUR_V=buf1/1000/buf;
		buf1=Dectime;
		AX_RDR_V=buf1/1000/buf; 
	}
	if(SD_EN)AX_RMD_V=AX_RMD_V+0x0100;

	buf=AX_REH_V;
	buf=buf*buf;
	buf1=AX_REL_V;
	buf1=buf1*buf1;
	if(curve)//S
	{
		rup=(buf-buf1)*(AX_RUR_V+1)/(rmgdt+1)/65536;//系数待定  最佳脉冲位置 =（AX_REH_V×AX_REH_V-AX_REL_V×AX_REL_V）*(AX_RUR_V+1)/(rmgdt+1)/65536/4;
		rdp=(buf-buf1)*(AX_RDR_V+1)/(rmgdt+1)/65536;
	}
	else
	{
		rup=(buf-buf1)*(AX_RUR_V+1)/(rmgdt+1)/65536/2;//系数待定  最佳脉冲位置 =（AX_REH_V×AX_REH_V-AX_REL_V×AX_REL_V）*(AX_RUR_V+1)/(rmgdt+1)/65536/4;
		rdp=(buf-buf1)*(AX_RDR_V+1)/(rmgdt+1)/65536/2;
	}


	if((rdp+rup+100)>Length)
	{
		//printf("rdp = %f,rup = %f,Length = %d \n",rdp,rup,Length);
		erro=-3;
		AX_RDP_V=Length/2;
	}
	else
	{  
		if(StartDec==0)
		AX_RDP_V=Length-(unsigned int)rdp;
		else
		AX_RDP_V=StartDec;
		erro=AX_RDP_V;
	}

	AX_PMD_V= Outmod;
	unsigned short base_addr;

	if(Axs==0)
	{
	 base_addr=20;	
	}
	else
	if(Axs==1)
	{ 
		base_addr=50;
	} 
	else
	if(Axs==2)
	{ 
		base_addr=80;
	} 
	else
	if(Axs==3)
	{ 
		base_addr=110;
	} 
	if(HMC_outpb(dev,base_addr+1,0)==-1) return -1;
	HMC_outpLw(dev,base_addr+3,Length);//RMV
	HMC_outpLw(dev,base_addr+17,AX_RDP_V);//RDP
	HMC_outpw(dev,base_addr+11,AX_RUR_V);//RUR
	HMC_outpw(dev,base_addr+15,AX_RDR_V);//RDR
	HMC_outpw(dev,base_addr+13,rmgdt);//RMG
	HMC_outpw(dev,base_addr+7,AX_REL_V);//REL
	HMC_outpw(dev,base_addr+9,AX_REH_V);//REH
	//HMC_outpb(dev,base_addr+23,3);//A
	//HMC_outpb(dev,base_addr+25,2);//B
	HMC_outpb(dev,base_addr+27,AX_PMD_V);//PMD
	HMC_outpw(dev,base_addr+21,AX_RMD_V);//RMD
	if(WaitSYNC==0)
	{HMC_outpb(dev,base_addr+1,1);//start
	HMC_outpb(dev,base_addr+1,0);//start
	}
	return erro;
}
int Read_EndAccP(int dev,unsigned int* AccP)
{ 
	*AccP=HMC_inpLw(dev,50); 
	return 0; 
}
  
 int AXIS_Interpolation2(int dev,unsigned int Axs1,unsigned int Axs2,unsigned int curve,unsigned int Dir1,unsigned int Dir2,
									unsigned char Outmod,unsigned int Vo,unsigned int Vt,unsigned int Length1,unsigned int Length2,unsigned int StartDec,
									unsigned int Acctime,unsigned int Dectime,unsigned int SD_EN,unsigned int WaitSYNC)
{ 
	unsigned int  Pos;unsigned char  RunState;unsigned char IOState;unsigned char  SyncIO;
	Read_Position(dev, Axs1,&Pos,&RunState,&IOState,&SyncIO);
	if(RunState) return -2;
	Read_Position(dev, Axs2,&Pos,&RunState,&IOState,&SyncIO);
	if(RunState) return -2;
	unsigned int AX_REL_V,AX_REH_V,rmgdt,AX_RMD_V1,AX_RMD_V2,AX_RDP_V;
	unsigned short AX_RUR_V;//加速率
	unsigned short AX_RDR_V;//减速率
	unsigned char  AX_PMD_V; //输出模式寄存器
	double buf,buf1,AX_RUR_S,AX_RDR_S,rdp,rup; 
	unsigned int A,B,C,linA;
	unsigned char start;
	unsigned int gongyue,temp;
	int erro=0;
	rmgdt=2999;	
	if(Vt>3276)rmgdt=1499;	//0.1倍放大模式   0.1--6553.5 参考时钟是12M 倍率=参考时钟/65536/（prmg+1) 
	if(Vt>6553)rmgdt=599;										    //0.2倍放大模式   0.2--13107                
	if(Vt>13107)rmgdt=299;										    //0.5倍放大模式   0.5--32767                
	if(Vt>32767)rmgdt=149;		 						  		    // 1倍放大模式
	if(Vt>65535)rmgdt=59;										    // 2倍放大模式
	if(Vt>131070)rmgdt=29;										    // 5倍放大模式
	if(Vt>327675)rmgdt=14;		 							   	    //10倍放大模式
	if(Vt>655350)rmgdt=5;		 								    //20倍放大模式
	if(Vt>1310700)rmgdt=2;		  								    //50倍放大模式
	if(Vt>3276750)rmgdt=1;		  								    //100倍放大模式
	Circular_Reset(dev);  //round reset
	AX_REL_V=Vo/(300.0/(rmgdt+1));	  							        //计算实际的FL速度
	AX_REH_V=Vt/(300.0/(rmgdt+1));	  							        //计算实际的FH速度
	buf= AX_REH_V-AX_REL_V;
	buf=buf/19660800.0;
	if(curve)
	{
		if(Dir1) //负方向
		AX_RMD_V1=0x04c1;
		else  //正方向
		AX_RMD_V1=0x0441;

		if(Dir2) //负方向
		AX_RMD_V2=0x04c1;
		else  //正方向
		AX_RMD_V2=0x0441;

		buf1=Acctime;
		AX_RUR_V=buf1/1000/buf/2;
		buf1=Dectime;
		AX_RDR_V=buf1/1000/buf/2; 
	}
	else
	{
		if(Dir1) //负方向
		AX_RMD_V1=0x00c1;
		else  //正方向
		AX_RMD_V1=0x0041;

		if(Dir2) //负方向
		AX_RMD_V2=0x00c1;
		else  //正方向
		AX_RMD_V2=0x0041;

		buf1=Acctime;
		AX_RUR_V=buf1/1000/buf;
		buf1=Dectime;
		AX_RDR_V=buf1/1000/buf; 
	}
	if(SD_EN)
	{
		AX_RMD_V1=AX_RMD_V1+0x0100;
		AX_RMD_V2=AX_RMD_V2+0x0100;
	}
	// gongyue=divisor(Length1,Length2); //求最大公约数
	if(Length1>Length2)
	{
		AX_RMD_V2=AX_RMD_V2+0x0800;
		linA=Length1;
		A=Length1/Length2;
		if(A>65535) return -3; //插补轴步进太少

		C=Length1%Length2;
		if(C>0)
		B=Length2/C;
		else
		B=1;
	}
	else
	{
		AX_RMD_V1=AX_RMD_V1+0x0800;
		linA=Length2;
		A=Length2/Length1;
		if(A>65535) return -3; //插补轴步进太少

		C=Length2%Length1;
		if(C>0)
		B=Length1/C;
		else
		B=1;
	}
	if(A>0)A=A-1;
	if(B>0) B=B-1;
	buf=AX_REH_V;
	buf=buf*buf;
	buf1=AX_REL_V;
	buf1=buf1*buf1;
	if(curve)//S
	{
		rup=(buf-buf1)*(AX_RUR_V+1)/(rmgdt+1)/65536;//系数待定  最佳脉冲位置 =（AX_REH_V×AX_REH_V-AX_REL_V×AX_REL_V）*(AX_RUR_V+1)/(rmgdt+1)/65536/4;
		rdp=(buf-buf1)*(AX_RDR_V+1)/(rmgdt+1)/65536;
	}
	else
	{
		rup=(buf-buf1)*(AX_RUR_V+1)/(rmgdt+1)/65536/2;//系数待定  最佳脉冲位置 =（AX_REH_V×AX_REH_V-AX_REL_V×AX_REL_V）*(AX_RUR_V+1)/(rmgdt+1)/65536/4;
		rdp=(buf-buf1)*(AX_RDR_V+1)/(rmgdt+1)/65536/2;
	}
	if(Length1>Length2)
	{
		if((rdp+rup+100)>Length1)
		{
			erro=-2;
			AX_RDP_V=Length1/2;
		}
		else
		{  
			if(StartDec==0)
				AX_RDP_V=Length1-(unsigned int)rdp;
			else
				AX_RDP_V=StartDec;
			erro=AX_RDP_V;
		}
	}
	else
	{
		if((rdp+rup+100)>Length2)
		{
			erro=-2;
			AX_RDP_V=Length2/2;
		}
		else
		{  
			if(StartDec==0)
				AX_RDP_V=Length2-(unsigned int)rdp;
			else
				AX_RDP_V=StartDec;
			erro=AX_RDP_V;
		}
	}
	AX_PMD_V= Outmod;
	unsigned short base_addr;

	if(Axs1==0)
	{
		base_addr=20;	
		HMC_outpw(dev,140,(unsigned short)A);//
		HMC_outpLw(dev,142,B);//
		HMC_outpLw(dev,146,C);//
	}
	else
	if(Axs1==1)
	{ 
		base_addr=50;
		HMC_outpw(dev,150,(unsigned short)A);//
		HMC_outpLw(dev,152,B);//
		HMC_outpLw(dev,156,C);//
	} 
	else
	if(Axs1==2)
	{ 
		base_addr=80;
		HMC_outpw(dev,160,(unsigned short)A);//
		HMC_outpLw(dev,162,B);//
		HMC_outpLw(dev,166,C);//
	} 
	else
	if(Axs1==3)
	{ 
		base_addr=110;
		HMC_outpw(dev,170,(unsigned short)A);//
		HMC_outpLw(dev,172,B);//
		HMC_outpLw(dev,176,C);//
	} 

	if(HMC_outpb(dev,base_addr+1,0)==-1) return -1;
	HMC_outpLw(dev,base_addr+3,Length1);//RMV
	HMC_outpLw(dev,base_addr+17,AX_RDP_V);//RDP
	HMC_outpw(dev,base_addr+11,AX_RUR_V);//RUR
	HMC_outpw(dev,base_addr+15,AX_RDR_V);//RDR
	HMC_outpw(dev,base_addr+13,rmgdt);//RMG
	HMC_outpw(dev,base_addr+7,AX_REL_V);//REL
	HMC_outpw(dev,base_addr+9,AX_REH_V);//REH

	HMC_outpLw(dev,base_addr+23,linA);//A

	HMC_outpb(dev,base_addr+27,AX_PMD_V);//PMD 
	HMC_outpw(dev,base_addr+21,AX_RMD_V1);//RMD

	if(Axs2==0)
	{
		base_addr=20;	
		HMC_outpw(dev,140,(unsigned short)A);//
		HMC_outpLw(dev,142,B);//
		HMC_outpLw(dev,146,C);//
	}
	else
	if(Axs2==1)
	{ 
		base_addr=50;
		HMC_outpw(dev,150,(unsigned short)A);//
		HMC_outpLw(dev,152,B);//
		HMC_outpLw(dev,156,C);//
	} 
	else
	if(Axs2==2)
	{ 
		base_addr=80;
		HMC_outpw(dev,160,(unsigned short)A);//
		HMC_outpLw(dev,162,B);//
		HMC_outpLw(dev,166,C);//
	} 
	else
	if(Axs2==3)
	{ 
		base_addr=110;
		HMC_outpw(dev,170,(unsigned short)A);//
		HMC_outpLw(dev,172,B);//
		HMC_outpLw(dev,176,C);//
	} 

	if(HMC_outpb(dev,base_addr+1,0)==-1) return -1;
	HMC_outpLw(dev,base_addr+3,Length2);//RMV
	HMC_outpLw(dev,base_addr+17,AX_RDP_V);//RDP
	HMC_outpw(dev,base_addr+11,AX_RUR_V);//RUR
	HMC_outpw(dev,base_addr+15,AX_RDR_V);//RDR
	HMC_outpw(dev,base_addr+13,rmgdt);//RMG
	HMC_outpw(dev,base_addr+7,AX_REL_V);//REL
	HMC_outpw(dev,base_addr+9,AX_REH_V);//REH

	HMC_outpLw(dev,base_addr+23,linA);
	HMC_outpb(dev,base_addr+27,AX_PMD_V);//PMD
	HMC_outpw(dev,base_addr+21,AX_RMD_V2);//RMD

	if(WaitSYNC==0)
	{
		start=0;
		if(Axs1==0)start=start+1;
		else if(Axs1==1) start=start+2;
		else start=start+4;

		if(Axs2==0)start=start+1;
		else if(Axs2==1) start=start+2;
		else start=start+4;

		HMC_outpb(dev,3,start);//start
		if(Axs1==0)
		{
		 base_addr=20;	
		}
		else
		if(Axs1==1)
		{ 
		base_addr=50;
		} 
		else
		if(Axs1==2)
		{ 
		base_addr=80;
		} 
		else
		if(Axs1==3)
		{ 
		base_addr=110;
		} 
		HMC_outpb(dev,base_addr+1,0);
		if(Axs2==0)
		{
		 base_addr=20;	
		}
		else
		if(Axs2==1)
		{ 
		base_addr=50;
		} 
		else
		if(Axs2==2)
		{ 
		base_addr=80;
		} 
		else
		if(Axs2==3)
		{ 
		base_addr=110;
		}  
		HMC_outpb(dev,base_addr+1,0);
	}

	return erro;
}

  
int AXIS_Interpolation3(int dev,unsigned int Axs1,unsigned int Axs2,unsigned int Axs3,unsigned int curve,unsigned int Dir1,unsigned int Dir2,unsigned int Dir3,
									unsigned char Outmod,unsigned int Vo,unsigned int Vt,unsigned int Length1,unsigned int Length2,unsigned int Length3,unsigned int StartDec,
									unsigned int Acctime,unsigned int Dectime,unsigned int SD_EN,unsigned int WaitSYNC)
{ 
	unsigned int  Pos;unsigned char  RunState;unsigned char IOState;unsigned char  SyncIO;
	Read_Position(dev, Axs1,&Pos,&RunState,&IOState,&SyncIO);
	if(RunState) return -2;
		Read_Position(dev, Axs2,&Pos,&RunState,&IOState,&SyncIO);
	if(RunState) return -2;
		Read_Position(dev, Axs3,&Pos,&RunState,&IOState,&SyncIO);
	if(RunState) return -2;
	unsigned int AX_REL_V,AX_REH_V,rmgdt,AX_RMD_V1,AX_RMD_V2,AX_RMD_V3,AX_RDP_V;
	unsigned short AX_RUR_V;//加速率
	unsigned short AX_RDR_V;//减速率
	unsigned char  AX_PMD_V; //输出模式寄存器
	double buf,buf1,AX_RUR_S,AX_RDR_S,rdp,rup; 
	unsigned int A1=0,B1=0,C1=0,linA1=0;
	unsigned int A2=0,B2=0,C2=0,linA2=0;
	unsigned int A3=0,B3=0,C3=0,linA3=0;
	unsigned char start;
	unsigned int gongyue,temp;
	int erro=0;
	rmgdt=2999;	
	if(Vt>3276)rmgdt=1499;	//0.1倍放大模式   0.1--6553.5 参考时钟是12M 倍率=参考时钟/65536/（prmg+1) 
	if(Vt>6553)rmgdt=599;										    //0.2倍放大模式   0.2--13107                
	if(Vt>13107)rmgdt=299;										    //0.5倍放大模式   0.5--32767                
	if(Vt>32767)rmgdt=149;		 						  		    // 1倍放大模式
	if(Vt>65535)rmgdt=59;										    // 2倍放大模式
	if(Vt>131070)rmgdt=29;										    // 5倍放大模式
	if(Vt>327675)rmgdt=14;		 							   	    //10倍放大模式
	if(Vt>655350)rmgdt=5;		 								    //20倍放大模式
	if(Vt>1310700)rmgdt=2;		  								    //50倍放大模式
	if(Vt>3276750)rmgdt=1;		  								    //100倍放大模式
	Circular_Reset(dev);  //round reset
	AX_REL_V=Vo/(300.0/(rmgdt+1));	  							        //计算实际的FL速度
	AX_REH_V=Vt/(300.0/(rmgdt+1));	  							        //计算实际的FH速度
	buf= AX_REH_V-AX_REL_V;
	buf=buf/19660800.0;
	if(curve)
	{
		if(Dir1) //负方向
		AX_RMD_V1=0x04c1;
		else  //正方向
		AX_RMD_V1=0x0441;

		if(Dir2) //负方向
		AX_RMD_V2=0x04c1;
		else  //正方向
		AX_RMD_V2=0x0441;

		if(Dir3) //负方向
		AX_RMD_V3=0x04c1;
		else  //正方向
		AX_RMD_V3=0x0441;

		buf1=Acctime;
		AX_RUR_V=buf1/1000/buf/2;
		buf1=Dectime;
		AX_RDR_V=buf1/1000/buf/2; 
	}
	else
	{
		if(Dir1) //负方向
		AX_RMD_V1=0x00c1;
		else  //正方向
		AX_RMD_V1=0x0041;

		if(Dir2) //负方向
		AX_RMD_V2=0x00c1;
		else  //正方向
		AX_RMD_V2=0x0041;

		if(Dir3) //负方向
		AX_RMD_V3=0x00c1;
		else  //正方向
		AX_RMD_V3=0x0041;

		buf1=Acctime;
		AX_RUR_V=buf1/1000/buf;
		buf1=Dectime;
		AX_RDR_V=buf1/1000/buf; 
	}
	if(SD_EN)
	{
		AX_RMD_V1=AX_RMD_V1+0x0100;
		AX_RMD_V2=AX_RMD_V2+0x0100;
		AX_RMD_V3=AX_RMD_V3+0x0100;
	}

	if(Length1>Length2)
	{
		if(Length1>Length3)//Length1 最大
		{    
			AX_RMD_V2=AX_RMD_V2+0x0800;
			AX_RMD_V3=AX_RMD_V3+0x0800;

			linA2=Length1;
			A2=Length1/Length2;
			if(A2>65535) return -3; //插补轴步进太少

			C2=Length1%Length2;
			if(C2>0)
				B2=Length2/C2;
			else
			 B2=1; 
			linA3=Length1;
			A3=Length1/Length3;
			if(A3>65535) return -3; //插补轴步进太少

			C3=Length1%Length3;
			if(C3>0)
				B3=Length3/C3;
			else
				B3=1;
		}
		else//Length3 最大
		{
			A3=1;
			B3=1;
			AX_RMD_V2=AX_RMD_V2+0x0800;
			AX_RMD_V1=AX_RMD_V1+0x0800;

			linA2=Length3;
			A2=Length3/Length2;
			if(A2>65535) return -3; //插补轴步进太少

			C2=Length3%Length2;
			if(C2>0)
			B2=Length2/C2;
			else
			B2=1;

			linA1=Length3;
			A1=Length3/Length1;
			if(A1>65535) return -3; //插补轴步进太少

			C1=Length3%Length1;
			if(C1>0)
			B1=Length1/C1;
			else
			B1=1;
		}
	}
	else
	{
		if(Length2>Length3)//Length2 最大
		{    
			A2=1;
			B2=1;
			AX_RMD_V1=AX_RMD_V1+0x0800;
			AX_RMD_V3=AX_RMD_V3+0x0800;

			linA3=Length2;
			A3=Length2/Length3;
			if(A3>65535) return -3; //插补轴步进太少

			C3=Length2%Length3;
			if(C3>0)
				B3=Length3/C3;
			else
				B3=1;
			linA1=Length2;
			A1=Length2/Length1;
			if(A1>65535) return -3; //插补轴步进太少

			C1=Length2%Length1;
			if(C1>0)
				B1=Length1/C1;
			else
			B1=1;
		}
		else//Length3 最大
		{
			A3=1;
			B3=1;
			AX_RMD_V2=AX_RMD_V2+0x0800;
			AX_RMD_V1=AX_RMD_V1+0x0800;
			linA2=Length3;
			A2=Length3/Length2;
			if(A2>65535) return -3; //插补轴步进太少

			C2=Length3%Length2;
			if(C2>0)
				B2=Length2/C2;
			else
				B2=1;

			linA1=Length3;
			A1=Length3/Length1;
			if(A1>65535) return -3; //插补轴步进太少

			C1=Length3%Length1;
			if(C1>0)
				B1=Length1/C1;
			else
				B1=1;
		}
	}

	if(A1>0)A1=A1-1;
	if(B1>0) B1=B1-1;

	if(A2>0)A2=A2-1;
	if(B2>0) B2=B2-1;

	if(A3>0)A3=A3-1;
	if(B3>0) B3=B3-1;

	buf=AX_REH_V;
	buf=buf*buf;
	buf1=AX_REL_V;
	buf1=buf1*buf1;
	if(curve)//S
	{
		rup=(buf-buf1)*(AX_RUR_V+1)/(rmgdt+1)/65536;//系数待定  最佳脉冲位置 =（AX_REH_V×AX_REH_V-AX_REL_V×AX_REL_V）*(AX_RUR_V+1)/(rmgdt+1)/65536/4;
		rdp=(buf-buf1)*(AX_RDR_V+1)/(rmgdt+1)/65536;
	}
	else
	{
		rup=(buf-buf1)*(AX_RUR_V+1)/(rmgdt+1)/65536/2;//系数待定  最佳脉冲位置 =（AX_REH_V×AX_REH_V-AX_REL_V×AX_REL_V）*(AX_RUR_V+1)/(rmgdt+1)/65536/4;
		rdp=(buf-buf1)*(AX_RDR_V+1)/(rmgdt+1)/65536/2;
	}
	if((Length1>Length2)&&(Length1>Length3))//Length1 最大
	{
		if((rdp+rup+100)>Length1)
		{
			erro=-2;
			AX_RDP_V=Length1/2;
		}
		else
		{  
			if(StartDec==0)
				AX_RDP_V=Length1-(unsigned int)rdp;
			else
				AX_RDP_V=StartDec;
			erro=AX_RDP_V;
		}
	}
	else
	if((Length2>Length1)&&(Length2>Length3))//Length2 最大
	{
		if((rdp+rup+100)>Length2)
		{
			erro=-2;
			AX_RDP_V=Length2/2;
		}
		else
		{  
			if(StartDec==0)
				AX_RDP_V=Length2-(unsigned int)rdp;
			else
				AX_RDP_V=StartDec;
			erro=AX_RDP_V;
		}
	}
	else
	{
		if((rdp+rup+100)>Length3)
		{
			erro=-2;
			AX_RDP_V=Length3/2;
		}
		else
		{  
			if(StartDec==0)
				AX_RDP_V=Length3-(unsigned int)rdp;
			else
				AX_RDP_V=StartDec;
			erro=AX_RDP_V;
		}
	}

	AX_PMD_V= Outmod;
	unsigned short base_addr;
	if(Axs1==0)
	{
	 base_addr=20;		
	}
	else
	if(Axs1==1)
	{ 
		base_addr=50;
	} 
	else
	if(Axs1==2)
	{ 
		base_addr=80;
	} 
	else
	if(Axs1==3)
	{ 
		base_addr=110;
	} 

	if(HMC_outpb(dev,base_addr+1,0)==-1) return -1;
	HMC_outpLw(dev,base_addr+3,Length1);//RMV
	HMC_outpLw(dev,base_addr+17,AX_RDP_V);//RDP
	HMC_outpw(dev,base_addr+11,AX_RUR_V);//RUR
	HMC_outpw(dev,base_addr+15,AX_RDR_V);//RDR
	HMC_outpw(dev,base_addr+13,rmgdt);//RMG
	HMC_outpw(dev,base_addr+7,AX_REL_V);//REL
	HMC_outpw(dev,base_addr+9,AX_REH_V);//REH

	HMC_outpLw(dev,base_addr+23,linA1);//A
	HMC_outpb(dev,base_addr+27,AX_PMD_V);//PMD
	HMC_outpw(dev,base_addr+21,AX_RMD_V1);//RMD

	if(Axs2==0)
	{
		base_addr=20;	
	}
	else
	if(Axs2==1)
	{ 
		base_addr=50;
	} 
	else
	if(Axs2==2)
	{ 
		base_addr=80;
	} 
	else
	if(Axs2==3)
	{ 
		base_addr=110;
	} 

	if(HMC_outpb(dev,base_addr+1,0)==-1) return -1;
	HMC_outpLw(dev,base_addr+3,Length2);//RMV
	HMC_outpLw(dev,base_addr+17,AX_RDP_V);//RDP
	HMC_outpw(dev,base_addr+11,AX_RUR_V);//RUR
	HMC_outpw(dev,base_addr+15,AX_RDR_V);//RDR
	HMC_outpw(dev,base_addr+13,rmgdt);//RMG
	HMC_outpw(dev,base_addr+7,AX_REL_V);//REL
	HMC_outpw(dev,base_addr+9,AX_REH_V);//REH

	HMC_outpLw(dev,base_addr+23,linA2);//A
	HMC_outpb(dev,base_addr+27,AX_PMD_V);//PMD
	HMC_outpw(dev,base_addr+21,AX_RMD_V2);//RMD

	if(Axs3==0)
	{
	 base_addr=20;	 
	}
	else
	if(Axs3==1)
	{ 
		base_addr=50;
	} 
	else
	if(Axs3==2)
	{ 
		base_addr=80;
	} 
	else
	if(Axs3==3)
	{ 
		base_addr=110;
	} 

	if(HMC_outpb(dev,base_addr+1,0)==-1) return -1;
	HMC_outpLw(dev,base_addr+3,Length3);//RMV
	HMC_outpLw(dev,base_addr+17,AX_RDP_V);//RDP
	HMC_outpw(dev,base_addr+11,AX_RUR_V);//RUR
	HMC_outpw(dev,base_addr+15,AX_RDR_V);//RDR
	HMC_outpw(dev,base_addr+13,rmgdt);//RMG
	HMC_outpw(dev,base_addr+7,AX_REL_V);//REL
	HMC_outpw(dev,base_addr+9,AX_REH_V);//REH

	HMC_outpLw(dev,base_addr+23,linA3);//A
	HMC_outpb(dev,base_addr+27,AX_PMD_V);//PMD
	HMC_outpw(dev,base_addr+21,AX_RMD_V3);//RMD

	if(WaitSYNC==0)
	{
		start=0;
		start=start+(0x1<<Axs1);
		start=start+(0x1<<Axs2);
		start=start+(0x1<<Axs3);

		HMC_outpb(dev,3,start);//start
		base_addr=20+Axs1*30; 
		HMC_outpb(dev,base_addr+1,0);
		base_addr=20+Axs2*30; 
		HMC_outpb(dev,base_addr+1,0);
		base_addr=20+Axs3*30; 
		HMC_outpb(dev,base_addr+1,0);
	}

	return erro;
 }
 int  AXIS_Interpolation_Set(int dev,unsigned int Axs,unsigned int curve,unsigned int Dir,
				unsigned char Outmod,unsigned int Vo,unsigned int Vt,unsigned int Length,unsigned int LinA,unsigned int StartDec,
				unsigned int Acctime,unsigned int Dectime,unsigned int SD_EN,unsigned int Axs_Main)
{ 
	unsigned int  Pos;unsigned char  RunState;unsigned char IOState;unsigned char  SyncIO;
	Read_Position(dev, Axs,&Pos,&RunState,&IOState,&SyncIO);
	if(RunState){return -2;}

	unsigned int AX_REL_V,AX_REH_V,rmgdt,AX_RMD_V1,AX_RMD_V2,AX_RDP_V;
	unsigned short AX_RUR_V;//加速率
	unsigned short AX_RDR_V;//减速率
	unsigned char  AX_PMD_V; //输出模式寄存器
	double buf,buf1,AX_RUR_S,AX_RDR_S,rdp,rup; 

	unsigned char start;
	unsigned int gongyue,temp;
	int erro=0;
	rmgdt=2999;	
	if(Vt>3276)rmgdt=1499;	//0.1倍放大模式   0.1--6553.5 参考时钟是12M 倍率=参考时钟/65536/（prmg+1) 
	if(Vt>6553)rmgdt=599;										    //0.2倍放大模式   0.2--13107                
	if(Vt>13107)rmgdt=299;										    //0.5倍放大模式   0.5--32767                
	if(Vt>32767)rmgdt=149;		 						  		    // 1倍放大模式
	if(Vt>65535)rmgdt=59;										    // 2倍放大模式
	if(Vt>131070)rmgdt=29;										    // 5倍放大模式
	if(Vt>327675)rmgdt=14;		 							   	    //10倍放大模式
	if(Vt>655350)rmgdt=5;		 								    //20倍放大模式
	if(Vt>1310700)rmgdt=2;		  								    //50倍放大模式
	if(Vt>3276750)rmgdt=1;		  								    //100倍放大模式
	//Circular_Reset(&pSockClient);  //round reset
	AX_REL_V=Vo/(300.0/(rmgdt+1));	  							        //计算实际的FL速度
	AX_REH_V=Vt/(300.0/(rmgdt+1));	  							        //计算实际的FH速度
	buf= AX_REH_V-AX_REL_V;
	buf=buf/19660800.0;
	if(curve)
	{
		if(Dir) //负方向
			AX_RMD_V1=0x04c1;
		else  //正方向
			AX_RMD_V1=0x0441;
		
		buf1=Acctime;
		AX_RUR_V=buf1/1000/buf/2;
		buf1=Dectime;
		AX_RDR_V=buf1/1000/buf/2; 
		}
	else
	{
		if(Dir) //负方向
			AX_RMD_V1=0x00c1;
		else  //正方向
			AX_RMD_V1=0x0041;

		buf1=Acctime;
		AX_RUR_V=buf1/1000/buf;
		buf1=Dectime;
		AX_RDR_V=buf1/1000/buf; 
	}
	if(SD_EN)
	{
		AX_RMD_V1=AX_RMD_V1+0x0100;   
	}

	if(Axs_Main==0)
	{
		AX_RMD_V1=AX_RMD_V1+0x0800;	 	 
	}

	buf=AX_REH_V;
	buf=buf*buf;
	buf1=AX_REL_V;
	buf1=buf1*buf1;
	if(curve)//S
	{
		rup=(buf-buf1)*(AX_RUR_V+1)/(rmgdt+1)/65536;//系数待定  最佳脉冲位置 =（AX_REH_V×AX_REH_V-AX_REL_V×AX_REL_V）*(AX_RUR_V+1)/(rmgdt+1)/65536/4;
		rdp=(buf-buf1)*(AX_RDR_V+1)/(rmgdt+1)/65536;
	}
	else
	{
		rup=(buf-buf1)*(AX_RUR_V+1)/(rmgdt+1)/65536/2;//系数待定  最佳脉冲位置 =（AX_REH_V×AX_REH_V-AX_REL_V×AX_REL_V）*(AX_RUR_V+1)/(rmgdt+1)/65536/4;
		rdp=(buf-buf1)*(AX_RDR_V+1)/(rmgdt+1)/65536/2;
	}
	if(Axs_Main)
	{
		if((rdp+rup+100)>Length)
		{
			erro=-2;
			AX_RDP_V=Length/2;
		}
		else
		{  
			if(StartDec==0)
				AX_RDP_V=Length-(unsigned int)rdp;
			else
				AX_RDP_V=StartDec;
			erro=AX_RDP_V;
		}
	}
	else
	{
		if((rdp+rup+100)>LinA)
		{
			erro=-2;
			AX_RDP_V=LinA/2;
		}
		else
		{  
			if(StartDec==0)
				AX_RDP_V=LinA-(unsigned int)rdp;
			else
				AX_RDP_V=StartDec;
			erro=AX_RDP_V;
		}
	}
	AX_PMD_V= Outmod;
	unsigned short base_addr;

	if(Axs<0)Axs=0;
	else
	if(Axs>5)Axs=5;  
	base_addr=20+Axs*30;

	if(HMC_outpb(dev,base_addr+1,0)==-1) {return -1;}
	HMC_outpLw(dev,base_addr+3,Length);//RMV
	HMC_outpLw(dev,base_addr+17,AX_RDP_V);//RDP
	HMC_outpw(dev,base_addr+11,AX_RUR_V);//RUR
	HMC_outpw(dev,base_addr+15,AX_RDR_V);//RDR
	HMC_outpw(dev,base_addr+13,rmgdt);//RMG
	HMC_outpw(dev,base_addr+7,AX_REL_V);//REL
	HMC_outpw(dev,base_addr+9,AX_REH_V);//REH

	HMC_outpLw(dev,base_addr+23,LinA);//A

	HMC_outpb(dev,base_addr+27,AX_PMD_V);//PMD 
	HMC_outpw(dev,base_addr+21,AX_RMD_V1);//RMD

	return erro;
 }
 int AxsSyncStart(int dev,unsigned int Axs1,unsigned int Axs2,unsigned int Axs3,unsigned int Axs4)
 {
	unsigned char start;
	start=0;
	// HMC_outpb(dev,216,0);//round reset
	if(Axs1==1)start=start+1;

	if(Axs2==1)start=start+2;
	if(Axs3==1) start=start+4;
	if(Axs4==1) start=start+8;
	// if(Axs5==1) start=start+16;
	//      if(Axs6==1) start=start+32;

	if(HMC_outpb(dev,3,start)==-1) {return -1;}//start
	if(Axs1==1) HMC_outpb(dev,21,0);//start to 0
	if(Axs2==1) HMC_outpb(dev,51,0);//start to 0
	if(Axs3==1) HMC_outpb(dev,81,0);//start to 0
	if(Axs4==1) HMC_outpb(dev,111,0);//start to 0
	//    if(Axs5==1) HMC_outpb(dev,141,0);//start to 0
	//     if(Axs6==1) HMC_outpb(dev,171,0);//start to 0

	return 0;
}
int AxsSyncStop(int dev,unsigned int Axs1,unsigned int Axs2,unsigned int Axs3,unsigned int Axs4)
{
	unsigned char stop;
	stop=0;
	HMC_outpb(dev,216,0);//round reset
	if(Axs1==1)stop=stop+1;

	if(Axs2==1)stop=stop+2;
	if(Axs3==1) stop=stop+4;
	if(Axs4==1) stop=stop+8;

	if(HMC_outpb(dev,7,stop)==-1) return -1;//stop
	if(Axs1==1) HMC_outpb(dev,22,0);//stop to 0
	if(Axs2==1) HMC_outpb(dev,52,0);//stop to 0
	if(Axs3==1) HMC_outpb(dev,82,0);//stop to 0
	if(Axs4==1) HMC_outpb(dev,112,0);//stop to 0
	return 0;
}

   
int Set_Encorder(int dev,int Axs,int mod,int z_reset_en,int z_dir,int set8000,int enable)
{ 
	unsigned char zreset,set8,modx,outBuf;

	if(mod<0)modx=0;
	else
	if(mod>7)modx=7;
	else
	modx=(unsigned char )mod;

	if(z_reset_en)
		zreset=0x08;
	else
		zreset=0x00;
	if(set8000)
		set8=0x10;
	else
		set8=0x00;

	outBuf =modx+zreset+set8; 
	if(z_dir)
		outBuf=outBuf+0x20;

	if(Axs==0)
	{
		HMC_outpb(dev,10,outBuf);	  
	}
	else
	if(Axs==1)
	{
		HMC_outpb(dev,11,outBuf);	  
	}
	else
	if(Axs==2)
	{
		HMC_outpb(dev,12,outBuf);
	}
	if(Axs==3)
	{
		HMC_outpb(dev,13,outBuf);
	}

	if(enable)
	{
		if(Axs==0)
		{
			HMC_outpb(dev,9,1);      
		}
		else
		if(Axs==1)
		{
			HMC_outpb(dev,9,2);     
		}
		else
		if(Axs==2)
		{
			HMC_outpb(dev,9,4);
		}
		else
		if(Axs==3)
		{
			HMC_outpb(dev,9,8);
		}
	}
	else
	{
		if(Axs==0)
		{
		HMC_outpb(dev,8,1);      
		}
		else
		if(Axs==1)
		{
			HMC_outpb(dev,8,2);     
		}
		else
		if(Axs==2)
		{
		HMC_outpb(dev,8,4);
		}
		else
		if(Axs==3)
		{
		HMC_outpb(dev,8,8);
		}
	}
	return 0;
}
unsigned int Read_Encorder(int dev,int Axs)
{
	unsigned int Cnt;
	if(Axs==0)
	{
		Cnt=HMC_inpLw(dev,80);
	}
	else
	if(Axs==1)
	{
		Cnt=HMC_inpLw(dev,84);
	}
	else if(Axs==2)
	{
		Cnt=HMC_inpLw(dev,88);
	}
	else if(Axs==3)
	{
		Cnt=HMC_inpLw(dev,92);
	}
	else if(Axs==4)
	{
		Cnt=HMC_inpLw(dev,96);
	}
	else if(Axs==5)
	{
		Cnt=HMC_inpLw(dev,100);
	}
	return Cnt;
}
int Circular_Busy(int dev)
{
	unsigned char busy;
	busy=HMC_inpb(dev,107);
	if(busy)
		return 1;
	else
		return 0;
 }

int Circular_Reset(int dev)
{
	HMC_outpb(dev,216,0);
	HMC_outpb(dev,20+27,0);
	HMC_outpb(dev,50+27,0);
	HMC_outpb(dev,80+27,0);
	HMC_outpb(dev,110+27,0);
	return 0;
}
int Circular_Interpolation(int dev,unsigned int Axs1,unsigned int Axs2,unsigned char Outmod,unsigned int Vt,int xi,int yi,int xe,int ye,int foward,int continu)
{
	unsigned int  Pos;unsigned char  RunState;unsigned char IOState;unsigned char  SyncIO;
	Read_Position(dev, Axs1,&Pos,&RunState,&IOState,&SyncIO);
	if(RunState) return -2;
	Read_Position(dev, Axs2,&Pos,&RunState,&IOState,&SyncIO);
	if(RunState) return -2;
	if(Axs1>2)return -3;
	unsigned int AX_REL_V=1000,AX_REH_V,rmgdt,AX_RMD_V;
	unsigned char  AX_PMD_V; //输出模式寄存器
	int erro=0;
	////round  set
	HMC_outpb(dev,216,0);//reset
	if(Axs1<0)Axs1=0;
	else
	if(Axs1>5)Axs1=5;

	if((xi>0x00ffffff)||(xi<-0x00ffffff)||(yi>0x00ffffff)||(yi<-0x00ffffff)||(xe>0x00ffffff)||(xe<-0x00ffffff)||(ye>0x00ffffff)||(ye<-0x00ffffff)) return -4;
	HMC_outpLw(dev,200,xi);//xi
	HMC_outpLw(dev,204,yi);//yi  
	HMC_outpLw(dev,208,xe );//xe  
	HMC_outpLw(dev,212,ye );//ye  

	unsigned char roudset;
	if(foward)
		roudset=0x10;
	else
		roudset=0x11; 
	roudset=roudset+((unsigned char)Axs1<<1);

	HMC_outpb(dev,216,roudset);
	if(continu)
		HMC_outpb(dev,217,1);   
	else
		HMC_outpb(dev,217,0);

	rmgdt=2999;	
	if(Vt>3276)rmgdt=1499;	//0.1倍放大模式   0.1--6553.5 参考时钟是12M 倍率=参考时钟/65536/（prmg+1) 
	if(Vt>6553)rmgdt=599;										    //0.2倍放大模式   0.2--13107                
	if(Vt>13107)rmgdt=299;										    //0.5倍放大模式   0.5--32767                
	if(Vt>32767)rmgdt=149;		 						  		    // 1倍放大模式
	if(Vt>65535)rmgdt=59;										    // 2倍放大模式
	if(Vt>131070)rmgdt=29;										    // 5倍放大模式
	if(Vt>327675)rmgdt=14;		 							   	    //10倍放大模式
	if(Vt>655350)rmgdt=5;		 								    //20倍放大模式
	if(Vt>1310700)rmgdt=2;		  								    //50倍放大模式
	if(Vt>3276750)rmgdt=1;		  								    //100倍放大模式


	//	AX_REL_V=Vo/(300.0/(rmgdt+1));	  							        //计算实际的FL速度
	AX_REH_V=Vt/(300.0/(rmgdt+1));	  							        //计算实际的FH速度
	// if(Dir) //负方向
	//	AX_RMD_V=0x0083;
	//  else  //正方向
	AX_RMD_V=0x0003;

	//  if(SD_EN)AX_RMD_V=AX_RMD_V+0x0100;

	unsigned short base_addr;
	if(Axs2==0)
	{
		base_addr=20;	
	}
	else
	if(Axs2==1)
	{ 
		base_addr=50;
	} 
	else if(Axs2==2)
	{ 
		base_addr=80;
	} 
	else if(Axs2==3)
	{ 
		base_addr=110;
	} 
	AX_PMD_V= Outmod+0x20;//roudy
	HMC_outpb(dev,base_addr+27,AX_PMD_V);//PMD

	if(Axs1==0)
	{
		base_addr=20;	
	}
	else
	if(Axs1==1)
	{ 
		base_addr=50;
	} 
	else if(Axs1==2)
	{ 
		base_addr=80;
	} 
	else if(Axs1==3)
	{ 
		base_addr=110;
	} 
	if(HMC_outpb(dev,base_addr+1,0)==-1) return -1;
	//// HMC_outpLw(dev,base_addr+3,Length);//RMV
	//HMC_outpLw(dev,base_addr+17,AX_RDP_V);//RDP
	//HMC_outpw(dev,base_addr+11,AX_RUR_V);//RUR
	//HMC_outpw(dev,base_addr+15,AX_RDR_V);//RDR
	HMC_outpw(dev,base_addr+13,rmgdt);//RMG
	HMC_outpw(dev,base_addr+7,AX_REL_V);//REL
	HMC_outpw(dev,base_addr+9,AX_REH_V);//REH

	AX_PMD_V= Outmod+0x10;//roudx
	HMC_outpb(dev,base_addr+27,AX_PMD_V);//PMD

	HMC_outpw(dev,base_addr+21,AX_RMD_V);//RMD

	HMC_outpb(dev,base_addr+1,1);//start
	HMC_outpb(dev,base_addr+1,0);//start to 0
	return erro;
 }
