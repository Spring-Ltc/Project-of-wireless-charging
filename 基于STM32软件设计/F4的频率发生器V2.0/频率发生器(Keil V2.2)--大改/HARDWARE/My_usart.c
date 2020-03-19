#include "My_usart.h"
#include "sys.h"

#if style1
void My_UsartInit1(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;//GPIO初始化结构体
	USART_InitTypeDef USART_InitStruct;//串口相关变量结构体
	NVIC_InitTypeDef NVIC_InitStruct;//中断相关变量结构体
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//串口时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//IO时钟使能
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);//管脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	//GPIO初始化设置
	GPIO_InitStruct.GPIO_Mode =GPIO_Mode_AF;//复用模式
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//串口初始化设置
	USART_InitStruct.USART_BaudRate=115200;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity=USART_Parity_No;
	USART_InitStruct.USART_StopBits=USART_StopBits_1;
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;//字长
	USART_Init(USART1,&USART_InitStruct);
	
	USART_Cmd(USART1,ENABLE);//使能串口
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//开串口中断【只开了接收中断】
	
	//中断优先级相关初始化设置
	NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;//通道选择
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x03;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x02;
	NVIC_Init(&NVIC_InitStruct);
	
}
#endif


#if style3
void My_UsartInit(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;	//线上中断成员变量结构体
	GPIO_InitTypeDef GPIO_InitStruct;//GPIO初始化结构体
	NVIC_InitTypeDef NVIC_InitStruct;//中断相关变量结构体

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC,ENABLE);//IO时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);	//使用到外部中断，就必须打开 SYSCFG 时钟
	


	
	//GPIO初始化设置
	GPIO_InitStruct.GPIO_Mode =GPIO_Mode_OUT;//复用模式
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_25MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode =GPIO_Mode_IN;//复用模式
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_25MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource11);//模式选择中断
	
	//中断线初始化
	EXTI_InitStruct.EXTI_Line=EXTI_Line11;	//中断线为5
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;	//中断事件
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising_Falling;	//触发方式为跳变沿
	EXTI_Init(&EXTI_InitStruct);

	//中断通道设置
	NVIC_InitStruct.NVIC_IRQChannel=EXTI15_10_IRQn;		//中断通道11
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x02;	//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x01;	//响应优先级
	NVIC_Init(&NVIC_InitStruct);

}
#endif
