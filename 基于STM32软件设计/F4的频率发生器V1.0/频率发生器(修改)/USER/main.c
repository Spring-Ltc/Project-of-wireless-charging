//////////////////////////////////////////////////////////////////
//@4.3����������ֱ���800*480	�������Ͻǵ�����Ϊ��0,0���������½ǵ�����Ϊ��480,800����
//@�������ͨ����
//@Ƶ�ʵ��ڡ���ת���뿪�أ��ж�PC6,			��ѯPG6,		��������			KEY2�Ӵ󲽽�ֵ
//@ռ�ձȵ��ڡ���ת���뿪�أ��ж�PC9,		��ѯPG7,		��������			KEY1�Ӵ󲽽�ֵ
//@����ʱ����ڡ���ת���뿪�أ��ж�PC8,		��ѯPG8,		��������			KEY0�Ӵ󲽽�ֵ
//@����ѡ����������ذ���KEY_UP�����л�
//////////////////////////////////////////////////////////////////
//@�жϲ���OK�������ʵ���������
//�����޸ĵ�����..................................
//////////////////////////////////////////////////////////////////
#include "stm32f4xx.h"
#include "LED.h"
#include "Key.h"
#include "IO_Init.h"
#include "BUZZ.h"
#include "pwm.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "24cxx.h"
#include "Display.h"
/*
//��һ�鱣�������
int32_t  val1=2000;
int16_t  zhan1=50;
int32_t  deadtime1=0;//��ΧΪ0��6000�������������(5.95)��������
//�ڶ��鱣�������
int32_t  val2=3000;
int16_t  zhan2=40;
int32_t  deadtime2=5;//��ΧΪ0��6000�������������(5.95)��������
*/


//��һ�鱣�������
u32  val1=2000;
u16  zhan1=50;
u32  deadtime1=0;//��ΧΪ0��6000�������������(5.95)��������
//�ڶ��鱣�������
u32  val2=3000;
u16  zhan2=40;
u32  deadtime2=5;//��ΧΪ0��6000�������������(5.95)��������


//����ѡ���־
u8 date_choose=1;
	
//���ݸı��־����ʾ���ݷ����ı䣬��Ҫ���³�ʼ��
u8 date_change=0;

int main(void)
{ 
	u8 lastdate=0;//�ж�������û�б��ı�
	u8 writeflag=0;	//û�б�д��24C02����־
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	
	IO_INIT();//���뿪�����ų�ʼ��
	SetCorection();//�ж�ӳ���ϵ
	ExtiGpoup();//�ж����ȼ�����
	
	LED_Init();
	Key_Init();
	BUZZ_Init();
	delay_init(168);
	uart_init(115200);
	LCD_Init();
	AT24CXX_Init();			//IIC��ʼ�� 
	LCD_ShowString(20,40,200,16,16,lcd_id);		//��ʾLCD ID
 	while(AT24CXX_Check())//��ⲻ��24c02	//������ܲ��ܴ�IICоƬ��ȡ���ݣ������򷵻�ֵΪ1��
	{
		LCD_ShowString(20,60,200,16,16,"24C02 Check Failed!");
		delay_ms(500);
		LCD_ShowString(20,60,200,16,16,"Please Check!      ");
		delay_ms(500);
		LED0=!LED0;//DS0��˸
	}
	//StopFlag
	LCD_ShowString(20,60,200,16,16,"24C02 Ready!");  	//����ʾ������⵽IICоƬ�� 
	TIM8_PWM_Init();
	writeflag=ReadDate(12);
	/*
				WriteDate(0,val1);
				WriteDate(4,zhan1);
				WriteDate(8,deadtime1);
	
				WriteDate(16,val1);
				WriteDate(20,zhan1);
				WriteDate(24,deadtime1);
	*/
	if(writeflag==0xaa)	//��ȡ24C02�е�����
	{
		val1=ReadDate(0);
		zhan1=ReadDate(4);
		deadtime1=ReadDate(8);//��һ�����ݶ�ȡ
		
		val2=ReadDate(16);
		zhan2=ReadDate(20);
		deadtime2=ReadDate(24);//�ڶ������ݶ�ȡ
		
		//date_choose=ReadDate(24);//����ѡ���ȡ
	}
	//StopFlag
	//���ݼ�Ŀ¼��ʾ
	DateShow(val1,zhan1,deadtime1,130);
	DateShow(val2,zhan2,deadtime2,210);
	MenuShow(date_choose);
	
	
  while(1)
	{
		if(date_change != lastdate)	//���ݷ����ı䣬����ر������³�ʼ��
		{
			LED1=!LED1;
			lastdate=date_change;	//������ʷ����
			
			if(date_choose==1)										//��ʾ���µı���
				DateShow(val1,zhan1,deadtime1,130);
			else
				DateShow(val2,zhan2,deadtime2,210);
			
			MenuShow(date_choose);						//�˵�������ɫ�ı���ʾ
			
			if(date_choose == 1)									//���³�ʼ�����洢����
			{
				SetPwmVal(val1,zhan1,deadtime1);
				WriteDate(0,val1);
				WriteDate(4,zhan1);
				WriteDate(8,deadtime1);
				WriteDate(12,0xaa);	//��־�Ѿ�д�������
			}
			else if(date_choose == 2)
			{
				SetPwmVal(val2,zhan2,deadtime2);
				WriteDate(16,val2);
				WriteDate(20,zhan2);
				WriteDate(24,deadtime2);
				WriteDate(12,0xaa);	//��־�Ѿ�д�������
			}
		}
		//StopFlag
	}
}




//�жϷ������������뿪��
void EXTI9_5_IRQHandler()
{
	u16 step=0;//���θı�Ĵ�С��������
	LED0=!LED0;	//����һ�θ��жϣ�LEDȡ��һ�Σ���ʾ��������
	//Ƶ�ʵ����ж�
	if(EXTI_GetITStatus(EXTI_Line6) != RESET)	//�ж��ж����ϵ��ж��Ƿ���,�����򷵻� 1 ��
	{
		step=10;//��������10Hz
		if(PCin(6)==0)	//�����жϺ��ɵͱ��
		{
			
			if(KEY2==0)
				step=1000;//�ֵ�����500Hz
			//delay_ms(2);
			if(PGin(6)==0)	//����
			{
				if(date_choose==1)
				{
					val1+=step;
					if(val1>2500000)	//Ƶ������
						val1=2500000;
				}
				if(date_choose==2)
				{
					val2+=step;
					if(val2>2500000)	//Ƶ������
						val2=2500000;
				}
			}
			else	//����
			{
				if(date_choose==1)
				{
					val1-=step;
					if(val1<10)	//Ƶ������
						val1=10;
				}
				if(date_choose==2)
				{
					val2-=step;
					if(val2<10)	//Ƶ������
						val2=10;
				}
			}
		}
		delay_ms(5);
		 EXTI_ClearITPendingBit(EXTI_Line6);	//������½����жϱ�־λ
	}
	
		//ռ�ձȵ����ж�
	else if(EXTI_GetITStatus(EXTI_Line9) != RESET)	//�ж��ж����ϵ��ж��Ƿ���,�����򷵻� 1 ��
	{
		step=1;//��������1%
		if(PCin(9)==0)
		{
			//LED1=!LED1;
			if(KEY1==0)
				step=5;//�ֵ�����5%
			//delay_ms(2);
			if(PGin(7)==0)	//����
			{
				if(date_choose==1)
				{
					zhan1+=step;
					if(zhan1>99)	//ռ�ձ�����
						zhan1=99;
				}
				if(date_choose==2)
				{
					zhan2+=step;
					if(zhan2>99)	//ռ�ձ�����
						zhan2=99;
				}
			}
			else	//����
			{
				if(date_choose==1)
				{
					zhan1-=step;
					if(zhan1<2)	//ռ�ձ�����
						zhan1=2;
				}
				if(date_choose==2)
				{
					zhan2-=step;
					if(zhan2<2)	//ռ�ձ�����
						zhan2=2;
				}
			}
		}
		delay_ms(25);
		 EXTI_ClearITPendingBit(EXTI_Line9);	//������½����жϱ�־λ
	}
	
		//����ʱ������ж�
	else if(EXTI_GetITStatus(EXTI_Line8) != RESET)	//�ж��ж����ϵ��ж��Ƿ���,�����򷵻� 1 ��
	{
		step=5;//��������5ns
		if(PCin(8)==0)
		{

			if(KEY2==0)
				step=50;//�ֵ�����50ns
			if(PGin(8)==0)	//����
			{
				if(date_choose==1)
				{
					deadtime1+=step;
					if(deadtime1>6000)	//����ʱ������6us=6000ns
						deadtime1=6000;
				}
				if(date_choose==2)
				{
					deadtime2+=step;
					if(deadtime2>6000)	//����ʱ������6us=6000ns
						deadtime2=6000;
				}
			}
			else	//����
			{
				if(date_choose==1)
				{
					deadtime1-=step;
					if(deadtime1<=0)		//����ʱ������
						deadtime1 =0;
				}
				if(date_choose==2)
				{
					deadtime2-=step;
					if(deadtime2<=0)		//����ʱ������
						deadtime2 =0;
				}
			}
		}
		delay_ms(25);
		 EXTI_ClearITPendingBit(EXTI_Line8);	//������½����жϱ�־λ
	}
	date_change++;
	if(date_change==100)
		date_change=1;
}
//StopFlag

//�����жϷ�����,�л�����ѡ��
void EXTI0_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)	//�ж��ж����ϵ��ж��Ƿ���,�����򷵻� 1 ��
	{
		delay_ms(5);
		if(PAin(0)==1)
		{
			if(date_choose==1)	//���������ݼ佻��
				date_choose=2;
			else
				date_choose=1;
			//���ݿ��ٸ�λ
			if(KEY2==0)
				val1=60000,zhan1=50,deadtime1=6;
			if(KEY0==0)
				val2=60000,zhan2=49,deadtime2=10;
			
			date_change++;
			if(date_change==100)
				date_change=1;
		}
	}
	delay_ms(25);
	EXTI_ClearITPendingBit(EXTI_Line0);
}



