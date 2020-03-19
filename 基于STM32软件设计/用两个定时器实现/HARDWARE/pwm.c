

#include "pwm.h"

/*

void TIM1_PWM_Init()
{
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	pwm_gpioInit();	
	pwm_timeInit(900,0);
	
	///GPIO_PinRemapConfig(GPIO_PartialRemap_TIM1,ENABLE);	//TIM1������ӳ��
	
	// ͨ����ʼ������
	//TIM_OCInitStruct.TIM_OCIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM2;
	//TIM_OCInitStruct.TIM_OCNIdleState=
	//TIM_OCInitStruct.TIM_OCNPolarity=
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;
	//TIM_OCInitStruct.TIM_OutputNState=
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;
	//TIM_OCInitStruct.TIM_Pulse=
	TIM_OC1Init(TIM1,&TIM_OCInitStruct);
	
	
	//TIM1->CH1ͨ����PA8
	TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable); //ʹ��TIM1��CCR2�ϵ�Ԥװ�ؼĴ���
	
	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIM3
	
}

*/

void TIM1_PWM_Init()
{
	TIM_OCInitTypeDef TIM_OCInitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    //����ʱ��ʹ�� 
	
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;	//�����������ģʽ
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;	//����ΪPC13
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;	//�����������ģʽ
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_7;	//����ΪPC13
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    //����ʱ��ʹ�� 
	///GPIO_PinRemapConfig(GPIO_PartialRemap_TIM1,ENABLE);	//TIM1������ӳ��
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision= 0;	//ʱ�ӷָ�
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;	//���ϼ���
	TIM_TimeBaseInitStruct.TIM_Period=71;	//�Զ���װ�ĳ�ֵ�����ڣ�����
	TIM_TimeBaseInitStruct.TIM_Prescaler=0;	//ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ����Ƶϵ����
	//TIM_TimeBaseInitStruct.TIM_RepetitionCounter=
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);

	TIM_TimeBaseInitStruct.TIM_Period = 7199; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler =0; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ



	// ͨ����ʼ������
	//TIM_OCInitStruct.TIM_OCIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM2;
	//TIM_OCInitStruct.TIM_OCNIdleState=
	//TIM_OCInitStruct.TIM_OCNPolarity=
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;
	//TIM_OCInitStruct.TIM_OutputNState=
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;
	//TIM_OCInitStruct.TIM_Pulse=
	TIM_OC1Init(TIM2,&TIM_OCInitStruct);	//		��ѡ��CH1��
	
		TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ը�
	TIM_OC2Init(TIM3, &TIM_OCInitStruct);  //����Tָ���Ĳ�����ʼ������TIM3 OC2

	
	//TIM1->CH1ͨ����PA8
	TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable); //ʹ��TIM1��CCR2�ϵ�Ԥװ�ؼĴ���
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
 
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3
	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIM3
	
}

//GPIO��ʼ��
void pwm_gpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//����ʱ��
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;	//�����������ģʽ
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;	//����ΪPC13
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    //����ʱ��ʹ�� 
}

//��ʱ����ʼ��
void pwm_timeInit(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision= 0;	//ʱ�ӷָ�
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;	//���ϼ���
	TIM_TimeBaseInitStruct.TIM_Period=arr-1;	//�Զ���װ�ĳ�ֵ�����ڣ�����
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc;	//ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ����Ƶϵ����
	//TIM_TimeBaseInitStruct.TIM_RepetitionCounter=
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);

	//TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM3�����ں���ȥ����
}



