

#include "pwm.h"

/*

void TIM1_PWM_Init()
{
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	pwm_gpioInit();	
	pwm_timeInit(900,0);
	
	///GPIO_PinRemapConfig(GPIO_PartialRemap_TIM1,ENABLE);	//TIM1部分重映射
	
	// 通道初始化设置
	//TIM_OCInitStruct.TIM_OCIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM2;
	//TIM_OCInitStruct.TIM_OCNIdleState=
	//TIM_OCInitStruct.TIM_OCNPolarity=
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;
	//TIM_OCInitStruct.TIM_OutputNState=
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;
	//TIM_OCInitStruct.TIM_Pulse=
	TIM_OC1Init(TIM1,&TIM_OCInitStruct);
	
	
	//TIM1->CH1通道，PA8
	TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable); //使能TIM1在CCR2上的预装载寄存器
	
	TIM_Cmd(TIM2, ENABLE);  //使能TIM3
	
}

*/

void TIM1_PWM_Init()
{
	TIM_OCInitTypeDef TIM_OCInitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能定时器3时钟
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    //复用时钟使能 
	
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;	//复用推挽输出模式
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;	//引脚为PC13
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;	//复用推挽输出模式
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_7;	//引脚为PC13
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    //复用时钟使能 
	///GPIO_PinRemapConfig(GPIO_PartialRemap_TIM1,ENABLE);	//TIM1部分重映射
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision= 0;	//时钟分割
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInitStruct.TIM_Period=71;	//自动重装的初值【周期？？】
	TIM_TimeBaseInitStruct.TIM_Prescaler=0;	//时钟频率除数的预分频值【分频系数】
	//TIM_TimeBaseInitStruct.TIM_RepetitionCounter=
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);

	TIM_TimeBaseInitStruct.TIM_Period = 7199; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseInitStruct.TIM_Prescaler =0; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位



	// 通道初始化设置
	//TIM_OCInitStruct.TIM_OCIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM2;
	//TIM_OCInitStruct.TIM_OCNIdleState=
	//TIM_OCInitStruct.TIM_OCNPolarity=
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;
	//TIM_OCInitStruct.TIM_OutputNState=
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;
	//TIM_OCInitStruct.TIM_Pulse=
	TIM_OC1Init(TIM2,&TIM_OCInitStruct);	//		【选择CH1】
	
		TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性高
	TIM_OC2Init(TIM3, &TIM_OCInitStruct);  //根据T指定的参数初始化外设TIM3 OC2

	
	//TIM1->CH1通道，PA8
	TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable); //使能TIM1在CCR2上的预装载寄存器
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
 
	TIM_Cmd(TIM3, ENABLE);  //使能TIM3
	TIM_Cmd(TIM2, ENABLE);  //使能TIM3
	
}

//GPIO初始化
void pwm_gpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//开启时钟
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;	//复用推挽输出模式
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;	//引脚为PC13
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    //复用时钟使能 
}

//定时器初始化
void pwm_timeInit(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision= 0;	//时钟分割
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInitStruct.TIM_Period=arr-1;	//自动重装的初值【周期？？】
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc;	//时钟频率除数的预分频值【分频系数】
	//TIM_TimeBaseInitStruct.TIM_RepetitionCounter=
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);

	//TIM_Cmd(TIM1, ENABLE);  //使能TIM3【放在后面去？】
}



