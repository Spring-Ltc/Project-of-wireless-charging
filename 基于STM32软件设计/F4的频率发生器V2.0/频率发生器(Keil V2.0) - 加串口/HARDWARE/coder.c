#include "coder.h"


////////////////////
//��ʱ��TIM4��ʼ��������Ϊ������ģʽ
void TIM4_CoderInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;//GPIO�ṹ��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//��ʱ���ṹ��
	TIM_ICInitTypeDef TIM_ICInitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	//IO�ڹܽų�ʼ��
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF ;//����IOģʽ
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP ;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;//TIM4��CH1��CH2ͨ����Ӧ�ܽ�ΪPB6\PB7
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP ;//�����������ͨ��Ϊ�͵�ƽ
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz ;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//ӳ�䵽��ʱ����
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_TIM4);
	
	//��ʱ����ʼ������
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;//���ϼ���ģʽ
	TIM_TimeBaseInitStruct.TIM_Period=0xffff;
	TIM_TimeBaseInitStruct.TIM_Prescaler=0x0000;
	//TIM_TimeBaseInitStruct.TIM_RepetitionCounter=
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStruct);
	
	//����TIMx�������ӿ�				��ʱ��			����ģʽ					ͨ��1������������						ͨ��2������������
	TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI1,TIM_ICPolarity_Falling,TIM_ICPolarity_Falling);
	
	TIM_ICStructInit(&TIM_ICInitStruct);
	TIM_ICInitStruct.TIM_ICFilter=10;
	TIM_ICInit(TIM4,&TIM_ICInitStruct);
	TIM_ClearFlag(TIM4,TIM_FLAG_Update);
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	
	TIM4->CNT = 0;
  TIM_Cmd(TIM4, ENABLE);
}

