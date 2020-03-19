#ifndef __PWM_H__
#define __PWM_H__

#include "sys.h"

void TIM8_PWM_Init(void);
void SetPwmVal(u32 Freq,u32 Duty,u32 Dead);
void Calculate_Regester(u32 Freq,u32 Duty,u32 Dead,u8 choose);


//ʵ����д��Ĵ�����ֵ������Ϊȫ�ֱ���
extern u32 Period1;
extern u16 Pulse1;
extern u32 DeadTime1;
extern u32 Period2;
extern u16 Pulse2;
extern u32 DeadTime2;
#endif


