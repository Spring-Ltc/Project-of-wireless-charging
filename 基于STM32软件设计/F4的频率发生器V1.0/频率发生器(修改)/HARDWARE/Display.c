#include "Display.h"
#include "lcd.h"


u8 lcd_id[12];

//���˵���ʾ
void MenuShow(u8 colur)
{
	POINT_COLOR=RED;	//��ɫ��ʾ�豸����
	LCD_ShowString(80,0,600,32,32,"Frequency generator");	//���д����ʾ
	//LCD_ShowString(20,40,200,16,16,lcd_id);		//��ʾLCD ID
	LCD_ShowString(20,60,200,16,16,"24C02 Ready!");  	//����ʾ������⵽IICоƬ�� 
	
	
	if(colur==2)//��ɫ��ʾ��ǰ��ѡ���Ƶ��
	{
		POINT_COLOR=BLACK;	
	//	LCD_ShowString(0,100,600,24,24,"Frequency1(Hz)    Duty1(%)    Deadtime1(ns)");	//Ƶ��1��ʾ
		LCD_ShowString(0,100,600,24,24,"Frequency1(Hz)");	
		LCD_ShowString(200,100,600,24,24,"Duty1(%)");	
		LCD_ShowString(324,100,600,24,24,"Deadtime1(ns)");	
		
		POINT_COLOR=RED;
	//	LCD_ShowString(0,180,600,24,24,"Frequency2(Hz)    Duty2(%)    Deadtime2(ns)");	//Ƶ��2��ʾ
		LCD_ShowString(0,180,600,24,24,"Frequency2(Hz)");	
		LCD_ShowString(200,180,600,24,24,"Duty2(%)");	
		LCD_ShowString(324,180,600,24,24,"Deadtime2(ns)");
	}
	if(colur==1)
	{
		POINT_COLOR=RED;	
		//LCD_ShowString(0,100,600,24,24,"Frequency1(Hz)    Duty1(%)    Deadtime1(ns)");	//Ƶ��1��ʾ
		LCD_ShowString(0,100,600,24,24,"Frequency1(Hz)");	
		LCD_ShowString(200,100,600,24,24,"Duty1(%)");	
		LCD_ShowString(324,100,600,24,24,"Deadtime1(ns)");
		POINT_COLOR=BLACK;
		//LCD_ShowString(0,180,600,24,24,"Frequency2(Hz)    Duty2(%)    Deadtime2(ns)");	//Ƶ��2��ʾ
		LCD_ShowString(0,180,600,24,24,"Frequency2(Hz)");	
		LCD_ShowString(200,180,600,24,24,"Duty2(%)");	
		LCD_ShowString(324,180,600,24,24,"Deadtime2(ns)");
	}

	LCD_ShowString(150,300,300,24,24,"Write in register");
	LCD_ShowString(0,330,480,24,24,"period        prescaler          dead");	//Ƶ��1��ʾ
	 
	
	
	
	POINT_COLOR=BLACK;	//����������ʾ
	LCD_ShowString(15,740,480,24,24,"             KEY_UP:+5%             ");
	LCD_ShowString(15,770,480,24,24,"KEY2:+5000Hz  KEY1:-5%  KEY0:-5000Hz");	
}

//����������ʾ
void DateShow(u32 freq,u32 duty,u32 dead,u16 line)
{
		LCD_ShowNum(40,line,freq,7,24);	//��ʾƵ�ʣ����xxMHz(8λ��)
		LCD_ShowNum(225,line,duty,4,24);	//��ʾռ�ձȣ�����Ϊ4λ��
		LCD_ShowNum(375,line,dead,4,24);	//��ʾ����ʱ�䣬��λns,���6us��6000ns����λ����
}
