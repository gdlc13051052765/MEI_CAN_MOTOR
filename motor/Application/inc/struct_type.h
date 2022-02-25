#ifndef  __STRUCT_TYPE_H
#define  __STRUCT_TYPE_H
#include <stdint.h>

#define RADIO_MAX_LEN					256
#define SERIAL_MAX_LEN	1028

//typedef union
//{
//	struct
//	{
//		uint8_t w_addr;
//		uint8_t w_len;
//		uint8_t w_cmd;
//		uint8_t w_data[1];
//	}_Tx_Msg;
//	
//	struct
//	{
//		uint8_t r_addr;
//		uint8_t r_len;
//		uint8_t r_cmd;
//		uint8_t r_status;
//		uint8_t r_data[1];
//	}_Rx_Msg;
//	
//	struct
//	{
//		uint8_t s_addr;
//		uint8_t s_len;
//		uint8_t s_cmd;
//		uint8_t s_crc;
//	}_Simply;
//	
//	uimt8_t buff[MAX_LEN];
//}_Comm_Msg,*_pComm_Msg;


typedef union
{
	struct
	{
		uint8_t match;
		uint8_t d_addr;
		uint8_t s_addr;
		uint8_t node_id;	//boxid
		uint8_t cmd_index;
		uint8_t payload[1];
	}_Comm;
	 
	struct
	{
		uint8_t d_addr;
		uint8_t s_addr;
		uint8_t node_id;	//boxid
		uint8_t cmd_index;
		uint32_t crc; 
	}_Simply;
	
	uint8_t buff[RADIO_MAX_LEN];
}_Radio_Msg,*_pRadio_Msg;			//无线通信结构

typedef union
{
	struct
	{
		uint8_t sof_0;
		uint8_t sof_1;
		uint8_t first_id; 
		uint8_t len;
		uint8_t sl_id;
		uint8_t cmd;
		uint8_t node_id;		//对于uart-si4463 node 相当于 数据域
		uint8_t payload[1];
	}_Cmd_Msg;
	
	struct
	{
		uint8_t sof_0;
		uint8_t sof_1;
		uint8_t first_id; 
		uint8_t len;
		uint8_t sl_id;
		uint8_t cmd; 		
		uint8_t payload[1]; //对于uart-si4463 node 相当于 数据域
	}_Comm;
	  
	uint8_t buff[SERIAL_MAX_LEN];
}_Serial_Msg,*_pSerial_Msg; //串口通信结果


//radio si通信包

typedef struct
{ 
	uint8_t match;
	uint8_t channel;
}_IP_Typedef;


typedef 	uint8_t				_Serial_Item;
typedef  	uint8_t*			_pSerial_Item;


typedef struct
{ 
	uint8_t pointer;	//指针
	uint8_t lenght;
	_IP_Typedef ip; 
	uint8_t buff[RADIO_MAX_LEN];
}_Si_Pkt,*_pSi_Pkt;

typedef struct
{
	uint8_t is_use;	//是否使用  0:可用  1：使用中
	uint8_t try_count;	//尝试重发次数 
	_Si_Pkt pkt_buff;
}_Fifo_Item,*_pFifo_Item;


typedef struct
{
	uint8_t w_point;					//写入的长度
	uint8_t pp_buf[256];			//ping缓存   查询帧步进值+box_id+标签个数（1B，需要更新标签数量）+（标签类型(1B 1新增  0:删除)+标签信息(8+8)）（17*标签个数）
}_PP_item,*_pPP_Item;

typedef struct
{
	uint8_t step_count;				//自定义帧步进值
	_pPP_Item p_cur;					//当前指针
	_PP_item ping_item;			//ping缓存
	_PP_item pang_item;			//ping缓存
}_PP_Cache,*_pPP_Cache;		//乒乓数据结构

#endif
