#include "debug_task.h"
#include "main.h"
#include "def_fifo.h" 

/*===========================自定义接口列表==================================*/
 void DEF_NAME_FUNC(SI_INT_STATE)(void *argc, void *argv);
 void DEF_NAME_FUNC(SI_DEV_STATE)(void *argc, void* argv);
 void DEF_NAME_FUNC(SI_NAME_RET)(void *argc, void *argv);
void DEF_NAME_FUNC(SI_RESET)(void *argc, void *argv);

 /*==============================end=======================================*/
 

#define MAX_MIN_CMD_LEN				4			//最小命令字的长度

static _cmd_cache mCmd_Cache = 
{
	.is_value				= 0,	//是否接收有效命令
	.rev_count			= 0,
	.r_point				= 0,
};

//命令格式  [命令字符串]空格[参数1]空格[参数2]空格[...]\r\n 
static void DEF_NAME_FUNC(help)(void *argc, void* argv);
static void DEF_NAME_FUNC(shell)(void *argc, void* argv);
static void DEF_NAME_FUNC(error)(void *argc, void* argv);
static void DEF_NAME_FUNC(reboot)(void *argc, void* argv);

_cmd_list  mCmd_List_Table[] = 
{ 
	CMD_FUNC_DEF(error),		//错误提示
	CMD_FUNC_DEF(help),
	CMD_FUNC_DEF(shell),
	CMD_FUNC_DEF(reboot),
	
	/*==========自定义==============*/
	CMD_FUNC_DEF(SI_INT_STATE),
	CMD_FUNC_DEF(SI_DEV_STATE),
	CMD_FUNC_DEF(SI_NAME_RET),
	CMD_FUNC_DEF(SI_RESET),
	
};

//help
static void DEF_NAME_FUNC(help)(void *argc, void* argv)
{
	uint8_t step_count = 0;
	
	printf("命令个数: [命令字符串] [参数1] [参数2]\n");
	
	printf("cmd list[sys cmd]:\n");
	
	printf("\t: %s\n", mCmd_List_Table[step_count++].cmd_name);
	printf("\t: %s\n", mCmd_List_Table[step_count++].cmd_name);
	printf("\t: %s\n", mCmd_List_Table[step_count++].cmd_name);
	printf("\t: %s\n", mCmd_List_Table[step_count++].cmd_name);
	
	printf("cmd list[usr cmd]:\n");
	for(int i=step_count++; i<  MAX_NUM(mCmd_List_Table); i++)
	{ 
	  printf("\t: %s\n", mCmd_List_Table[i].cmd_name);
		
		if(((uint32_t *)argv)[0] == 0xFF)
		{
			mCmd_List_Table[i].cmd_func(argc, argv);	//打印详细信息
		}
	}
	
}

//entyr shell
static void DEF_NAME_FUNC(error)(void *argc, void* argv)
{
	printf("不支持 [  %s  ] 命令，请使用help查看命令列表\n" , (char*)argc);
}

//entyr shell
static void DEF_NAME_FUNC(shell)(void *argc, void* argv)
{
	printf("cmd frame: [命令字符串] [参数1] [参数2]\n");
}

//sys reboot
static void DEF_NAME_FUNC(reboot)(void *argc, void* argv)
{
	printf("systerm reboot success!\n");
	 NVIC_SystemReset();
}

/*==================================================================================
* 函 数 名： push_cmd_cache
* 参    数： None
* 功能描述:  把接收到的命令字符放到缓存
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-11-12 172859
==================================================================================*/
void push_cmd_cache(uint8_t ch)
{ 
	mCmd_Cache.rev_buff[mCmd_Cache.rev_count++] = ch; 
	 
	if(ch == '\r')
	{
		mCmd_Cache.is_value++;		//接到的完整命令
	}
}

/*==================================================================================
* 函 数 名： execute_cmd
* 参    数： None
* 功能描述:  命令处理
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-11-12 172859
==================================================================================*/
static void execute_cmd(const char *cmd_buf, uint16_t len)
{

}

/*==================================================================================
* 函 数 名： cmd_string_handle
* 参    数： None
* 功能描述:  字符串命令处理
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-11-12 172859
==================================================================================*/ 
p_func find_func(char *cmd_str)
{ 
	for(int i=0; i<  MAX_NUM(mCmd_List_Table); i++)
	{ 
		if(strstr(cmd_str,mCmd_List_Table[i].cmd_name) != 0)
		{
			return mCmd_List_Table[i].cmd_func;
		}
	}
	 
	return mCmd_List_Table[0].cmd_func;		//打印错误提示
}

/*==================================================================================
* 函 数 名： sub_cmd_string
* 参    数： None
* 功能描述:  截取命令
* 返 回 值： 返回解析字符串的长度
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-11-14 104515
==================================================================================*/ 
int sub_cmd_string(char *src_str, int src_len, char *dst_str)
{
	uint8_t start_index =0, end_index=0;
	
	//查找起始位置
	while((src_str[start_index] == '\r') || (src_str[start_index] == '\n'))
	{
		start_index++;
	}
	
	//修改起始查找位置
	end_index = start_index;
	
	//查找结束位置
	while((src_str[end_index] != '\r') && (src_str[end_index] != '\n'))
	{
		end_index++;
	}
	
	//拷贝起始地址
	for(int i=start_index; i<end_index; i++)
	{
		dst_str[i] = src_str[i];
	}

	return (end_index+1);
}

/*==================================================================================
* 函 数 名： cmd_handle_func_task
* 参    数： None
* 功能描述:  字符串命令处理
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-11-14 105247
==================================================================================*/ 
int cmd_frame_param(const char *cmd_frame, char *cmd_str, uint32_t *param_tab)
{ 	
	//查找
	sscanf(cmd_frame, "\r\n%[^ ] %d %d \r", cmd_str, (int*)&param_tab[0], (int*)&param_tab[1]); 	//分离命令和参数
	 
	printf("cmd_str=%s, param_tab[0]=0x%04x, param_tab[1]=0x%04x\n", cmd_str, param_tab[0], param_tab[1]);
	
	return 0;
}

/*==================================================================================
* 函 数 名： cmd_handle_func_task
* 参    数： None
* 功能描述:  字符串命令处理
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-11-14 105247
==================================================================================*/ 
void cmd_handle_func_task(void)
{
	p_func pfunc = 0;
	char cmd_frame[16] = {0};			//一帧命令的缓存
	char cmd_str[16] = {0};				//命令字
	uint32_t param_tab[8] = {0}; 	//参数（十六进制） 
	
	if(!mCmd_Cache.is_value)
	{
		return ;
	}
	
	while(1)
	{
		//解析出一帧命令（不包含\r\n）
		mCmd_Cache.r_point = sub_cmd_string((char*)(mCmd_Cache.rev_buff+mCmd_Cache.r_point), mCmd_Cache.rev_count, cmd_frame);
		
		//解析参数
		cmd_frame_param(cmd_frame, cmd_str, param_tab);
		
		//查找函数
		pfunc = find_func(cmd_str);
		 
		//执行函数
		pfunc(cmd_str, param_tab);
		 
		//判断是否解析完成
		if(mCmd_Cache.r_point + MAX_MIN_CMD_LEN >= mCmd_Cache.rev_count)
		{
			//清空缓存 
			mCmd_Cache.is_value = 0;
			mCmd_Cache.rev_count = 0;
			mCmd_Cache.r_point = 0;
			memset(mCmd_Cache.rev_buff, 0, sizeof(mCmd_Cache.rev_buff));
			return ;
		}
	}
}

/*==================================================================================
* 函 数 名： cmd_handle_func_task
* 参    数： None
* 功能描述:  字符串命令处理
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-11-12 172859
==================================================================================*/ 
//void cmd_handle_func_task(void)
//{
//	p_func pfunc = 0;
//	char cmd_str[16] = {0};
//	uint8_t param_tab[8] = {0}; 
//	uint8_t point = 0;
//	
//	if(!mCmd_Cache.is_value)
//	{
//		return ;
//	}
//	
//	//进行命令解析
//	do{
//		//去除多余的 \r\n
//		while((mCmd_Cache.rev_buff[mCmd_Cache.r_point] == '\r')||(mCmd_Cache.rev_buff[mCmd_Cache.r_point] == '\n')) 
//		{
//			mCmd_Cache.r_point++; 
//			if(mCmd_Cache.rev_count > 0)
//			{
//				mCmd_Cache.rev_count--;
//			}
//			
//			if(mCmd_Cache.rev_count == 0)
//			{
//				//清空缓存
//				mCmd_Cache.rev_count = 0;
//				mCmd_Cache.r_point = 0;
//				mCmd_Cache.is_value = 0;
//				memset(mCmd_Cache.rev_buff, 0, sizeof(mCmd_Cache.rev_buff));
//				return ;
//			}
//		}
//		
//		//进行命令拷贝
//		do{cmd_str[point++] =  mCmd_Cache.rev_buff[mCmd_Cache.r_point++]; mCmd_Cache.rev_count--;}while((mCmd_Cache.rev_buff[mCmd_Cache.r_point] != '\r')&&(mCmd_Cache.rev_count != 0));
//	
//		//命令合法性判断
//		if((cmd_str[0] != 0) && (cmd_str[0] != '\r') && (cmd_str[0] != '\n'))
//		{
//			//查找函数
//			pfunc = find_func(cmd_str);
//			
//			//执行命令
//			if(pfunc != NULL)
//			{
//				pfunc(param_tab);
//			}
//			
//			point = 0;
//			memset(cmd_str, 0, sizeof(cmd_str));
//		}

//		//修正参数
//		(mCmd_Cache.is_value>0)?(mCmd_Cache.is_value--):(mCmd_Cache.is_value=0);
//	}while(mCmd_Cache.is_value);
//	
//	//清空缓存 
//	mCmd_Cache.is_value = 0;
//	mCmd_Cache.rev_count = 0;
//	mCmd_Cache.r_point = 0;
//	memset(mCmd_Cache.rev_buff, 0, sizeof(mCmd_Cache.rev_buff));
//	 
//}



