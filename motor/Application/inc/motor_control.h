#ifndef  __MOTOR_CONTROL_H
#define  __MOTOR_CONTROL_H

#include <stdint.h>
#include <stdio.h>

#define	ZERO_SPEED	10000//�����ԭ����ٶ�
#define MOTOR_NUM		4 //������� 4��
//pc��������ָ���Ƶ��
#define PLUS_RATE  20 //20ms
//ԭ��λ��
#define ZREO_PLUS  1000
//�����ٶ�
#define LIMIT_SPEED 30000

enum {
	stop_status = 0,//ֹͣ״̬
	negative_limit = 4,//�����޿���
	positive_limit = 8,//������	
};

//��������ṹ��
typedef struct
{ 
	uint8_t  run_state;//�˶�״̬
	uint8_t	 Io_state;//����ļ���λ��
	int			 position;//������߼�λ��
	int      tagetPosition;//Ŀ��λ��
	int			 runSpeed;//��������ٶ�
	uint8_t  data[MOTOR_NUM];
}_motor_Msg,*_pmotor_Msg;

typedef struct motor_queue {
	_motor_Msg queue[MOTOR_NUM];
}motor_queue_t, * p_motor_queue_t;

/*==================================================================================
* �� �� ���� motor_position_zero
* ��    ���� None
* ��������:  hk ���λ�ù���
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-05-29 170658
==================================================================================*/
void motor_position_zero(void);

/*==================================================================================
* �� �� ���� motor_emergency_stop
* ��    ���� None
* ��������:  hk �����ͣ
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-06-29 170658
==================================================================================*/
void motor_emergency_stop(void);

/*==================================================================================
* �� �� ���� get_motor_current_state
* ��    ���� None
* ��������:  ��ȡ�����ǰ״̬
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-06-29 170658
==================================================================================*/
motor_queue_t get_motor_current_state(void);

#endif
