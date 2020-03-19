/***************************************************

//Ĭ�ϸ��ã���ӳ��
TIM1_CH1 :	PA8,			TIM1_CH1N	: PB13
TIM1_CH2 :	PA9,			TIM1_CH2N	: PB14
TIM1_CH3 :	PA10,			TIM1_CH3N	: PB15
TIM1_CH4 :	PA11,
//��ӳ�����
											TIM1_CH1N	: PA7
											TIM1_CH2N	: PB0
											TIM1_CH3N	: PB1
		
*****************************************************/

#include "pwm.h"

#define Count_Clock 72000	//��ʱ��ʱ��Ƶ��


//�ı�Ƶ�ʺ�ռ�ձ�
//�൱�ڶԶ�ʱ�������ͨ�����³�ʼ��һ��
void SetPwmVal(u32 Freq,u16 Duty)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//��ʱ���ṹ��
	TIM_OCInitTypeDef TIM_OCInitStruct;//ͨ���ṹ��
	
	u32 period;
	u16 pulse;
	
	period=Count_Clock/Freq-1;	//����������ڣ�������ΪƵ�ʣ�
	pulse=(period+1)*Duty/100;	//�����������ֵ����ռ�ձȣ�

	
//��ʱ��TIM1��ʼ��
	TIM_TimeBaseInitStruct.TIM_ClockDivision= 0;	//ʱ�ӷָ�
	//TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;	//���ϼ���
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_CenterAligned1;	//��Ҳ�����ϼ���������������������������������������
	TIM_TimeBaseInitStruct.TIM_Period=period;	//�Զ���װ�ĳ�ֵ�����ڡ�
	TIM_TimeBaseInitStruct.TIM_Prescaler=0;	//��Ԥ��Ƶֵ������Ƶϵ����
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;	//�ظ���������Ϊ0
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);

	// ͨ����ʼ������

	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNPolarity=TIM_OCNPolarity_High;							//�����������ѡ��
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;	//�������ѡ��
	TIM_OCInitStruct.TIM_OutputNState=TIM_OutputNState_Enable;						//�������ͨ��ʹ��
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//���ͨ��ʹ��
	
	TIM_OCInitStruct.TIM_Pulse=pulse;		//����������������ռ�ձ�(1/4)
	TIM_OC1Init(TIM1,&TIM_OCInitStruct);	//		��ѡ��CH1��
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);  //�ȽϼĴ�����TIM1->CCR1����װ��ֵʹ�� 
	
	TIM_OCInitStruct.TIM_Pulse=pulse;		//����������������ռ�ձ�(1/2)
	TIM_OC2Init(TIM1,&TIM_OCInitStruct);	//		��ѡ��CH2��
	TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Enable);
	
	TIM_Cmd(TIM1,ENABLE); //��ʱ��1ʹ��
}


//���ֳ�ʼ����GPIO����ʱ����ͨ����������
void TIM1_PWM_Init()
{
	TIM_OCInitTypeDef TIM_OCInitStruct;//ͨ���ṹ��
	GPIO_InitTypeDef GPIO_InitStruct;//GPIO�ṹ��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//��ʱ���ṹ��
	TIM_BDTRInitTypeDef TIM_BDTRInitStruct;//�����ṹ��

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);//��ʱ��1ʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);//����GPIOA��GPIOB��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    //����ʱ��ʹ�� 
	
	//GPIO��ʼ������
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;	//�����������ģʽ
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8;	//����ΪPA8
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;	//�����������ģʽ
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_13;	//����ͨ������ΪPB13
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);

	//��ʱ��TIM1��ʼ��
	TIM_TimeBaseInitStruct.TIM_ClockDivision= 0;	//ʱ�ӷָ�
	//TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;	//���ϼ���
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_CenterAligned1;	//��Ҳ�����ϼ���������������������������������������
	TIM_TimeBaseInitStruct.TIM_Period=7199;	//�Զ���װ�ĳ�ֵ�����ڡ�
	TIM_TimeBaseInitStruct.TIM_Prescaler=1;	//��Ԥ��Ƶֵ������Ƶϵ����
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;	//�ظ���������Ϊ0
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);

	// ͨ����ʼ������

	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNPolarity=TIM_OCNPolarity_High;							//�����������ѡ��
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;	//�������ѡ��
	TIM_OCInitStruct.TIM_OutputNState=TIM_OutputNState_Enable;						//�������ͨ��ʹ��
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//���ͨ��ʹ��
	
	TIM_OCInitStruct.TIM_Pulse=3600;		//����������������ռ�ձ�(1/4)
	TIM_OC1Init(TIM1,&TIM_OCInitStruct);	//		��ѡ��CH1��
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);  //�ȽϼĴ�����TIM1->CCR1����װ��ֵʹ�� 
	
	TIM_OCInitStruct.TIM_Pulse=3600;		//����������������ռ�ձ�(1/2)
	TIM_OC2Init(TIM1,&TIM_OCInitStruct);	//		��ѡ��CH2��
	TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Enable);
	
	//������ʼ������
	TIM_BDTRInitStruct.TIM_AutomaticOutput=TIM_AutomaticOutput_Enable;//�������ʹ��
	TIM_BDTRInitStruct.TIM_Break=TIM_Break_Disable;//��ֹɲ������
	TIM_BDTRInitStruct.TIM_BreakPolarity=TIM_BreakPolarity_High;//ɲ�����뼫��
	TIM_BDTRInitStruct.TIM_DeadTime=28; //�����Ĵ�����ŵ���ֵΪ28 
	TIM_BDTRInitStruct.TIM_LOCKLevel=TIM_LOCKLevel_OFF;//�����رգ��Ĵ�����д����
	TIM_BDTRInitStruct.TIM_OSSIState=TIM_OSSIState_Disable;
	TIM_BDTRInitStruct.TIM_OSSRState=TIM_OSSRState_Disable;
	TIM_BDTRConfig(TIM1,&TIM_BDTRInitStruct);
	

	TIM_ARRPreloadConfig(TIM1,ENABLE); //��ʱ��1��װ��ʹ��
	TIM_CtrlPWMOutputs(TIM1,ENABLE);//pwm���ʹ��
	TIM_Cmd(TIM1,ENABLE); //��ʱ��1ʹ��

}
