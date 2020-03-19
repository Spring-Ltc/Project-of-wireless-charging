#include "coder.h"


////////////////////
//定时器TIM4初始化，设置为编码器模式
void TIM4_CoderInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;//GPIO结构体
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//定时器结构体
	TIM_ICInitTypeDef TIM_ICInitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	//IO口管脚初始化
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF ;//复用IO模式
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP ;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;//TIM4的CH1和CH2通道对应管脚为PB6\PB7
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP ;//上拉，触点接通后为低电平
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz ;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//映射到定时器上
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_TIM4);
	
	//定时器初始化设置
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;//向上计数模式
	TIM_TimeBaseInitStruct.TIM_Period=0xffff;
	TIM_TimeBaseInitStruct.TIM_Prescaler=0x0000;
	//TIM_TimeBaseInitStruct.TIM_RepetitionCounter=
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStruct);
	
	//配置TIMx编码器接口				定时器			计数模式					通道1触发计数极性						通道2触发计数极性
	TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI1,TIM_ICPolarity_Falling,TIM_ICPolarity_Falling);
	
	TIM_ICStructInit(&TIM_ICInitStruct);
	TIM_ICInitStruct.TIM_ICFilter=10;
	TIM_ICInit(TIM4,&TIM_ICInitStruct);
	TIM_ClearFlag(TIM4,TIM_FLAG_Update);
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	
	TIM4->CNT = 0;
  TIM_Cmd(TIM4, ENABLE);
}

