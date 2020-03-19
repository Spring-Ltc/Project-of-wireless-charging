
/****************��ʱ��TIM1����Դ����*************************************/

//ӳ��ǰ��
//		TIM1_CH1		PE9	,					TIM1_CH1N		PE8	;
//		TIM1_CH2		PE11	,				TIM1_CH2N		PE10;
//		TIM1_CH3		PE13	,				TIM1_CH3N		PE12;
//		TIM1_CH4		PE14	,				


//ӳ���


/**************************************************************************/

/****************��ʱ��TIM8����Դ����*************************************/

//ӳ��ǰ��
//		TIM8_CH1		PC6	,				TIM8_CH1N		PA5/PA7	;
//		TIM8_CH2		PC7	,				TIM8_CH2N		PB0/PB14;
//		TIM8_CH3		PC8	,				TIM8_CH3N		PB1/PB15;
//		TIM8_CH4		PC9	,	
/**************************************************************************/


//�������TIM8��CH2ͨ������������Ρ�PC7��PB14��


#include "pwm.h"
#include "lcd.h"
#define Count_Clock 84000000	//��ʱ��ʱ��Ƶ��84MHz
//#define Count_Clock 168000000	//��ʱ��ʱ��Ƶ��168MHz

//�ı�Ƶ�ʺ�ռ�ձȺ�����ʱ��
//�൱�ڶԶ�ʱ�������ͨ�����³�ʼ��һ��
void SetPwmVal(u32 Freq,u32 Duty,u32 Dead)
{
	//TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//��ʱ���ṹ��
	//TIM_OCInitTypeDef TIM_OCInitStruct;//ͨ���ṹ��
	TIM_BDTRInitTypeDef TIM_BDTRInitStruct;//�����ṹ��

	u32 period;
	u32 prescaler;
	u16 pulse;
	u32 dead;
	
	u8 sign=0;//������Χ��־
	
	
	//�ж�Ƶ�ʷ�Χ
	if(Freq<3000)//��Ƶ��Ϊ��߾��ȣ�Ҫ��Ƶ
	{
		prescaler=3000/Freq;
		period=Count_Clock/(Freq*(prescaler+1))-1;
	}
	else
	{
		prescaler=0;
		period=Count_Clock/Freq-1;
	}
	pulse=(period+1)*Duty/100;	//�����������ֵ����ռ�ձȣ�
	
	//�ж�����ʱ�䷶Χ
	if(Dead<=756)//���һ
	{
		dead=Dead/5.95;sign=1;
	}
	else if(Dead<=1512)//�����
	{
		dead=((Dead/2)/5.95)+128+0-64;sign=2;
	}
	else if(Dead<=3000)//�����
	{
		dead=((Dead/8)/5.95)+128+64+0-32;sign=3;
	}
	else if(Dead<=6000)//�����
	{
		dead=Dead/16/5.95-32+128+64+32;sign=4;
	}

	
	//2019/09/08/14:56�޸�Ϊ�������ã�ֱ���޸���Ӧ�Ĵ���������ȡ�����³�ʼ��
	//											[ע��Ҫ����һ�γ�ʼ���ķ�Ƶϵ����Ϊ0����������ļ�����ʽ]
	TIM_SetAutoreload(TIM8,period);//���������Զ���װ��ֵ
	TIM_SetCompare1(TIM8,pulse);
	TIM_SetCompare2(TIM8,pulse);

	
	/*
//��ʱ��TIM8��ʼ��
	TIM_TimeBaseInitStruct.TIM_ClockDivision= 0;	//ʱ�ӷָ�
	//TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;	//���ϼ���
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_CenterAligned1;	//��Ҳ�����ϼ���������������������������������������
	TIM_TimeBaseInitStruct.TIM_Period=period;	//�Զ���װ�ĳ�ֵ�����ڡ�
	TIM_TimeBaseInitStruct.TIM_Prescaler=prescaler;	//��Ԥ��Ƶֵ������Ƶϵ����
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;	//�ظ���������Ϊ0
	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseInitStruct);

	// ͨ����ʼ������

	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNPolarity=TIM_OCNPolarity_High;							//�����������ѡ��
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;	//�������ѡ��
	TIM_OCInitStruct.TIM_OutputNState=TIM_OutputNState_Enable;						//�������ͨ��ʹ��
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//���ͨ��ʹ��
	
	TIM_OCInitStruct.TIM_Pulse=pulse;		//����������������ռ�ձ�(1/4)
	TIM_OC1Init(TIM8,&TIM_OCInitStruct);	//		��ѡ��CH1��
	TIM_OC1PreloadConfig(TIM8,TIM_OCPreload_Enable);  //�ȽϼĴ�����TIM1->CCR1����װ��ֵʹ�� 
	
	TIM_OCInitStruct.TIM_Pulse=pulse;		//����������������ռ�ձ�(1/2)
	TIM_OC2Init(TIM8,&TIM_OCInitStruct);	//		��ѡ��CH2��
	TIM_OC2PreloadConfig(TIM8,TIM_OCPreload_Enable);
	*/
	
	TIM_BDTRInitStruct.TIM_AutomaticOutput=TIM_AutomaticOutput_Enable;//�������ʹ��
	TIM_BDTRInitStruct.TIM_Break=TIM_Break_Disable;//��ֹɲ������
	TIM_BDTRInitStruct.TIM_BreakPolarity=TIM_BreakPolarity_High;//ɲ�����뼫��
	TIM_BDTRInitStruct.TIM_DeadTime=dead; //�����Ĵ�����ŵ���ֵ
	TIM_BDTRInitStruct.TIM_LOCKLevel=TIM_LOCKLevel_OFF;//�����رգ��Ĵ�����д����
	TIM_BDTRInitStruct.TIM_OSSIState=TIM_OSSIState_Disable;
	TIM_BDTRInitStruct.TIM_OSSRState=TIM_OSSRState_Disable;
	TIM_BDTRConfig(TIM8,&TIM_BDTRInitStruct);
	
	//TIM_Cmd(TIM8,ENABLE); //��ʱ��1ʹ��
	
			LCD_ShowString(0,360,480,24,24,"                                                        ");//�൱�ھֲ�����
			LCD_ShowNum(0,360,period,8,24);
			LCD_ShowNum(200,360,prescaler,4,24);
		//	LCD_ShowNum(270,210,pulse,10,24);
			
			//LCD_ShowString(0,420,480,24,24,"                                                        ");//�൱�ھֲ�����
			LCD_ShowNum(320,360,dead,10,24);//��ʾ�����д��������ֵ��0---65535��
		//	LCD_ShowNum(320,390,sign,4,24);//������־�ο�
}


//���ֳ�ʼ����GPIO����ʱ����ͨ����������
void TIM8_PWM_Init()
{
	TIM_OCInitTypeDef TIM_OCInitStruct;//ͨ���ṹ��
	GPIO_InitTypeDef GPIO_InitStruct;//GPIO�ṹ��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//��ʱ���ṹ��
	TIM_BDTRInitTypeDef TIM_BDTRInitStruct;//�����ṹ��

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);//��ʱ��8ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC,ENABLE);//����GPIOA��GPIOB��ʱ��
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    //����ʱ��ʹ�� 
	
	//GPIO��ʼ������
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;	//�����������ģʽ
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_7;	//����ΪPA7
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;	//�����������ģʽ
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_14;	//����ͨ������ΪPB0
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//����
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM8);

	//��ʱ��TIM1��ʼ��
	TIM_TimeBaseInitStruct.TIM_ClockDivision= 0;	//ʱ�ӷָ�0x0000
	//TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;	//���ϼ���
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_CenterAligned1;	//��Ҳ�����ϼ���������������������������������������
	TIM_TimeBaseInitStruct.TIM_Period=419;	//�Զ���װ�ĳ�ֵ�����ڡ���65535ʱ����Ӧ��СƵ�ʡ�
	TIM_TimeBaseInitStruct.TIM_Prescaler=0;	//��Ԥ��Ƶֵ������Ƶϵ����
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;	//�ظ���������Ϊ0
	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseInitStruct);

	// ͨ����ʼ������

	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNPolarity=TIM_OCNPolarity_High;							//�����������ѡ��
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;	//�������ѡ��
	TIM_OCInitStruct.TIM_OutputNState=TIM_OutputNState_Enable;						//�������ͨ��ʹ��
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//���ͨ��ʹ��
	
	TIM_OCInitStruct.TIM_Pulse=210;		//����������������ռ�ձ�(1/4)
	TIM_OC1Init(TIM8,&TIM_OCInitStruct);	//		��ѡ��CH1��
	TIM_OC1PreloadConfig(TIM8,TIM_OCPreload_Enable);  //�ȽϼĴ�����TIM1->CCR1����װ��ֵʹ�� 
	
	TIM_OCInitStruct.TIM_Pulse=210;		//����������������ռ�ձ�(1/2)
	TIM_OC2Init(TIM8,&TIM_OCInitStruct);	//		��ѡ��CH2��
	TIM_OC2PreloadConfig(TIM8,TIM_OCPreload_Enable);
	
	
	//������ʼ������
	TIM_BDTRInitStruct.TIM_AutomaticOutput=TIM_AutomaticOutput_Enable;//�������ʹ��
	TIM_BDTRInitStruct.TIM_Break=TIM_Break_Disable;//��ֹɲ������
	TIM_BDTRInitStruct.TIM_BreakPolarity=TIM_BreakPolarity_High;//ɲ�����뼫��
	TIM_BDTRInitStruct.TIM_DeadTime=0x00; //��һ�γ�ʼ������Ϊ0 
	TIM_BDTRInitStruct.TIM_LOCKLevel=TIM_LOCKLevel_OFF;//�����رգ��Ĵ�����д����
	TIM_BDTRInitStruct.TIM_OSSIState=TIM_OSSIState_Disable;
	TIM_BDTRInitStruct.TIM_OSSRState=TIM_OSSRState_Disable;
	TIM_BDTRConfig(TIM8,&TIM_BDTRInitStruct);
	

	TIM_ARRPreloadConfig(TIM8,ENABLE); //��ʱ��1��װ��ʹ��
	TIM_CtrlPWMOutputs(TIM8,ENABLE);//pwm���ʹ��
	TIM_Cmd(TIM8,ENABLE); //��ʱ��1ʹ��

}
