#include "crc.h"
#include "frame_parse.h"
#include "iap_protocols.h"
#include "st_crc.h"


static _Lost_Pkg lost_pkg_record[LOST_MAX_LEN] = 
{
	LOST_INIT(),LOST_INIT(),
	LOST_INIT(),LOST_INIT(),
	LOST_INIT(),LOST_INIT(),
	LOST_INIT(),LOST_INIT(),
	LOST_INIT(),LOST_INIT(),  
};

static _Record_Lost record_lost_param = 
{
	.lost_pkg_num = 0,
	.precord_lost = lost_pkg_record,
};

#define LOST_RECORD(id) do{uint16_t frame_id = id;\
														if(fw_ids != 0) {fw_ids = id;} \
														if(record_lost_param.lost_pkg_num<LOST_MAX_LEN)\
															{record_lost_param.precord_lost[record_lost_param.lost_pkg_num].state=0x01;\
															 record_lost_param.precord_lost[record_lost_param.lost_pkg_num].frame_id=frame_id;}\
																record_lost_param.lost_pkg_num++;}while(0)
#define GET_LOST_RECORD() (record_lost_param.lost_pkg_num)
 											
/*==================================================================================
* 函 数 名： get_one_lost_record
* 参    数： None
* 功能描述:  获取丢失帧ID
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2021-01-08 045307
==================================================================================*/																
uint8_t get_one_lost_record(void *instance, uint16_t *fw_id)
{
	uint8_t ret = 0;
	uint16_t fw_ids = 0;
 
	//如果当前缓存中有数据直接从缓存中取，否则更新缓存数据
	if(record_lost_param.lost_pkg_num == 0)
	{
		ret = find_firmware_lost_step(instance, &fw_ids);
		if(ret == ret_rev_pkg_id)
		{
			//需要的包ID
			*fw_id = fw_ids;
			return ret_need_pkg;
		}
		else
		{
			return ret;
		}
	}
	
	for(int i=0; i<LOST_MAX_LEN; i++)
	{
		if(record_lost_param.precord_lost[record_lost_param.lost_pkg_num].state)
		{
//			record_lost_param.precord_lost[i].state = 0;
//			(record_lost_param.lost_pkg_num>0)?(record_lost_param.lost_pkg_num--):(record_lost_param.lost_pkg_num=0);
			return record_lost_param.precord_lost[i].frame_id;			
		}
	}
	
	return 0;
}

/*==================================================================================
* 函 数 名： get_one_lost_record
* 参    数： None
* 功能描述:  获取丢失帧ID
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2021-01-08 045307
==================================================================================*/																
uint16_t clear_lost_record(uint16_t frame_id) 
{
	for(int i=0; i<LOST_MAX_LEN; i++)
	{
		if(record_lost_param.precord_lost[record_lost_param.lost_pkg_num].frame_id == frame_id)
		{
			record_lost_param.precord_lost[i].state = 0;
			record_lost_param.precord_lost[i].frame_id = 0;
			(record_lost_param.lost_pkg_num>0)?(record_lost_param.lost_pkg_num--):(record_lost_param.lost_pkg_num=0);
			return 0;			
		}
	}
	
	return 0;
}

/*==================================================================================
* 函 数 名： msg_frame_parse
* 参    数： None
* 功能描述:  信息帧解析
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2019-12-09 151546
==================================================================================*/ 
uint8_t msg_frame_check(void *instance)
{
	_pIap_Param pthis = instance; 
	 
	if((pthis->fw_msg.MsgFrame.FrameStep == 0x00) && (crc16_ccitt(pthis->fw_msg.FrameBuff, FRAME_LENGTH-2) == pthis->fw_msg.MsgFrame.CRC16))
	{
		return 0x01;
	}
	else
	{
		return 0;
	}
}

/*==================================================================================
* 函 数 名： firmware_data_check
* 参    数： fw_cache_addr: 缓存固件的起始地址
* 功能描述:  校验接收固件的完整性
* 返 回 值： None
* 备    注： 只校验 id 版本 crc16  crc32
* 作    者： xiaozh
* 创建时间： 2021-01-08 015246
==================================================================================*/
uint8_t firmware_data_check(void *instance, uint16_t *fw_id)
{
	uint8_t read_error = 0;	//读取存储错误
	uint32_t fw_src_crc32 = 0;	//源码CRC32校验
	uint8_t fw_cache[FRAME_LENGTH+1] = {0};
	uint16_t cur_pkg_step = 0; 
	uint32_t base_addr = 0; //起始地址
	uint64_t read_fw_addr =0;
	_pIap_Param pthis = instance; 
	pBootAppFrame pmsg = (pBootAppFrame)fw_cache;
	 
	base_addr = pthis->app_cache_addr;
	
	//进行数据解析
	while(cur_pkg_step < pthis->fw_msg.MsgFrame.AppTotalPackage)
	{
		cur_pkg_step++;
		read_fw_addr = base_addr+(cur_pkg_step*FRAME_LENGTH);
		
		if(pthis->r_flash->read(read_fw_addr, fw_cache, FRAME_LENGTH))
		{
			read_error = 0;
			//CRC校验&帧ID校验
			if(pmsg->DataFrame.CRC16 == crc16_ccitt(fw_cache,FRAME_LENGTH-2))   //校验数据帧的完整性
			{
				//校验版本
				if(pthis->fw_msg.MsgFrame.AppVer != pmsg->DataFrame.SoftVer)
				{
						//写入固件校验错误原因
					*fw_id = pmsg->DataFrame.FrameStep;
					return  ret_rev_pkg_id;
				//	return ret_ver_error;
				}
				
				if(pmsg->DataFrame.FrameStep != cur_pkg_step)
				{
					//写入固件校验错误原因
					*fw_id = pmsg->DataFrame.FrameStep;
					return  ret_rev_pkg_id;
				}
				 
				//计算CRC
				fw_src_crc32 = st_crc32((fw_cache+4), (FRAME_LENGTH));
			}
			else
			{
				*fw_id = cur_pkg_step;
				return  ret_rev_pkg_id;
			}
		}
		else
		{ 
			read_error++;
			if(read_error >= 3)
			{ 
				return  ret_flash_lost;
			}
		}
	}
	
	//进行全局校验
	if(fw_src_crc32 != pthis->fw_msg.MsgFrame.AppCrc32)	
	{
		//全局校验失败，写入失败原因
		
		return ret_check_error;
	}
	
	*fw_id = pmsg->DataFrame.FrameStep; //记录当前接收包数
	return ret_ok;
}


/*==================================================================================
* 函 数 名： find_firmware_lost_step
* 参    数： 
* 功能描述:  查找丢失固件包ID
* 返 回 值： None
* 备    注： 只校验 id 版本 crc16  crc32
* 作    者： xiaozh
* 创建时间： 2021-01-08 015246
==================================================================================*/
/*
static uint8_t find_firmware_lost_step(void *instance, uint16_t *fw_id)
{  
	uint16_t fw_ids = 0;
	uint8_t read_error = 0;	//读取存储错误
	uint32_t fw_src_crc32 = 0;	//源码CRC32校验
	uint8_t fw_cache[FRAME_LENGTH+1] = {0};
	uint16_t cur_pkg_step = 0; 
	uint32_t base_addr = 0; //起始地址
	uint64_t read_fw_addr =0;
	_pIap_Param pthis = instance; 
	pBootAppFrame pmsg = (pBootAppFrame)fw_cache;

	base_addr = pthis->app_cache_addr;

	
	do{
		read_error = 0;
		cur_pkg_step++;
		read_fw_addr = base_addr+(cur_pkg_step*FRAME_LENGTH);
		
		if(pthis->r_flash->read(read_fw_addr, fw_cache, FRAME_LENGTH))
		{
			read_error = 0;
			//CRC校验&帧ID校验
			if(pmsg->DataFrame.CRC16 == crc16_ccitt(fw_cache,FRAME_LENGTH-2))   //校验数据帧的完整性
			{
				//校验版本
				if(pthis->fw_msg.MsgFrame.AppVer != pmsg->DataFrame.SoftVer)
				{
					LOST_RECORD(pmsg->DataFrame.FrameStep); 
					continue; //继续查找
				}
				
				//校验帧区分
				if(pthis->fw_msg.MsgFrame.board_type != pmsg->DataFrame.FrameType)
				{
					LOST_RECORD(pmsg->DataFrame.FrameStep); 
					continue; //继续查找
				}
				
				if(pmsg->DataFrame.FrameStep != cur_pkg_step)
				{
					LOST_RECORD(pmsg->DataFrame.FrameStep); 
					continue; //继续查找
				}
				  
			}
			else
			{
				LOST_RECORD(pmsg->DataFrame.FrameStep); 
				continue; //继续查找
			}
		}
		else
		{ 
			read_error++;
			if(read_error >= 3)
			{ 
				return  ret_flash_lost;
			}
		}
	}while(cur_pkg_step < (pthis->fw_msg.MsgFrame.AppTotalPackage-1));

	if(GET_LOST_RECORD() != 0)	//丢包
	{
		*fw_id = fw_ids;
		return ret_rev_pkg_id;
	}
 
	return ret_ok;
}
*/

/*==================================================================================
* 函 数 名： find_firmware_lost_step
* 参    数： 
* 功能描述:  查找丢失固件包ID
* 返 回 值： None
* 备    注： 只校验 id 版本 crc16  crc32
* 作    者： xiaozh
* 创建时间： 2021-01-08 015246
==================================================================================*/
uint8_t find_firmware_lost_step(void *instance, uint16_t *fw_id)
{
	uint16_t fw_ids = 0;
	uint8_t read_error = 0;	//读取存储错误
	uint32_t fw_src_crc32 = 0;	//源码CRC32校验
	uint8_t fw_cache[FRAME_LENGTH+1] = {0};
	uint16_t cur_pkg_step = 0; 
	uint32_t base_addr = 0; //起始地址
	uint64_t read_fw_addr =0;
	_pIap_Param pthis = instance; 
	pBootAppFrame pmsg = (pBootAppFrame)fw_cache;

	base_addr = pthis->app_cache_addr;

	do{
		read_error = 0;
		cur_pkg_step++;
		read_fw_addr = base_addr+(cur_pkg_step*FRAME_LENGTH);
		
		if(pthis->r_flash->read(read_fw_addr, fw_cache, FRAME_LENGTH))
		{
			read_error = 0;
			//CRC校验&帧ID校验
			if(pmsg->DataFrame.CRC16 == crc16_ccitt(fw_cache,FRAME_LENGTH-2))   //校验数据帧的完整性
			{
				//校验版本
				if(pthis->fw_msg.MsgFrame.AppVer != pmsg->DataFrame.SoftVer)
				{
					*fw_id = cur_pkg_step; //不能使用读取的数据
					return ret_need_pkg;
				}
				
				//校验帧区分
				if(pthis->fw_msg.MsgFrame.board_type != pmsg->DataFrame.FrameType)
				{
					*fw_id = cur_pkg_step; //不能使用读取的数据
					return ret_need_pkg;
				}
				
				if(pmsg->DataFrame.FrameStep != cur_pkg_step) //包不连续
				{
					*fw_id = cur_pkg_step; //不能使用读取的数据
					return ret_need_pkg;
				}
			}
			else
			{
				*fw_id = cur_pkg_step; //不能使用读取的数据
				return ret_need_pkg;
			}
		}
		else
		{ 
			read_error++;
			if(read_error >= 3)
			{ 
				return  ret_flash_lost;
			}
		}
	}while(cur_pkg_step < (pthis->fw_msg.MsgFrame.AppTotalPackage-1));

	return ret_ok;
}

/*==================================================================================
* 函 数 名： firmware_data_check
* 参    数： fw_cache_addr: 缓存固件的起始地址
* 功能描述:  校验接收固件的完整性
* 返 回 值： None
* 备    注： 只校验 id 版本 crc16  crc32
* 作    者： xiaozh
* 创建时间： 2021-01-08 015246
==================================================================================*/
uint8_t check_firmware_lost(void *instance, uint16_t *fw_id)
{  
	uint16_t fw_ids = 0;
	uint8_t read_error = 0;	//读取存储错误
	uint32_t fw_src_crc32 = 0;	//源码CRC32校验
	uint8_t fw_cache[FRAME_LENGTH+1] = {0};
	uint16_t cur_pkg_step = 0; 
	uint32_t base_addr = 0; //起始地址
	uint64_t read_fw_addr =0;
	_pIap_Param pthis = instance; 
	pBootAppFrame pmsg = (pBootAppFrame)fw_cache;

	base_addr = pthis->app_cache_addr;

	//进行数据解析
	st_crc32_reset();
	
	do{
		read_error = 0;
		cur_pkg_step++;
		read_fw_addr = base_addr+(cur_pkg_step*FRAME_LENGTH);
		
		if(pthis->r_flash->read(read_fw_addr, fw_cache, FRAME_LENGTH))
		{
			//CRC校验&帧ID校验
			if(pmsg->DataFrame.CRC16 == crc16_ccitt(fw_cache,FRAME_LENGTH-2))   //校验数据帧的完整性
			{
				//校验版本
				if(pthis->fw_msg.MsgFrame.AppVer != pmsg->DataFrame.SoftVer)
				{
					*fw_id = cur_pkg_step; //不能使用读取的数据
					return ret_need_pkg;
				}
				
				//校验帧区分
				if(pthis->fw_msg.MsgFrame.board_type != pmsg->DataFrame.FrameType)
				{
					LOST_RECORD(pmsg->DataFrame.FrameStep); 
					continue; //继续查找
				}
				
				if(pmsg->DataFrame.FrameStep != cur_pkg_step)
				{
					LOST_RECORD(pmsg->DataFrame.FrameStep); 
					continue; //继续查找
				} 
				
				//计算CRC
				fw_src_crc32 = st_crc32((fw_cache+4), (FRAME_LENGTH-6));
			}
			else
			{
				LOST_RECORD(pmsg->DataFrame.FrameStep); 
				continue; //继续查找
			}
		}
		else
		{
			read_error++;
			if(read_error >= 3)
			{ 
				return  ret_flash_lost;
			}
		}
	}while(cur_pkg_step < (pthis->fw_msg.MsgFrame.AppTotalPackage-1));

	if(GET_LOST_RECORD() != 0)	//丢包
	{
		//如果固件不完整返回不完整信息
		if(((pthis->fw_msg.MsgFrame.AppTotalPackage/GET_LOST_RECORD())*100)> MAX_MASS_LOST_LEVEL)
		{
			return  ret_mass_lost; //丢包率过高
		}
		else
		{
			*fw_id = fw_ids;
			return ret_rev_pkg_id;
		}
	}

	//进行全局校验
	if(fw_src_crc32 != pthis->fw_msg.MsgFrame.AppCrc32)	
	{
		//全局校验失败，写入失败原因
		
		return ret_check_error;
	}
	 
	return ret_ok;
}

/*==================================================================================
* 函 数 名： msg_frame_compare
* 参    数： None
* 功能描述:  信息帧比较
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2019-12-09 151546
==================================================================================*/
uint8_t msg_frame_compare(const uint8_t *src_buff, const uint8_t *rev_buff, uint8_t len)
{
	for(int i=0; i<len; i++)
	{
		if(src_buff[i] != rev_buff[i])
		{
			return 0x81;
		}
	}
	
	return 0x01;
}




