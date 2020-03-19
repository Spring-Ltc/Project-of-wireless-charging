#include "Key.h"
#include "delay.h"

/////////////////////////////////////
//������ʼ����أ�4�����ذ�����һ�����뿪�ذ���
//����KEY_UP���ͱ��뿪�ذ���ӳ��Ϊ�����ж�
//@@@:�ò��ֵ�������OK
//���뿪�ذ���ΪPB5,
void Key_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;	//�����жϳ�Ա�����ṹ��
	GPIO_InitTypeDef  GPIO_InitStructure;//GPIO
	NVIC_InitTypeDef NVIC_InitStruct;//�ж����ȼ�
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��GPIOA,GPIOEʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);	//ʹ�õ��ⲿ�жϣ��ͱ���� SYSCFG ʱ��
	
	
	
	//������ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4; //KEY0 KEY1 KEY2��Ӧ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//WK_UP��Ӧ����PA0
	//GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//���췽ʽ��check:����Ϊ���ʱ�����ø�ѡ�
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA0
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//������������Ӧ�ܽ�
	//GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//���췽ʽ
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB5
	
	//���� GPIO ���ж��ߵ�ӳ���ϵ
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);//���ѡ���ж�
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource5);//ģʽѡ���ж�

	
	//�ж��߳�ʼ��
	EXTI_InitStruct.EXTI_Line=EXTI_Line3|EXTI_Line5;	//�ж���Ϊ5
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;	//�ж��¼�
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;	//������ʽΪ�½���
	EXTI_Init(&EXTI_InitStruct);
	
	EXTI_InitStruct.EXTI_Line=EXTI_Line0;	//�ж���Ϊ0
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;	//�ж��¼�
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising;	//������ʽΪ������
	EXTI_Init(&EXTI_InitStruct);
	
	//�ж�ͨ������
	NVIC_InitStruct.NVIC_IRQChannel=EXTI0_IRQn;		//�ж�ͨ��0
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x02;	//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x01;	//��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStruct);
	

	NVIC_InitStruct.NVIC_IRQChannel=EXTI9_5_IRQn;		//�ж�ͨ����5--9�����ж�ͨ����
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x03;	//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x02;	//��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStruct);
}


//������ֵ��ȡ����
u8 key_scanf(void)
{
	if(KEY0==0||KEY1==0||KEY2==0||WK_UP==1)
	{
		delay_ms(5);//��������
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
	return 0;	//û�а������·���ֵΪ0	
}








