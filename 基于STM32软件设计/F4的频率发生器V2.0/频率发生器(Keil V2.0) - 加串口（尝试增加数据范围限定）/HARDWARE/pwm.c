
/****************定时器TIM1的资源配置*************************************/

//映射前：
//		TIM1_CH1		PE9	,					TIM1_CH1N		PE8	;
//		TIM1_CH2		PE11	,				TIM1_CH2N		PE10;
//		TIM1_CH3		PE13	,				TIM1_CH3N		PE12;
//		TIM1_CH4		PE14	,				


//映射后：


/**************************************************************************/

/****************定时器TIM8的资源配置*************************************/

//映射前：
//		TIM8_CH1		PC6	,				TIM8_CH1N		PA5/PA7	;
//		TIM8_CH2		PC7	,				TIM8_CH2N		PB0/PB14;
//		TIM8_CH3		PC8	,				TIM8_CH3N		PB1/PB15;
//		TIM8_CH4		PC9	,	
/**************************************************************************/


//下面采用TIM8的CH2通道输出互补波形【PC7和PB14】


#include "pwm.h"
#include "lcd.h"
#define Count_Clock 84000000	//定时器时钟频率84MHz
//#define Count_Clock 168000000	//定时器时钟频率168MHz

//改变频率和占空比和死区时间
//相当于对定时器和输出通道重新初始化一遍
void SetPwmVal(u32 Freq,u32 Duty,u32 Dead)
{
	//TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//定时器结构体
	//TIM_OCInitTypeDef TIM_OCInitStruct;//通道结构体
	TIM_BDTRInitTypeDef TIM_BDTRInitStruct;//死区结构体

	u32 period;
	u32 prescaler;
	u16 pulse;
	u32 dead;
	
	u8 sign=0;//死区范围标志
	
	
	//判断频率范围
	if(Freq<3000)//低频，为提高精度，要分频
	{
		prescaler=3000/Freq;
		period=Count_Clock/(Freq*(prescaler+1))-1;
	}
	else
	{
		prescaler=0;
		period=Count_Clock/Freq-1;
	}
	pulse=(period+1)*Duty/100;	//计算输出脉宽值（即占空比）
	
	//判断死区时间范围
	if(Dead<=756)//情况一
	{
		dead=Dead/5.95;sign=1;
	}
	else if(Dead<=1512)//情况二
	{
		dead=((Dead/2)/5.95)+128+0-64;sign=2;
	}
	else if(Dead<=3000)//情况三
	{
		dead=((Dead/8)/5.95)+128+64+0-32;sign=3;
	}
	else if(Dead<=6000)//情况四
	{
		dead=Dead/16/5.95-32+128+64+32;sign=4;
	}

	
	//2019/09/08/14:56修改为如下重置，直接修改相应寄存器参数，取代重新初始化
	//											[注意要将第一次初始化的分频系数改为0才适用上面的计算表达式]
	TIM_SetAutoreload(TIM8,period);//重新设置自动重装初值
	TIM_SetCompare1(TIM8,pulse);
	TIM_SetCompare2(TIM8,pulse);

	
	/*
//定时器TIM8初始化
	TIM_TimeBaseInitStruct.TIM_ClockDivision= 0;	//时钟分割
	//TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_CenterAligned1;	//【也是向上计数？？？？？？？？？？？？？？？？？】
	TIM_TimeBaseInitStruct.TIM_Period=period;	//自动重装的初值【周期】
	TIM_TimeBaseInitStruct.TIM_Prescaler=prescaler;	//【预分频值】【分频系数】
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;	//重复计数设置为0
	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseInitStruct);

	// 通道初始化设置

	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNPolarity=TIM_OCNPolarity_High;							//反向输出极性选择
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;	//输出极性选择
	TIM_OCInitStruct.TIM_OutputNState=TIM_OutputNState_Enable;						//反向输出通道使能
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//输出通道使能
	
	TIM_OCInitStruct.TIM_Pulse=pulse;		//用来调节输出脉冲的占空比(1/4)
	TIM_OC1Init(TIM8,&TIM_OCInitStruct);	//		【选择CH1】
	TIM_OC1PreloadConfig(TIM8,TIM_OCPreload_Enable);  //比较寄存器（TIM1->CCR1）重装载值使能 
	
	TIM_OCInitStruct.TIM_Pulse=pulse;		//用来调节输出脉冲的占空比(1/2)
	TIM_OC2Init(TIM8,&TIM_OCInitStruct);	//		【选择CH2】
	TIM_OC2PreloadConfig(TIM8,TIM_OCPreload_Enable);
	*/
	
	TIM_BDTRInitStruct.TIM_AutomaticOutput=TIM_AutomaticOutput_Enable;//主动输出使能
	TIM_BDTRInitStruct.TIM_Break=TIM_Break_Disable;//禁止刹车输入
	TIM_BDTRInitStruct.TIM_BreakPolarity=TIM_BreakPolarity_High;//刹车输入极性
	TIM_BDTRInitStruct.TIM_DeadTime=dead; //死区寄存器存放的数值
	TIM_BDTRInitStruct.TIM_LOCKLevel=TIM_LOCKLevel_OFF;//锁定关闭，寄存器无写保护
	TIM_BDTRInitStruct.TIM_OSSIState=TIM_OSSIState_Disable;
	TIM_BDTRInitStruct.TIM_OSSRState=TIM_OSSRState_Disable;
	TIM_BDTRConfig(TIM8,&TIM_BDTRInitStruct);
	
	//TIM_Cmd(TIM8,ENABLE); //定时器1使能
	
			LCD_ShowString(0,360,480,24,24,"                                                        ");//相当于局部清屏
			LCD_ShowNum(0,360,period,8,24);
			LCD_ShowNum(200,360,prescaler,4,24);
		//	LCD_ShowNum(270,210,pulse,10,24);
			
			//LCD_ShowString(0,420,480,24,24,"                                                        ");//相当于局部清屏
			LCD_ShowNum(320,360,dead,10,24);//显示计算后写入死区的值（0---65535）
		//	LCD_ShowNum(320,390,sign,4,24);//死区标志参考
}


//各种初始化（GPIO，定时器，通道，死区）
void TIM8_PWM_Init()
{
	TIM_OCInitTypeDef TIM_OCInitStruct;//通道结构体
	GPIO_InitTypeDef GPIO_InitStruct;//GPIO结构体
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//定时器结构体
	TIM_BDTRInitTypeDef TIM_BDTRInitStruct;//死区结构体

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);//定时器8时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC,ENABLE);//开启GPIOA和GPIOB的时钟
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    //复用时钟使能 
	
	//GPIO初始化设置
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;	//复用推挽输出模式
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_7;	//引脚为PA7
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;	//复用推挽输出模式
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_14;	//互补通道引脚为PB0
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//复用
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM8);

	//定时器TIM1初始化
	TIM_TimeBaseInitStruct.TIM_ClockDivision= 0;	//时钟分割0x0000
	//TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_CenterAligned1;	//【也是向上计数？？？？？？？？？？？？？？？？？】
	TIM_TimeBaseInitStruct.TIM_Period=419;	//自动重装的初值【周期】【65535时，对应最小频率】
	TIM_TimeBaseInitStruct.TIM_Prescaler=0;	//【预分频值】【分频系数】
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;	//重复计数设置为0
	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseInitStruct);

	// 通道初始化设置

	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNPolarity=TIM_OCNPolarity_High;							//反向输出极性选择
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;	//输出极性选择
	TIM_OCInitStruct.TIM_OutputNState=TIM_OutputNState_Enable;						//反向输出通道使能
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//输出通道使能
	
	TIM_OCInitStruct.TIM_Pulse=210;		//用来调节输出脉冲的占空比(1/4)
	TIM_OC1Init(TIM8,&TIM_OCInitStruct);	//		【选择CH1】
	TIM_OC1PreloadConfig(TIM8,TIM_OCPreload_Enable);  //比较寄存器（TIM1->CCR1）重装载值使能 
	
	TIM_OCInitStruct.TIM_Pulse=210;		//用来调节输出脉冲的占空比(1/2)
	TIM_OC2Init(TIM8,&TIM_OCInitStruct);	//		【选择CH2】
	TIM_OC2PreloadConfig(TIM8,TIM_OCPreload_Enable);
	
	
	//死区初始化设置
	TIM_BDTRInitStruct.TIM_AutomaticOutput=TIM_AutomaticOutput_Enable;//主动输出使能
	TIM_BDTRInitStruct.TIM_Break=TIM_Break_Disable;//禁止刹车输入
	TIM_BDTRInitStruct.TIM_BreakPolarity=TIM_BreakPolarity_High;//刹车输入极性
	TIM_BDTRInitStruct.TIM_DeadTime=0x00; //第一次初始化死区为0 
	TIM_BDTRInitStruct.TIM_LOCKLevel=TIM_LOCKLevel_OFF;//锁定关闭，寄存器无写保护
	TIM_BDTRInitStruct.TIM_OSSIState=TIM_OSSIState_Disable;
	TIM_BDTRInitStruct.TIM_OSSRState=TIM_OSSRState_Disable;
	TIM_BDTRConfig(TIM8,&TIM_BDTRInitStruct);
	

	TIM_ARRPreloadConfig(TIM8,ENABLE); //定时器1重装载使能
	TIM_CtrlPWMOutputs(TIM8,ENABLE);//pwm输出使能
	TIM_Cmd(TIM8,ENABLE); //定时器1使能

}
