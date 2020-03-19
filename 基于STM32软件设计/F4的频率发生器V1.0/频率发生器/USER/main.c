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
	u32 deadtime=0;//范围为0到6000，且最好是周期(5.95)的整数倍

int main(void)
{ 
	u8 key=0;
	u8 lcd_id[12];	//存放LCD ID字符串

	u8 writeflag=0;	//没有被写入24C02过标志
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	
	IO_INIT();//编码开关引脚初始化
	SetCorection();//中断映射关系
	ExtiGpoup();//中断优先级分组
	
	LED_Init();
	Key_Init();
	BUZZ_Init();
	delay_init(168);
	uart_init(115200);
	LCD_Init();
	AT24CXX_Init();			//IIC初始化 
	
 	while(AT24CXX_Check())//检测不到24c02	//【检测能不能从IIC芯片读取数据，不能则返回值为1】
	{
		LCD_ShowString(30,150,200,16,16,"24C02 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,150,200,16,16,"Please Check!      ");
		delay_ms(500);
		LED0=!LED0;//DS0闪烁
	}
	LCD_ShowString(30,150,200,24,24,"24C02 Ready!");  	//【提示正常检测到IIC芯片】 
	
	
	TIM8_PWM_Init();
	writeflag=ReadDate(12);
	
	if(writeflag==0xaa)
	{
		val=ReadDate(0);
		zhan=ReadDate(4);
		deadtime=ReadDate(8);
	}
	SetPwmVal(val,zhan,deadtime);	//读取上次的数据并进行初始化
	
	POINT_COLOR=RED;	//字符颜色
	
	sprintf((char*)lcd_id,"LCD  ID : %04X",lcddev.id);//打印LCD ID

	
	POINT_COLOR=RED;	//字符颜色
	LCD_ShowString(20,0,600,32,32,"   Frequency generator");	//居中大号显示
	
	//LCD_ShowString(100,500,200,24,24,lcd_id);		//显示LCD ID		
	
	POINT_COLOR=BLACK;//数据分栏显示
	//LCD_ShowString(0,32,600,24,24,"频率（Hz）   占空比（%）   死区宽度（us）");	//目录显示
	LCD_ShowString(0,270,600,24,24,"Frequency(Hz)    Duty(%)    Deadtime(ns)");	//目录显示
	LCD_ShowNum(0,300,val,8,24);
	LCD_ShowNum(120,300,zhan,10,24);
	LCD_ShowNum(270,300,deadtime,10,24);
	
	LCD_ShowString(0,350,600,24,24,"persiod        prescaler        pulse");	//目录显示

	
	
	POINT_COLOR=BLACK;	//按键操作提示
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
			//LCD_ShowString(0,300,480,24,24,"                                                        ");//相当于局部清屏
			LCD_ShowNum(0,300,val,8,24);
			LCD_ShowNum(120,300,zhan,10,24);
			LCD_ShowNum(270,300,deadtime,10,24);
			WriteDate(0,val);
			WriteDate(4,zhan);
			WriteDate(8,deadtime);
			WriteDate(12,0xaa);	//标志已经写入过数据
		}
		
	}
}




//中断服务函数3个
//频率调节，步进10Hz
void EXTI9_5_IRQHandler()
{
	LED0=!LED0;
	if(EXTI_GetITStatus(EXTI_Line6) != RESET)	//判断中断线上的中断是否发生,发生则返回 1 。
	{
		//delay_us(10);	//按键消抖
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
			WriteDate(12,0xaa);	//标志已经写入过数据
			LED1=!LED1;
		}
		 EXTI_ClearITPendingBit(EXTI_Line2);	//清除中断标志位
	}
	EXTI_ClearFlag(EXTI_Line6);
}
/*
//频率调节，步进100Hz
void EXTI3_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line3) != RESET)	//判断中断线上的中断是否发生,发生则返回 1 。
	{
		delay_ms(5);	//按键消抖
		if(PEin(3)==0)
		{
			LED0_OFF ;
			LED1_OFF ;
			style=1;
		}
		 EXTI_ClearITPendingBit(EXTI_Line3);	//清除中断标志位
	}
}
//死区时间调节，步进5ns
void EXTI4_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line4) != RESET)	//判断中断线上的中断是否发生,发生则返回 1 。
	{
		delay_ms(5);	//按键消抖
		if(PEin(4)==0)
		{
			style=0;
			LED1 =!LED1 ;
		}
		 EXTI_ClearITPendingBit(EXTI_Line4);	//清除中断标志位
	}
}


*/
