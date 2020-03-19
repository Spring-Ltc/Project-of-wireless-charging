#include "Display.h"
#include "lcd.h"


u8 lcd_id[12];

/////////主菜单显示
//@colur:输出选择，colur=1输出屏幕上面一组数据；colur=2输出屏幕下面一组数据
//@mode:对当前编码开关修改的数据项标红，mode=1:改频率；mode=2:改占空比；mode=3:改死区时间
//@无返回值
void MenuShow(u8 colur,u8 mode)
{
	POINT_COLOR=RED;	//红色显示设备名称
	LCD_ShowString(80,0,600,32,32,"Frequency generator");	//居中大号显示
	LCD_ShowString(20,40,200,16,16,lcd_id);		//显示LCD ID
	LCD_ShowString(20,60,200,16,16,"24C02 Ready!");  	//【提示正常检测到IIC芯片】 
	
	
	if(colur==2)//红色显示当前所选择的频率
	{
		POINT_COLOR=BLACK;	
		LCD_ShowString(0,100,600,24,24,"Frequency1(Hz)");	
		LCD_ShowString(200,100,600,24,24,"Duty1(%)");	
		LCD_ShowString(324,100,600,24,24,"Deadtime1(ns)");	
		LCD_ShowString(0,180,600,24,24,"Frequency2(Hz)");	
		LCD_ShowString(200,180,600,24,24,"Duty2(%)");	
		LCD_ShowString(324,180,600,24,24,"Deadtime2(ns)");
		LCD_ShowString(150,250,600,24,24,"Time:");	//显示通信延时
		//先全部显示黑色，后根据模式更改为红色
		if(mode==1)
			POINT_COLOR=RED,LCD_ShowString(0,180,600,24,24,"Frequency2(Hz)");	
		else if(mode==2)
			POINT_COLOR=RED,LCD_ShowString(200,180,600,24,24,"Duty2(%)");
		else if(mode==3)
			POINT_COLOR=RED,LCD_ShowString(324,180,600,24,24,"Deadtime2(ns)");
		else if(mode==4)
			POINT_COLOR=RED,LCD_ShowString(150,250,600,24,24,"Time:");	//显示通信延时
	}
	if(colur==1)
	{
		POINT_COLOR=BLACK;	
		LCD_ShowString(0,100,600,24,24,"Frequency1(Hz)");	
		LCD_ShowString(200,100,600,24,24,"Duty1(%)");	
		LCD_ShowString(324,100,600,24,24,"Deadtime1(ns)");
		LCD_ShowString(0,180,600,24,24,"Frequency2(Hz)");	
		LCD_ShowString(200,180,600,24,24,"Duty2(%)");	
		LCD_ShowString(324,180,600,24,24,"Deadtime2(ns)");
		LCD_ShowString(150,250,600,24,24,"Time:");	//显示通信延时
		//先全部显示黑色，后根据模式更改为红色
		if(mode==1)
			POINT_COLOR=RED,LCD_ShowString(0,100,600,24,24,"Frequency1(Hz)");	
		else if(mode==2)
			POINT_COLOR=RED,LCD_ShowString(200,100,600,24,24,"Duty1(%)");
		else if(mode==3)
			POINT_COLOR=RED,LCD_ShowString(324,100,600,24,24,"Deadtime1(ns)");
		else if(mode==4)
			POINT_COLOR=RED,LCD_ShowString(150,250,600,24,24,"Time:");	//显示通信延时
	}

	LCD_ShowString(150,300,300,24,24,"Write in register");
	LCD_ShowString(0,330,480,24,24,"period        prescaler          dead");	//频率1显示
	 
	
	
	POINT_COLOR=BLACK;	//按键操作提示
	LCD_ShowString(15,740,480,24,24,"             KEY_UP:+5%             ");
	LCD_ShowString(15,770,480,24,24,"KEY2:+5000Hz  KEY1:-5%  KEY0:-5000Hz");	
}
//Flagstop

/////////参数数据显示
//@freq,duty,dead:需要显示的数据项
//@line :数据所在的行数（显示再屏幕的哪一行）
void DateShow(u32 freq,u32 duty,u32 dead,u16 line)
{
		LCD_ShowNum(40,line,freq,7,24);	//显示频率，最大xxMHz(8位数)
		LCD_ShowNum(225,line,duty,4,24);	//显示占空比，暂设为4位数
		LCD_ShowNum(375,line,dead,4,24);	//显示死区时间，单位ns,最大6us（6000ns，四位数）
}
//Flagstop


