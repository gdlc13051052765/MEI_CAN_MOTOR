#include "config_can.h" 
#include "interface.h"
#include "application.h"

_IO_Node_Tab mCAN_PW_IO_MAP_Tab[BOX_POWER_PIN_NUM];

void can_power_and_addr_map(uint8_t power, uint8_t addr)
{

	mCAN_PW_IO_MAP_Tab[addr].box_addr = addr;
	mCAN_PW_IO_MAP_Tab[addr].box_pw_prot = (GPIO_TypeDef *)(g_box_power_port[power]);
	mCAN_PW_IO_MAP_Tab[addr].box_pw_pin = g_box_power_pin[power];
	mCAN_PW_IO_MAP_Tab[addr].power_addr = power;
}

void can_power_and_addr_init(void)
{
	can_power_and_addr_map(0x09, 0x00);//空位，分配地址从index为1开始
	can_power_and_addr_map(0x06, 0x01);
	can_power_and_addr_map(0x03, 0x02); 
	can_power_and_addr_map(0x00, 0x03); 
	can_power_and_addr_map(0x07, 0x04); 
	can_power_and_addr_map(0x04, 0x05); 
	can_power_and_addr_map(0x01, 0x06);
	can_power_and_addr_map(0x08, 0x07); 
	can_power_and_addr_map(0x05, 0x08); 
	can_power_and_addr_map(0x02, 0x09); 
	
	//保留
	can_power_and_addr_map(0x0A, 0x0A); 
	can_power_and_addr_map(0x0B, 0x0B); 
};
/*==================================================================================
* 函 数 名： sed_addr_to_slave
* 参    数： None
* 功能描述:  给从机发送配置地址
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-11-08 111658
==================================================================================*/
static uint8_t sed_addr_to_slave(uint8_t addr_value)
{
	uint8_t sed_buff[8] = {0};
	sed_buff[0] = mApp_Param.can_addr;		//固定字
	sed_buff[1] = addr_value;		//配置地址
	
	can_send_one_pkg_to_Box_by_radio(CC_BOX_ASSIGNED_ADDR, 0, sed_buff, 2);
	return 0;
}

/*==================================================================================
* 函 数 名： config_slave_can_addr
* 参    数： None
* 功能描述:  给从机分配一个can地址
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-11-07 172059
==================================================================================*/
void config_slave_can_addr(uint8_t c_addr, uint16_t delay_time)
{
	//关闭其他box电源
	SET_BOX_POWER(mCAN_PW_IO_MAP_Tab[c_addr].power_addr, CLOSE_OTHERS);	//关闭其他
	
	//延时等待从机正常工作
	HAL_Delay(delay_time);

	//给从机发送分配地址命令
	sed_addr_to_slave(c_addr); 

	printf("sed config can addr, addr=%d to power=%d\n", c_addr, mCAN_PW_IO_MAP_Tab[c_addr].power_addr);
}

/*==================================================================================
* 函 数 名： config_can_addr_task
* 参    数： None
* 功能描述:  给从机分配地址任务
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-11-08 123702
==================================================================================*/
void config_can_addr_task(void *argv)
{
	static uint8_t cfg_addr = 0x01;	//开始分配的地址
	static uint8_t try_time = 0;		//最大重发次数

	printf("config can addr\n");
	 
	if(cfg_addr >= MAX_BOX_ADDR)	//检测完成
	{
		cfg_addr = 0x01;	
		try_time = 0;
		//关闭地址分配任务
		DisableTask(TASK_CFG_CAN_ADDR);
		
		//打开所有电源
		box_power_all_open();
		
		printf("exit config can addr\n");
		
		return ;
	}

	if (mApp_Param.can_addr == 0) //若cc还没有地址，则不给餐格分配
	{
		return ;
	}
	 
	//发送绑定关系
	config_slave_can_addr(cfg_addr, 200+try_time*100);	//分配地址

	//进行下一个节点地址检测分配
	try_time++;

	if(try_time >= 3)
	{
		try_time = 0;
		cfg_addr = (uint8_t)((cfg_addr + 1) % (MAX_BOX_ADDR + 1));	//进行下一个节点检测
	}
}

