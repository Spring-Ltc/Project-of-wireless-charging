#include "IO_Init.h"
#include "sys.h"

#define PC6_IN PCin(6)


//IO口初始化设置
void IO_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;	//GPIO成员变量结构体
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOG,ENABLE);//使能GPIOE的时钟
	
	//结构体成员变量设置
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;	//输入模式
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;	//上拉，按下为低电平
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6;	//PC6为中断触发管脚
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8;	//PG8为触发中断后需要查询的管脚
	GPIO_Init(GPIOG,&GPIO_InitStruct);
}


//设置IO口与中断映射关系，并进行初始化
void SetCorection(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;	//线上中断成员变量结构体
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);	//使用到外部中断，就必须打开 SYSCFG 时钟
	
	//配置 GPIO 与中断线的映射关系
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource6);	//TIM3_CH1
	//SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource3);
	//SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource4);
	
	EXTI_InitStruct.EXTI_Line=EXTI_Line6;//|EXTI_Line3|EXTI_Line4;	//中断线为2、3、4
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;	//中断事件
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;	//触发方式为下降沿
	EXTI_Init(&EXTI_InitStruct);
		
}

//配置中断分组
void ExtiGpoup(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI9_5_IRQn;		//中断通道	【怎么确定是那个通道？？？？】
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x03;	//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x02;	//响应优先级
	NVIC_Init(&NVIC_InitStruct);
	/*
	NVIC_InitStruct.NVIC_IRQChannel=EXTI3_IRQn;		//中断通道	【怎么确定是那个通道？？？？】
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x02;	//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x02;	//响应优先级
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI4_IRQn;		//中断通道	【怎么确定是那个通道？？？？】
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x01;	//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x02;	//响应优先级
	NVIC_Init(&NVIC_InitStruct);
	*/
}
