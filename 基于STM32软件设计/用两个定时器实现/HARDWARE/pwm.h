#ifndef __PWM_H__
#define __PWM_H__

#include "sys.h"
//#include "stm32f10x.h"

void TIM1_PWM_Init(void);
void pwm_gpioInit(void);
void pwm_timeInit(u16 arr,u16 psc);

#endif

