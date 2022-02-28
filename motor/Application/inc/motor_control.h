#ifndef  __MOTOR_CONTROL_H
#define  __MOTOR_CONTROL_H

#include <stdint.h>
#include <stdio.h>

#define	ZERO_SPEED	10000//电机回原点的速度
#define MOTOR_NUM		4 //电机个数 4轴
//pc发送运行指令的频率
#define PLUS_RATE  20 //20ms
//原点位置
#define ZREO_PLUS  1000
//极限速度
#define LIMIT_SPEED 30000

enum {
	stop_status = 0,//停止状态
	negative_limit = 4,//负向极限开关
	positive_limit = 8,//正向极限	
};

//电机参数结构体
typedef struct
{ 
	uint8_t  run_state;//运动状态
	uint8_t	 Io_state;//各轴的极限位置
	int			 position;//该轴的逻辑位置
	int      tagetPosition;//目标位置
	int			 runSpeed;//电机运行速度
	uint8_t  data[MOTOR_NUM];
}_motor_Msg,*_pmotor_Msg;

typedef struct motor_queue {
	_motor_Msg queue[MOTOR_NUM];
}motor_queue_t, * p_motor_queue_t;

/*==================================================================================
* 函 数 名： motor_position_zero
* 参    数： None
* 功能描述:  hk 电机位置归零
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-05-29 170658
==================================================================================*/
void motor_position_zero(void);

/*==================================================================================
* 函 数 名： motor_emergency_stop
* 参    数： None
* 功能描述:  hk 电机急停
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-06-29 170658
==================================================================================*/
void motor_emergency_stop(void);

/*==================================================================================
* 函 数 名： get_motor_current_state
* 参    数： None
* 功能描述:  获取点击当前状态
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-06-29 170658
==================================================================================*/
motor_queue_t get_motor_current_state(void);

#endif
