#include "IO_Init.h"
#include "sys.h"

#define PC6_IN PCin(6)
#define PC7_IN PCin(7)
#define PC8_IN PCin(8)
#define PG6_IN PGin(6)
#define PG7_IN PGin(7)
#define PG8_IN PGin(8)
///////////////////////////////////////////
//@��ת���뿪�����


////////////////////////////////////////////
//IO�ڳ�ʼ������
void IO_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;	//GPIO��Ա�����ṹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOA,ENABLE);//ʹ��GPIOE��ʱ��
	
	//�ṹ���Ա��������
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;	//����ģʽ
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;	//����������Ϊ�͵�ƽ
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_9|GPIO_Pin_8;	//�жϴ����ܽ�
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;	//�����жϺ���Ҫ��ѯ�Ĺܽ�
	GPIO_Init(GPIOG,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_DOWN;	//����������Ϊ�ߵ�ƽ
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;//����ѡ���л��������ж����ȼ���Ҫ��
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	
}

//����IO�����ж�ӳ���ϵ�������г�ʼ��
void SetCorection(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;	//�����жϳ�Ա�����ṹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);	//ʹ�õ��ⲿ�жϣ��ͱ���� SYSCFG ʱ��
	
	//���� GPIO ���ж��ߵ�ӳ���ϵ
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource6);	//TIM3_CH1
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource9);//TIM3_CH2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource8);//TIM3_CH3
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);//�����ж�KEY_UP
	
	EXTI_InitStruct.EXTI_Line=EXTI_Line6|EXTI_Line9|EXTI_Line8;	//�ж���Ϊ6��7��8
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;	//�ж��¼�
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;	//������ʽΪ�½���
	EXTI_Init(&EXTI_InitStruct);	
	
	EXTI_InitStruct.EXTI_Line=EXTI_Line0;	//�ж���Ϊ6��7��8
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;	//�ж��¼�
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising;	//������ʽΪ������
	EXTI_Init(&EXTI_InitStruct);	
}

//�����ж����ȼ�����
void ExtiGpoup(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI9_5_IRQn;		//�ж�ͨ��	��ͨ��line5----line9������һ�����жϷ����������жϾ�����һ��������
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x03;	//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x02;	//��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI0_IRQn;		//�����ж�ͨ��	
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x02;	//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x01;	//��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStruct);
}
