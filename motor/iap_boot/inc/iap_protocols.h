#ifndef __IAP_PROTOCOLS_H
#define __IAP_PROTOCOLS_H


#include "ex_flash.h"
#include "st_flash.h"
#include "frame_parse.h"


#define FRAME_MAX_LENS						106	//一帧数据最大长度
#define   TO_IAP_FLAG		 	 				0x12345678 
 
  
typedef enum
{
	ret_ok = 0,		//执行成功
	 
	ret_rev_pkg_id = 0x10,	//断点续传，断点位置包ID
	ret_need_pkg = 0x20,	//需要的包序号
	ret_flash_lost = 0xFE,	//存储器丢失或者不可读写
	ret_info_error = 0x11,	//信息帧校验错误
	ret_data_error = 0x12,	//数据帧校验错误
	ret_ver_error = 0xEB,		//版本信息不一致，重新开始
	ret_check_error = 0xEE,	//校验接收的固件错误
	ret_mass_lost = 0xFB,		//丢包率过高，重新开始
	ret_app_lost = 0xFF,		//运行固件不完整，加载指定的APP运行
	
}_Iap_Ret;

typedef enum
{
	iap_null = 0,	//未开始升级
	iap_info_state,	//信息帧接收完成
	iap_data_state,	//数据帧接收
	iap_check_complete, //校验完成
}_Update_State;

	
typedef struct
{
		//全局变量地址
 const  uint32_t boot_msg_addr;		//boot信息区
 const	uint32_t app_run_addr;		//app运行地址
 const	uint32_t app_cache_addr;	//app缓存地址
	_Update_State update_state;				//升级状态  0：未开始升级   1：信息帧接收完成  2：数据帧接收完成  3：校验完成
	uint8_t is_valid_fw_msg;		//缓存中的信息帧是否有效  0:没有接收到有效的信息帧  1：已经接收到有效的信息帧
	
	uint16_t frame_step;				//当前接收帧的ID号
	BootAppFrame fw_msg;	//信息帧内容
	
	const _Flash_Func *w_flash;	//写flash接口
	const _Flash_Func *r_flash;	//读取flash接口
	const _Flash_Func *msg_flash;	//读取flash接口
}_Iap_Param, *_pIap_Param;


typedef struct
{
	void (*init)(void);
	uint8_t (*info_opt)(const uint8_t*, uint16_t *);		//信息帧处理 固定 106字节
	uint8_t (*data_opt)(const uint8_t*, uint16_t *);	//数据帧处理 固定 106字节
	uint8_t (*patch_opt)(const uint8_t*, uint16_t *);	//补包处理 固定 106字节
	uint8_t (*check_opt)(uint16_t *);								//校验包完整性
	uint8_t (*reset_opt)(void);		//系统复位
}_Iap_Func, *_pIap_Func;


extern _pIap_Func pIap_Func;

#endif
