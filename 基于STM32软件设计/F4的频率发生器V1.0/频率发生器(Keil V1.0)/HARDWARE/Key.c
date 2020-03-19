#include "Key.h"
#include "delay.h"

/////////////////////////////////////
//按键初始化相关，4个板载按键和一个编码开关按键
//其中KEY_UP键和编码开关按键映射为线上中断
//@@@:该部分单独测试OK
//编码开关按键为PB5,
void Key_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;	//线上中断成员变量结构体
	GPIO_InitTypeDef  GPIO_InitStructure;//GPIO
	NVIC_InitTypeDef NVIC_InitStruct;//中断优先级
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOG, ENABLE);//使能GPIOA,GPIOE时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);	//使用到外部中断，就必须打开 SYSCFG 时钟
	
	
	
	//按键初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4; //KEY0 KEY1 KEY2对应引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE2,3,4

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//WK_UP对应引脚PA0
	//GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//推挽方式【check:配置为输出时才设置该选项】
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;//下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA0
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//编码器按键对应管脚
	//GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//推挽方式
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB5
	
	//配置 GPIO 与中断线的映射关系
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);//输出选择中断
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource5);//模式选择中断

	
	//中断线初始化
	EXTI_InitStruct.EXTI_Line=EXTI_Line3|EXTI_Line5;	//中断线为5
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;	//中断事件
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;	//触发方式为下降沿
	EXTI_Init(&EXTI_InitStruct);
	
	EXTI_InitStruct.EXTI_Line=EXTI_Line0;	//中断线为0
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;	//中断事件
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising;	//触发方式为上升沿
	EXTI_Init(&EXTI_InitStruct);
	
	//中断通道设置
	NVIC_InitStruct.NVIC_IRQChannel=EXTI0_IRQn;		//中断通道0
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x02;	//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x01;	//响应优先级
	NVIC_Init(&NVIC_InitStruct);
	

	NVIC_InitStruct.NVIC_IRQChannel=EXTI9_5_IRQn;		//中断通道【5--9共用中断通道】
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x03;	//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x02;	//响应优先级
	NVIC_Init(&NVIC_InitStruct);
}


//按键键值读取函数
u8 key_scanf(void)
{
	if(KEY0==0||KEY1==0||KEY2==0||WK_UP==1)
	{
		delay_ms(5);//按键消抖
		if(KEY0==0)
		{
			while(KEY0==0);
			return 1;
		}
		else if(KEY1==0)
		{
			while(KEY1==0);
			return 2;
		}
		else if(KEY2==0)
		{
			while(KEY2==0);
			return 3;
		}
		else if(WK_UP==1)
		{
			while(WK_UP==1);
			return 4;
		}
	}
	return 0;	//没有按键按下返回值为0	
}








