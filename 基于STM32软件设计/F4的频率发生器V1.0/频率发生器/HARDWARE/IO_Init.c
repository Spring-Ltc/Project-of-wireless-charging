#include "IO_Init.h"
#include "sys.h"

#define PC6_IN PCin(6)


//IO�ڳ�ʼ������
void IO_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;	//GPIO��Ա�����ṹ��
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOG,ENABLE);//ʹ��GPIOE��ʱ��
	
	//�ṹ���Ա��������
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;	//����ģʽ
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;	//����������Ϊ�͵�ƽ
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6;	//PC6Ϊ�жϴ����ܽ�
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8;	//PG8Ϊ�����жϺ���Ҫ��ѯ�Ĺܽ�
	GPIO_Init(GPIOG,&GPIO_InitStruct);
}


//����IO�����ж�ӳ���ϵ�������г�ʼ��
void SetCorection(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;	//�����жϳ�Ա�����ṹ��
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);	//ʹ�õ��ⲿ�жϣ��ͱ���� SYSCFG ʱ��
	
	//���� GPIO ���ж��ߵ�ӳ���ϵ
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource6);	//TIM3_CH1
	//SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource3);
	//SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource4);
	
	EXTI_InitStruct.EXTI_Line=EXTI_Line6;//|EXTI_Line3|EXTI_Line4;	//�ж���Ϊ2��3��4
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;	//�ж��¼�
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;	//������ʽΪ�½���
	EXTI_Init(&EXTI_InitStruct);
		
}

//�����жϷ���
void ExtiGpoup(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI9_5_IRQn;		//�ж�ͨ��	����ôȷ�����Ǹ�ͨ������������
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x03;	//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x02;	//��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStruct);
	/*
	NVIC_InitStruct.NVIC_IRQChannel=EXTI3_IRQn;		//�ж�ͨ��	����ôȷ�����Ǹ�ͨ������������
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x02;	//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x02;	//��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI4_IRQn;		//�ж�ͨ��	����ôȷ�����Ǹ�ͨ������������
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x01;	//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x02;	//��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStruct);
	*/
}
