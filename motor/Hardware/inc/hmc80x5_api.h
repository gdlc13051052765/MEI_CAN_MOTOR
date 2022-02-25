 
#ifndef __hmc80x5_H
#define __hmc80x5_H
 
#include "stm32f1xx_hal.h"
#include "crc.h"

 
void GPIO_Write(GPIO_TypeDef* GPIOx,unsigned char PinState);
void GPIO_WriteBit(GPIO_TypeDef* GPIOx,uint16_t GPIO_PIN,unsigned char PinState);
uint32_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);
void HMC_IO_init(void);
int  Read_Position(int dev,unsigned int Axs,unsigned int* Pos,unsigned char* RunState,unsigned char* IOState,unsigned char* SyncIO);
int  SYNC_Start(int dev);
int  SYNC_Stop(int dev);
int   Set_Axs(int dev,unsigned int Axs,unsigned int Run_EN,unsigned int Csta_EN,unsigned int Cstp_EN,unsigned int Csd_EN);
int   AxsStop(int dev,unsigned int Axs);
int   MovToOrg(int dev,unsigned int Axs,unsigned int Dir,unsigned char Outmod,unsigned int Speed);
int   FL_ContinueMov(int dev,unsigned int Axs,unsigned int Dir,unsigned char Outmod,unsigned int Vo,unsigned int Vt);
 
int   FH_ContinueMov(int dev,unsigned int Axs,unsigned int Dir,unsigned char Outmod,unsigned int Vo,unsigned int Vt,unsigned int SD_EN);
int   FH_ContinueAdjustSpeed(int dev,unsigned int Axs,unsigned int Vo,unsigned int Vt);
int  DeltMov(int dev,unsigned int Axs,unsigned int curve,unsigned int Dir,unsigned char Outmod,unsigned int Vo,unsigned int Vt,unsigned int Length,unsigned int StartDec,unsigned int Acctime,unsigned int Dectime,unsigned int SD_EN,unsigned int WaitSYNC);
int  Read_EndAccP(int dev,unsigned int* AccP);
int  Read_Speed(int dev,unsigned int Axs,unsigned int* speed);
int    AXIS_Interpolation2(int dev,unsigned int Axs1,unsigned int Axs2,unsigned int curve,unsigned int Dir1,unsigned int Dir2,
									unsigned char Outmod,unsigned int Vo,unsigned int Vt,unsigned int Length1,unsigned int Length2,unsigned int StartDec,
									unsigned int Acctime,unsigned int Dectime,unsigned int SD_EN,unsigned int WaitSYNC);
 
int    AXIS_Interpolation3(int dev,unsigned int Axs1,unsigned int Axs2,unsigned int Axs3,unsigned int curve,unsigned int Dir1,unsigned int Dir2,unsigned int Dir3,
									unsigned char Outmod,unsigned int Vo,unsigned int Vt,unsigned int Length1,unsigned int Length2,unsigned int Length3,unsigned int StartDec,
									unsigned int Acctime,unsigned int Dectime,unsigned int SD_EN,unsigned int WaitSYNC);
int    AXIS_Interpolation_Set(int dev,unsigned int Axs,unsigned int curve,unsigned int Dir,
				unsigned char Outmod,unsigned int Vo,unsigned int Vt,unsigned int Length,unsigned int LinA,unsigned int StartDec,
				unsigned int Acctime,unsigned int Dectime,unsigned int SD_EN,unsigned int Axs_Main);
int    AxsSyncStart(int dev,unsigned int Axs1,unsigned int Axs2,unsigned int Axs3,unsigned int Axs4);
int   AxsSyncStop(int dev,unsigned int Axs1,unsigned int Axs2,unsigned int Axs3,unsigned int Axs4);
int  Set_Encorder(int dev,int Axs,int mod,int z_reset_en,int z_dir,int set8000,int enable);
unsigned int   Read_Encorder(int dev,int Axs);
int     Circular_Interpolation(int dev,unsigned int Axs1,unsigned int Axs2,unsigned char Outmod,unsigned int Vt,int xi,int yi,int xe,int ye,int foward,int continu);
int     Circular_Reset(int dev);
int     Circular_Busy(int dev);
#endif