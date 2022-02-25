#ifndef  __BOARD_INFO_H
#define  __BOARD_INFO_H

#define  ST_FLASH_CACHE_ADDR					0x08008C00		//内部FLASH缓存地址
#define  EX_FLASH_CACHE_ADDR					0							//外部缓存地址

/*==================================================================================
* 功能说明： 此文件为mcu_tools.exe解析程序的配置入口
* 备    注： 每一项宏定义的值，必须要用()括起来，为了方便解析，名称最好不要变动，否则需要修改工具
* 作    者： xiaozh
* 创建时间： 2021-01-05 113517
==================================================================================*/
#define  SOFT_VERSION					(0x02)		//软件版本 1个字节
#define  HARD_VERSION					(0x02)		//硬件版本 1个字节
#define  MACHINE_TYPE					(0x06)		//机器种类码 2个字节
#define  BOARD_TYPE						(0x05)		//板卡类型 1个字节

/*==========================FLASH地址分配表=========================*/
#define   APP_START_ADDR			0x08003000					//用户程序起始地址
#define   APP_CACHE_ADDR			EX_FLASH_CACHE_ADDR					//升级程序缓存地址
#define   IAP_FLAG_ADDR				0x08002000					//升级配置参数地址


#endif
