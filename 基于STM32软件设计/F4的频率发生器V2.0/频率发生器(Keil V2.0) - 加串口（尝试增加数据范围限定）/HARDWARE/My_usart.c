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
	GPIO_InitTypeDef GPIO_InitStruct;//GPIO初始化结构体
	USART_InitTypeDef USART_InitStruct;//串口相关变量结构体
	NVIC_InitTypeDef NVIC_InitStruct;//中断相关变量结构体
	

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//串口时钟使能
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);//IO时钟使能

	

		GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);//管脚复用映射
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);

	
	//GPIO初始化设置
	GPIO_InitStruct.GPIO_Mode =GPIO_Mode_AF;//复用模式
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_11|GPIO_Pin_10;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_25MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);

	
	//串口初始化设置
	USART_InitStruct.USART_BaudRate=115200;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity=USART_Parity_No;
	USART_InitStruct.USART_StopBits=USART_StopBits_1;
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;//字长
	USART_Init(USART3,&USART_InitStruct);

	

	USART_Cmd(USART3,ENABLE);//使能串口
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);//开串口中断【只开了接收中断】

	
	//中断优先级相关初始化设置
	NVIC_InitStruct.NVIC_IRQChannel=USART3_IRQn;//通道选择
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x02;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x02;
	NVIC_Init(&NVIC_InitStruct);
	
}
#endif
