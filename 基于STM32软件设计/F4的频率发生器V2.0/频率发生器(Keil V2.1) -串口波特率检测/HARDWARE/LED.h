#ifndef __LED_H__
#define __LED_H__

#include "sys.h"


//����LED
#define LED0_ON GPIO_ResetBits(GPIOF,GPIO_Pin_9)
#define LED1_ON GPIO_ResetBits(GPIOF,GPIO_Pin_10)

//�ر�LED
#define LED0_OFF GPIO_SetBits(GPIOF,GPIO_Pin_9)
#define LED1_OFF GPIO_SetBits(GPIOF,GPIO_Pin_10)

#define LED0 PFout(9)
#define LED1 PFout(10)



//��������
extern void LED_Init(void);






#endif

