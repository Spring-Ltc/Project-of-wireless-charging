/***************************************************

//默认复用，不映射
TIM1_CH1 :	PA8,			TIM1_CH1N	: PB13
TIM1_CH2 :	PA9,			TIM1_CH2N	: PB14
TIM1_CH3 :	PA10,			TIM1_CH3N	: PB15
TIM1_CH4 :	PA11,
//重映射过后
											TIM1_CH1N	: PA7
											TIM1_CH2N	: PB0
											TIM1_CH3N	: PB1
		
*****************************************************/

#include "pwm.h"

#define Count_Clock 72000	//定时器时钟频率


//改变频率和占空比
//相当于对定时器和输出通道重新初始化一遍
void SetPwmVal(u32 Freq,u16 Duty)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//定时器结构体
	TIM_OCInitTypeDef TIM_OCInitStruct;//通道结构体
	
	u32 period;
	u16 pulse;
	
	period=Count_Clock/Freq-1;	//计算计数周期（倒数即为频率）
	pulse=(period+1)*Duty/100;	//计算输出脉宽值（即占空比）

	
//定时器TIM1初始化
	TIM_TimeBaseInitStruct.TIM_ClockDivision= 0;	//时钟分割
	//TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_CenterAligned1;	//【也是向上计数？？？？？？？？？？？？？？？？？】
	TIM_TimeBaseInitStruct.TIM_Period=period;	//自动重装的初值【周期】
	TIM_TimeBaseInitStruct.TIM_Prescaler=0;	//【预分频值】【分频系数】
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;	//重复计数设置为0
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);

	// 通道初始化设置

	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNPolarity=TIM_OCNPolarity_High;							//反向输出极性选择
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;	//输出极性选择
	TIM_OCInitStruct.TIM_OutputNState=TIM_OutputNState_Enable;						//反向输出通道使能
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//输出通道使能
	
	TIM_OCInitStruct.TIM_Pulse=pulse;		//用来调节输出脉冲的占空比(1/4)
	TIM_OC1Init(TIM1,&TIM_OCInitStruct);	//		【选择CH1】
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);  //比较寄存器（TIM1->CCR1）重装载值使能 
	
	TIM_OCInitStruct.TIM_Pulse=pulse;		//用来调节输出脉冲的占空比(1/2)
	TIM_OC2Init(TIM1,&TIM_OCInitStruct);	//		【选择CH2】
	TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Enable);
	
	TIM_Cmd(TIM1,ENABLE); //定时器1使能
}


//各种初始化（GPIO，定时器，通道，死区）
void TIM1_PWM_Init()
{
	TIM_OCInitTypeDef TIM_OCInitStruct;//通道结构体
	GPIO_InitTypeDef GPIO_InitStruct;//GPIO结构体
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//定时器结构体
	TIM_BDTRInitTypeDef TIM_BDTRInitStruct;//死区结构体

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);//定时器1时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);//开启GPIOA和GPIOB的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    //复用时钟使能 
	
	//GPIO初始化设置
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;	//复用推挽输出模式
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8;	//引脚为PA8
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;	//复用推挽输出模式
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_13;	//互补通道引脚为PB13
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);

	//定时器TIM1初始化
	TIM_TimeBaseInitStruct.TIM_ClockDivision= 0;	//时钟分割
	//TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_CenterAligned1;	//【也是向上计数？？？？？？？？？？？？？？？？？】
	TIM_TimeBaseInitStruct.TIM_Period=7199;	//自动重装的初值【周期】
	TIM_TimeBaseInitStruct.TIM_Prescaler=1;	//【预分频值】【分频系数】
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;	//重复计数设置为0
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);

	// 通道初始化设置

	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNPolarity=TIM_OCNPolarity_High;							//反向输出极性选择
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;	//输出极性选择
	TIM_OCInitStruct.TIM_OutputNState=TIM_OutputNState_Enable;						//反向输出通道使能
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//输出通道使能
	
	TIM_OCInitStruct.TIM_Pulse=3600;		//用来调节输出脉冲的占空比(1/4)
	TIM_OC1Init(TIM1,&TIM_OCInitStruct);	//		【选择CH1】
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);  //比较寄存器（TIM1->CCR1）重装载值使能 
	
	TIM_OCInitStruct.TIM_Pulse=3600;		//用来调节输出脉冲的占空比(1/2)
	TIM_OC2Init(TIM1,&TIM_OCInitStruct);	//		【选择CH2】
	TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Enable);
	
	//死区初始化设置
	TIM_BDTRInitStruct.TIM_AutomaticOutput=TIM_AutomaticOutput_Enable;//主动输出使能
	TIM_BDTRInitStruct.TIM_Break=TIM_Break_Disable;//禁止刹车输入
	TIM_BDTRInitStruct.TIM_BreakPolarity=TIM_BreakPolarity_High;//刹车输入极性
	TIM_BDTRInitStruct.TIM_DeadTime=28; //死区寄存器存放的数值为28 
	TIM_BDTRInitStruct.TIM_LOCKLevel=TIM_LOCKLevel_OFF;//锁定关闭，寄存器无写保护
	TIM_BDTRInitStruct.TIM_OSSIState=TIM_OSSIState_Disable;
	TIM_BDTRInitStruct.TIM_OSSRState=TIM_OSSRState_Disable;
	TIM_BDTRConfig(TIM1,&TIM_BDTRInitStruct);
	

	TIM_ARRPreloadConfig(TIM1,ENABLE); //定时器1重装载使能
	TIM_CtrlPWMOutputs(TIM1,ENABLE);//pwm输出使能
	TIM_Cmd(TIM1,ENABLE); //定时器1使能

}
