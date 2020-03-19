#ifndef __PWM_H__
#define __PWM_H__

#include "sys.h"

void TIM8_PWM_Init(void);
void SetPwmVal(u32 Freq,u32 Duty,u32 Dead);



#endif


