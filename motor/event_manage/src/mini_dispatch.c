#include "mini_dispatch.h"
#include "command.h"
extern void test_loop_task(void* argv);
extern void udp_socket_task(void* argv);
/*==================================================================================
* 函 数 名： TaskComps
* 参    数： None
* 功能描述:  任务表
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2017-7-28 15:12:42
==================================================================================*/
TASK_COMPONENTS TaskComps[] = 
{
	{0, TASK_ENABLE, 0, 15, 15, udp_socket_task},            	//优先级 4
	{0, TASK_ENABLE, 0, 15, 15, udp_rec_decode_task},         //优先级 4
	{0, TASK_ENABLE, 0, 3000, 3000, test_loop_task},          //优先级 4
};

/*==================================================================================
* 函 数 名： EnableTask
* 参    数： None
* 功能描述:  使能任务
* 返 回 值： None
* 备    注： 0:没有使能  1：使能   0xff:未知通道
* 作    者： xiaozh
* 创建时间： 2017-7-28 15:12:42
==================================================================================*/
uint8_t IsTaskEnable(TASK_LIST Task)
{
	if(Task > TASKS_MAX)
		return 0xff;
	
	if(TaskComps[Task].RunState == TASK_ENABLE)
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}

/*==================================================================================
* 函 数 名： TaskRefresh
* 参    数： None
* 功能描述:  刷新更新时间
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2017-7-28 15:12:42
==================================================================================*/
void TaskRefresh(TASK_LIST Task)
{
 	if(Task > TASKS_MAX)
		return;
	
	TaskComps[Task].Timer = TaskComps[Task].ItvTime; 
}

/*==================================================================================
* 函 数 名： EnableTask
* 参    数： None
* 功能描述:  使能任务
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2017-7-28 15:12:42
==================================================================================*/
void EnableTask(TASK_LIST Task)
{
	if(Task > TASKS_MAX)
		return;
	
//	TaskComps[Task].Timer = TaskComps[Task].ItvTime;
	TaskComps[Task].RunState = TASK_ENABLE;
}

/*==================================================================================
* 函 数 名： DisableTask
* 参    数： None
* 功能描述:  失能任务
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2017-7-28 15:12:42
==================================================================================*/
void DisableTask(TASK_LIST Task)
{
	if(Task > TASKS_MAX)
		return;
	
	TaskComps[Task].Timer = TaskComps[Task].ItvTime;
	TaskComps[Task].RunState = TASK_DISABLE;
}

/*==================================================================================
* 函 数 名： TaskSetTimes
* 参    数： None
* 功能描述:  任务标志标记函数
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2017-7-28 15:12:42
==================================================================================*/
void TaskSetTimes(TASK_LIST Task , uint32_t Times)
{
	if(Task > TASKS_MAX)
		return;
	
	TaskComps[Task].Timer = Times;
	TaskComps[Task].RunState = TASK_ENABLE;
}

/*==================================================================================
* 函 数 名： TaskRemarks
* 参    数： None
* 功能描述:  任务标志标记函数
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2017-7-28 15:12:42
==================================================================================*/
void TaskRemarks(void)
{
	uint8_t i;
	for (i=0; i<TASKS_MAX; i++)          // 逐个任务时间处理
	{
		if(TaskComps[i].RunState == TASK_ENABLE)		//如果使能了当前任务检测，则进行检测操作
		{
			if (TaskComps[i].Timer)          // 时间不为0
			{
				TaskComps[i].Timer--;         // 减去一个节拍
				if (TaskComps[i].Timer == 0)       // 时间减完了
				{
					 TaskComps[i].Timer = TaskComps[i].ItvTime;       // 恢复计时器值，从新下一次
					 TaskComps[i].run_power++;		//加权值增加
					 TaskComps[i].Run = 1;           // 任务可以运行
				}
			}
		}
  }
}

/*==================================================================================
* 函 数 名： TaskRemarks
* 参    数： None
* 功能描述:  任务处理
* 返 回 值： None
* 备    注： 建议在中循环中调用，在定时中断中调用注意操作时间
* 作    者： xiaozh
* 创建时间： 2017-7-28 15:12:42
==================================================================================*/
void TaskProcess(void *who)
{
	uint8_t i; 
	for (i=0; i<TASKS_MAX; i++)           // 逐个任务时间处理
	{
		if (TaskComps[i].Run)           // 时间不为0
		{
			 TaskComps[i].TaskHook(who);         // 运行任务
			 TaskComps[i].Run = 0;          // 标志清0
			 return;
		}
	}
}
