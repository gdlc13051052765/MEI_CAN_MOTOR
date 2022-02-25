#ifndef  __MINI_DISPATCH_H
#define  __MINI_DISPATCH_H
#include <stdint.h>

typedef void (*task_hook_func)(void*);

typedef enum 
{
	TASK_ENABLE=1,
	TASK_DISABLE
}TASK_STA;

// ����ṹ
typedef struct _TASK_COMPONENTS
{
	uint8_t 		Run;                 		//�������б�ǣ�0-�����У�1����
	TASK_STA		RunState;								//����״̬��TASK_ENABLE��ʹ�ܼ��    TASK_DISABLE�������м��
	//		uint16_t    Priority;								//���ȼ�
	uint32_t  	run_power;						//���Ǹ�����ÿ����һ�μ�һ
	uint32_t 		Timer;              		//��ʱ��
	uint32_t 		ItvTime;              	//�������м��ʱ�䣬��һ�����ʱ��
	void (*TaskHook)(void*);    		// Ҫ���е�������
} TASK_COMPONENTS;       					// ������


// �����嵥
typedef enum _TASK_LIST
{   
	UDP_SOCKET_LOOP,							//udp socket����	
	UDP_SOCKET_DECODE,						//udp ���ݽ���
	TASK_TEST_LOOP,							//��������
	
	TASKS_MAX                   // �ܵĿɹ�����Ķ�ʱ������Ŀ
} TASK_LIST;

void TaskSetTimes(TASK_LIST Task , uint32_t Times);
void TaskRefresh(TASK_LIST Task);
uint8_t IsTaskEnable(TASK_LIST Task);
void EnableTask(TASK_LIST Task);
void DisableTask(TASK_LIST Task);
void TaskRemarks(void);
void TaskProcess(void*);
#endif
