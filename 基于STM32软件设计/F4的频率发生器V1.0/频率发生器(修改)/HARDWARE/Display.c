#include "Display.h"
#include "lcd.h"


u8 lcd_id[12];

//主菜单显示
void MenuShow(u8 colur)
{
	POINT_COLOR=RED;	//红色显示设备名称
	LCD_ShowString(80,0,600,32,32,"Frequency generator");	//居中大号显示
	//LCD_ShowString(20,40,200,16,16,lcd_id);		//显示LCD ID
	LCD_ShowString(20,60,200,16,16,"24C02 Ready!");  	//【提示正常检测到IIC芯片】 
	
	
	if(colur==2)//红色显示当前所选择的频率
	{
		POINT_COLOR=BLACK;	
	//	LCD_ShowString(0,100,600,24,24,"Frequency1(Hz)    Duty1(%)    Deadtime1(ns)");	//频率1显示
		LCD_ShowString(0,100,600,24,24,"Frequency1(Hz)");	
		LCD_ShowString(200,100,600,24,24,"Duty1(%)");	
		LCD_ShowString(324,100,600,24,24,"Deadtime1(ns)");	
		
		POINT_COLOR=RED;
	//	LCD_ShowString(0,180,600,24,24,"Frequency2(Hz)    Duty2(%)    Deadtime2(ns)");	//频率2显示
		LCD_ShowString(0,180,600,24,24,"Frequency2(Hz)");	
		LCD_ShowString(200,180,600,24,24,"Duty2(%)");	
		LCD_ShowString(324,180,600,24,24,"Deadtime2(ns)");
	}
	if(colur==1)
	{
		POINT_COLOR=RED;	
		//LCD_ShowString(0,100,600,24,24,"Frequency1(Hz)    Duty1(%)    Deadtime1(ns)");	//频率1显示
		LCD_ShowString(0,100,600,24,24,"Frequency1(Hz)");	
		LCD_ShowString(200,100,600,24,24,"Duty1(%)");	
		LCD_ShowString(324,100,600,24,24,"Deadtime1(ns)");
		POINT_COLOR=BLACK;
		//LCD_ShowString(0,180,600,24,24,"Frequency2(Hz)    Duty2(%)    Deadtime2(ns)");	//频率2显示
		LCD_ShowString(0,180,600,24,24,"Frequency2(Hz)");	
		LCD_ShowString(200,180,600,24,24,"Duty2(%)");	
		LCD_ShowString(324,180,600,24,24,"Deadtime2(ns)");
	}

	LCD_ShowString(150,300,300,24,24,"Write in register");
	LCD_ShowString(0,330,480,24,24,"period        prescaler          dead");	//频率1显示
	 
	
	
	
	POINT_COLOR=BLACK;	//按键操作提示
	LCD_ShowString(15,740,480,24,24,"             KEY_UP:+5%             ");
	LCD_ShowString(15,770,480,24,24,"KEY2:+5000Hz  KEY1:-5%  KEY0:-5000Hz");	
}

//参数数据显示
void DateShow(u32 freq,u32 duty,u32 dead,u16 line)
{
		LCD_ShowNum(40,line,freq,7,24);	//显示频率，最大xxMHz(8位数)
		LCD_ShowNum(225,line,duty,4,24);	//显示占空比，暂设为4位数
		LCD_ShowNum(375,line,dead,4,24);	//显示死区时间，单位ns,最大6us（6000ns，四位数）
}
