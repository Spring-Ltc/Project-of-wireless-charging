#include "My_usart.h"
#include "sys.h"

#if style1
void My_UsartInit1(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;//GPIO��ʼ���ṹ��
	USART_InitTypeDef USART_InitStruct;//������ر����ṹ��
	NVIC_InitTypeDef NVIC_InitStruct;//�ж���ر����ṹ��
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//����ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//IOʱ��ʹ��
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);//�ܽŸ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	//GPIO��ʼ������
	GPIO_InitStruct.GPIO_Mode =GPIO_Mode_AF;//����ģʽ
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//���ڳ�ʼ������
	USART_InitStruct.USART_BaudRate=115200;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity=USART_Parity_No;
	USART_InitStruct.USART_StopBits=USART_StopBits_1;
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;//�ֳ�
	USART_Init(USART1,&USART_InitStruct);
	
	USART_Cmd(USART1,ENABLE);//ʹ�ܴ���
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//�������жϡ�ֻ���˽����жϡ�
	
	//�ж����ȼ���س�ʼ������
	NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;//ͨ��ѡ��
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x03;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x02;
	NVIC_Init(&NVIC_InitStruct);
	
}
#endif


#if style3
void My_UsartInit(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;	//�����жϳ�Ա�����ṹ��
	GPIO_InitTypeDef GPIO_InitStruct;//GPIO��ʼ���ṹ��
	NVIC_InitTypeDef NVIC_InitStruct;//�ж���ر����ṹ��

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC,ENABLE);//IOʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);	//ʹ�õ��ⲿ�жϣ��ͱ���� SYSCFG ʱ��
	


	
	//GPIO��ʼ������
	GPIO_InitStruct.GPIO_Mode =GPIO_Mode_OUT;//����ģʽ
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_25MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode =GPIO_Mode_IN;//����ģʽ
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_25MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource11);//ģʽѡ���ж�
	
	//�ж��߳�ʼ��
	EXTI_InitStruct.EXTI_Line=EXTI_Line11;	//�ж���Ϊ5
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;	//�ж��¼�
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising_Falling;	//������ʽΪ������
	EXTI_Init(&EXTI_InitStruct);

	//�ж�ͨ������
	NVIC_InitStruct.NVIC_IRQChannel=EXTI15_10_IRQn;		//�ж�ͨ��11
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x02;	//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x01;	//��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStruct);

}
#endif
