#include "IO_Init.h"
#include "sys.h"

#define PC6_IN PCin(6)
#define PC7_IN PCin(7)
#define PC8_IN PCin(8)
#define PG6_IN PGin(6)
#define PG7_IN PGin(7)
#define PG8_IN PGin(8)
///////////////////////////////////////////
//@旋转编码开关相关


////////////////////////////////////////////
//IO口初始化设置
void IO_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;	//GPIO成员变量结构体
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOA,ENABLE);//使能GPIOE的时钟
	
	//结构体成员变量设置
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;	//输入模式
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;	//上拉，按下为低电平
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_9|GPIO_Pin_8;	//中断触发管脚
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;	//触发中断后需要查询的管脚
	GPIO_Init(GPIOG,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_DOWN;	//下拉，按下为高电平
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;//数据选择切换按键，中断优先级需要高
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	
}

//设置IO口与中断映射关系，并进行初始化
void SetCorection(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;	//线上中断成员变量结构体
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);	//使用到外部中断，就必须打开 SYSCFG 时钟
	
	//配置 GPIO 与中断线的映射关系
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource6);	//TIM3_CH1
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource9);//TIM3_CH2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource8);//TIM3_CH3
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);//按键中断KEY_UP
	
	EXTI_InitStruct.EXTI_Line=EXTI_Line6|EXTI_Line9|EXTI_Line8;	//中断线为6、7、8
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;	//中断事件
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;	//触发方式为下降沿
	EXTI_Init(&EXTI_InitStruct);	
	
	EXTI_InitStruct.EXTI_Line=EXTI_Line0;	//中断线为6、7、8
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;	//中断事件
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising;	//触发方式为上升沿
	EXTI_Init(&EXTI_InitStruct);	
}

//配置中断优先级分组
void ExtiGpoup(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI9_5_IRQn;		//中断通道	【通道line5----line9共用这一个，中断服务函数里面判断具体哪一个触发】
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x03;	//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x02;	//响应优先级
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI0_IRQn;		//按键中断通道	
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x02;	//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x01;	//响应优先级
	NVIC_Init(&NVIC_InitStruct);
}
