#ifndef __PWM_H__
#define __PWM_H__

#include "sys.h"
//#include "stm32f10x.h"

void TIM1_PWM_Init(void);
void SetPwmVal(u32 Freq,u16 Duty);

#endif

