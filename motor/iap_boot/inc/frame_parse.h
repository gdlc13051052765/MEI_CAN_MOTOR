#ifndef __FRAME_PARSE_H
#define __FRAME_PARSE_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_MASS_LOST_LEVEL			5			//5%
#define LOST_MAX_LEN		10			//如果修改此值，需要修改初始化
#define LOST_INIT() {0, 0}


typedef struct
{
	uint8_t state;
	uint16_t frame_id;
}_Lost_Pkg, *_pLost_Pkg;

typedef struct
{
	uint8_t lost_pkg_num;		//丢失总包数据
	_pLost_Pkg precord_lost;	//记录丢失包信息
}_Record_Lost,_pRecord_Lost;




#define FRAME_LENGTH 				106//一帧数据最大长度


#define BigLittle16_t(Value)    (uint16_t)((Value>>8)|(Value<<8))
#define BigLittle32_t(Value)    ((Value&0xFF000000)>>24)|((Value&0x00FF0000)>>8)|((Value&0x0000FF00)<<8)|((Value&0x000000FF)<<24)

#pragma pack(1)
typedef  union
{
  struct
  {
    uint16_t FrameStep;         //帧步进值
    uint16_t AppTotalPackage;   //总包数
    uint8_t  FrameType;         //帧数据域区分  0x01：MS升级包   0x02:SL升级包  0x03:CC升级包
    uint8_t  AppVer;            //APP版本
    uint8_t  nAppVer;           //APP版本取反
		uint8_t  HardVer;           //硬件版本
    uint8_t  nHardVer;          //硬件版本取反
    uint32_t AppStartAddr;      //app起始地址
    uint32_t AppCrc32;          //app源码CRC32校验
    uint16_t AppYear;           //app编译年
    uint8_t  AppMomth;          //app编译月
    uint8_t  AppDay;            //app编译日
    uint8_t  AppHour;           //app编译时
    uint8_t  AppMinute;         //app编译分
    uint8_t  AppSolvePrb;       //app修正问题
    uint8_t  AppIsEncry;        //帧加密标志位
    uint8_t  FrameEncryKey[64]; //帧加解密密钥
		uint16_t  machine_type;			//机器种类码
		uint8_t  board_type;				//板子类型
    uint8_t  FrameRet[12];  		//帧保留
    uint16_t CRC16;             //帧CRC32校验
  }MsgFrame;
  
  struct
  {
    uint16_t FrameStep;         //帧步进值
    uint8_t  FrameType;         //帧类型 
		uint8_t  SoftVer;						//软件版本
    uint8_t  AppBuff[FRAME_LENGTH-6];    	//APP固件
    uint16_t CRC16;             //CRC16
  }DataFrame;
  
  uint8_t FrameBuff[FRAME_LENGTH];
}BootAppFrame,*pBootAppFrame;
#pragma pack ()
 
uint8_t find_firmware_lost_step(void *instance, uint16_t *fw_id);
uint8_t msg_frame_check(void *instance);
uint8_t msg_frame_compare(const uint8_t *src_buff, const uint8_t *rev_buff, uint8_t len);
uint8_t firmware_data_check(void *instance, uint16_t *fw_id);
uint8_t check_firmware_lost(void *instance, uint16_t *fw_id);
#endif
