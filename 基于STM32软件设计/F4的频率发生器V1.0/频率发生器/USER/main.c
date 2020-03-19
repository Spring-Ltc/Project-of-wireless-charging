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

	u32 val=2000;
	u16 zhan=50;
	u32 deadtime=0;//��ΧΪ0��6000�������������(5.95)��������

int main(void)
{ 
	u8 key=0;
	u8 lcd_id[12];	//���LCD ID�ַ���

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
	
 	while(AT24CXX_Check())//��ⲻ��24c02	//������ܲ��ܴ�IICоƬ��ȡ���ݣ������򷵻�ֵΪ1��
	{
		LCD_ShowString(30,150,200,16,16,"24C02 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,150,200,16,16,"Please Check!      ");
		delay_ms(500);
		LED0=!LED0;//DS0��˸
	}
	LCD_ShowString(30,150,200,24,24,"24C02 Ready!");  	//����ʾ������⵽IICоƬ�� 
	
	
	TIM8_PWM_Init();
	writeflag=ReadDate(12);
	
	if(writeflag==0xaa)
	{
		val=ReadDate(0);
		zhan=ReadDate(4);
		deadtime=ReadDate(8);
	}
	SetPwmVal(val,zhan,deadtime);	//��ȡ�ϴε����ݲ����г�ʼ��
	
	POINT_COLOR=RED;	//�ַ���ɫ
	
	sprintf((char*)lcd_id,"LCD  ID : %04X",lcddev.id);//��ӡLCD ID

	
	POINT_COLOR=RED;	//�ַ���ɫ
	LCD_ShowString(20,0,600,32,32,"   Frequency generator");	//���д����ʾ
	
	//LCD_ShowString(100,500,200,24,24,lcd_id);		//��ʾLCD ID		
	
	POINT_COLOR=BLACK;//���ݷ�����ʾ
	//LCD_ShowString(0,32,600,24,24,"Ƶ�ʣ�Hz��   ռ�ձȣ�%��   ������ȣ�us��");	//Ŀ¼��ʾ
	LCD_ShowString(0,270,600,24,24,"Frequency(Hz)    Duty(%)    Deadtime(ns)");	//Ŀ¼��ʾ
	LCD_ShowNum(0,300,val,8,24);
	LCD_ShowNum(120,300,zhan,10,24);
	LCD_ShowNum(270,300,deadtime,10,24);
	
	LCD_ShowString(0,350,600,24,24,"persiod        prescaler        pulse");	//Ŀ¼��ʾ

	
	
	POINT_COLOR=BLACK;	//����������ʾ
	LCD_ShowString(15,670,480,24,24,"             KEY_UP:+5%             ");
	LCD_ShowString(15,700,480,24,24,"KEY2:+5000Hz  KEY1:-5%  KEY0:-5000Hz");	
		
  while(1)
	{
		key=key_scanf();
		if(key!=0)
		{
			switch(key)
			{
				case 1:
					val-=10000;break;	
				case 2:
					deadtime-=50;break;	
					//zhan-=1;break;
				case 3:
					val+=10000;break;	
				case 4:
					deadtime+=50;break;
					//zhan+=1;break;
			}
			SetPwmVal(val,zhan,deadtime);
			//LCD_ShowString(0,300,480,24,24,"                                                        ");//�൱�ھֲ�����
			LCD_ShowNum(0,300,val,8,24);
			LCD_ShowNum(120,300,zhan,10,24);
			LCD_ShowNum(270,300,deadtime,10,24);
			WriteDate(0,val);
			WriteDate(4,zhan);
			WriteDate(8,deadtime);
			WriteDate(12,0xaa);	//��־�Ѿ�д�������
		}
		
	}
}




//�жϷ�����3��
//Ƶ�ʵ��ڣ�����10Hz
void EXTI9_5_IRQHandler()
{
	LED0=!LED0;
	if(EXTI_GetITStatus(EXTI_Line6) != RESET)	//�ж��ж����ϵ��ж��Ƿ���,�����򷵻� 1 ��
	{
		//delay_us(10);	//��������
		if(PCin(6)==0)
		{
			if(PGin(8)==0)
				val+=10;
			else
				val-=10;
			SetPwmVal(val,zhan,deadtime);
			
			LCD_ShowNum(0,300,val,8,24);
			LCD_ShowNum(120,300,zhan,10,24);
			LCD_ShowNum(270,300,deadtime,10,24);
			WriteDate(0,val);
			WriteDate(4,zhan);
			WriteDate(8,deadtime);
			WriteDate(12,0xaa);	//��־�Ѿ�д�������
			LED1=!LED1;
		}
		 EXTI_ClearITPendingBit(EXTI_Line2);	//����жϱ�־λ
	}
	EXTI_ClearFlag(EXTI_Line6);
}
/*
//Ƶ�ʵ��ڣ�����100Hz
void EXTI3_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line3) != RESET)	//�ж��ж����ϵ��ж��Ƿ���,�����򷵻� 1 ��
	{
		delay_ms(5);	//��������
		if(PEin(3)==0)
		{
			LED0_OFF ;
			LED1_OFF ;
			style=1;
		}
		 EXTI_ClearITPendingBit(EXTI_Line3);	//����жϱ�־λ
	}
}
//����ʱ����ڣ�����5ns
void EXTI4_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line4) != RESET)	//�ж��ж����ϵ��ж��Ƿ���,�����򷵻� 1 ��
	{
		delay_ms(5);	//��������
		if(PEin(4)==0)
		{
			style=0;
			LED1 =!LED1 ;
		}
		 EXTI_ClearITPendingBit(EXTI_Line4);	//����жϱ�־λ
	}
}


*/
