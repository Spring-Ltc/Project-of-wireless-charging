//////////////////////////////////////////////////////////////////
//@4.3寸电阻屏：分辨率800*480	【即左上角点坐标为（0,0），最右下角点坐标为（480,800）】
//@波形输出通道：
//@频率调节—旋转编码开关：中断PC6,			查询PG6,		按键暂无			KEY2加大步进值
//@占空比调节—旋转编码开关：中断PC9,		查询PG7,		按键暂无			KEY1加大步进值
//@死区时间调节—旋转编码开关：中断PC8,		查询PG8,		按键暂无			KEY0加大步进值
//@数据选择输出：板载按键KEY_UP进行切换
//////////////////////////////////////////////////////////////////
//@中断部分OK，可以适当增加消抖
//还在修改调试中..................................
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
//第一组保存的数据
int32_t  val1=2000;
int16_t  zhan1=50;
int32_t  deadtime1=0;//范围为0到6000，且最好是周期(5.95)的整数倍
//第二组保存的数据
int32_t  val2=3000;
int16_t  zhan2=40;
int32_t  deadtime2=5;//范围为0到6000，且最好是周期(5.95)的整数倍
*/


//第一组保存的数据
u32  val1=2000;
u16  zhan1=50;
u32  deadtime1=0;//范围为0到6000，且最好是周期(5.95)的整数倍
//第二组保存的数据
u32  val2=3000;
u16  zhan2=40;
u32  deadtime2=5;//范围为0到6000，且最好是周期(5.95)的整数倍


//数据选择标志
u8 date_choose=1;
	
//数据改变标志，提示数据发生改变，需要重新初始化
u8 date_change=0;

int main(void)
{ 
	u8 lastdate=0;//判断数据有没有被改变
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
	LCD_ShowString(20,40,200,16,16,lcd_id);		//显示LCD ID
 	while(AT24CXX_Check())//检测不到24c02	//【检测能不能从IIC芯片读取数据，不能则返回值为1】
	{
		LCD_ShowString(20,60,200,16,16,"24C02 Check Failed!");
		delay_ms(500);
		LCD_ShowString(20,60,200,16,16,"Please Check!      ");
		delay_ms(500);
		LED0=!LED0;//DS0闪烁
	}
	//StopFlag
	LCD_ShowString(20,60,200,16,16,"24C02 Ready!");  	//【提示正常检测到IIC芯片】 
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
	if(writeflag==0xaa)	//读取24C02中的数据
	{
		val1=ReadDate(0);
		zhan1=ReadDate(4);
		deadtime1=ReadDate(8);//第一组数据读取
		
		val2=ReadDate(16);
		zhan2=ReadDate(20);
		deadtime2=ReadDate(24);//第二组数据读取
		
		//date_choose=ReadDate(24);//数据选择读取
	}
	//StopFlag
	//数据即目录显示
	DateShow(val1,zhan1,deadtime1,130);
	DateShow(val2,zhan2,deadtime2,210);
	MenuShow(date_choose);
	
	
  while(1)
	{
		if(date_change != lastdate)	//数据发生改变，对相关变量重新初始化
		{
			LED1=!LED1;
			lastdate=date_change;	//更新历史变量
			
			if(date_choose==1)										//显示更新的变量
				DateShow(val1,zhan1,deadtime1,130);
			else
				DateShow(val2,zhan2,deadtime2,210);
			
			MenuShow(date_choose);						//菜单字体颜色改变提示
			
			if(date_choose == 1)									//重新初始化并存储数据
			{
				SetPwmVal(val1,zhan1,deadtime1);
				WriteDate(0,val1);
				WriteDate(4,zhan1);
				WriteDate(8,deadtime1);
				WriteDate(12,0xaa);	//标志已经写入过数据
			}
			else if(date_choose == 2)
			{
				SetPwmVal(val2,zhan2,deadtime2);
				WriteDate(16,val2);
				WriteDate(20,zhan2);
				WriteDate(24,deadtime2);
				WriteDate(12,0xaa);	//标志已经写入过数据
			}
		}
		//StopFlag
	}
}




//中断服务函数，检测编码开关
void EXTI9_5_IRQHandler()
{
	u16 step=0;//单次改变的大小，即步长
	LED0=!LED0;	//进入一次该中断，LED取反一次，提示正常运行
	//频率调节中断
	if(EXTI_GetITStatus(EXTI_Line6) != RESET)	//判断中断线上的中断是否发生,发生则返回 1 。
	{
		step=10;//基础步长10Hz
		if(PCin(6)==0)	//触发中断后，由低变高
		{
			
			if(KEY2==0)
				step=1000;//粗调步长500Hz
			//delay_ms(2);
			if(PGin(6)==0)	//增加
			{
				if(date_choose==1)
				{
					val1+=step;
					if(val1>2500000)	//频率上限
						val1=2500000;
				}
				if(date_choose==2)
				{
					val2+=step;
					if(val2>2500000)	//频率上限
						val2=2500000;
				}
			}
			else	//减少
			{
				if(date_choose==1)
				{
					val1-=step;
					if(val1<10)	//频率下限
						val1=10;
				}
				if(date_choose==2)
				{
					val2-=step;
					if(val2<10)	//频率下限
						val2=10;
				}
			}
		}
		delay_ms(5);
		 EXTI_ClearITPendingBit(EXTI_Line6);	//清除该下降沿中断标志位
	}
	
		//占空比调节中断
	else if(EXTI_GetITStatus(EXTI_Line9) != RESET)	//判断中断线上的中断是否发生,发生则返回 1 。
	{
		step=1;//基础步长1%
		if(PCin(9)==0)
		{
			//LED1=!LED1;
			if(KEY1==0)
				step=5;//粗调步长5%
			//delay_ms(2);
			if(PGin(7)==0)	//增加
			{
				if(date_choose==1)
				{
					zhan1+=step;
					if(zhan1>99)	//占空比上限
						zhan1=99;
				}
				if(date_choose==2)
				{
					zhan2+=step;
					if(zhan2>99)	//占空比上限
						zhan2=99;
				}
			}
			else	//减少
			{
				if(date_choose==1)
				{
					zhan1-=step;
					if(zhan1<2)	//占空比下限
						zhan1=2;
				}
				if(date_choose==2)
				{
					zhan2-=step;
					if(zhan2<2)	//占空比下限
						zhan2=2;
				}
			}
		}
		delay_ms(25);
		 EXTI_ClearITPendingBit(EXTI_Line9);	//清除该下降沿中断标志位
	}
	
		//死区时间调节中断
	else if(EXTI_GetITStatus(EXTI_Line8) != RESET)	//判断中断线上的中断是否发生,发生则返回 1 。
	{
		step=5;//基础步长5ns
		if(PCin(8)==0)
		{

			if(KEY2==0)
				step=50;//粗调步长50ns
			if(PGin(8)==0)	//增加
			{
				if(date_choose==1)
				{
					deadtime1+=step;
					if(deadtime1>6000)	//死区时间上线6us=6000ns
						deadtime1=6000;
				}
				if(date_choose==2)
				{
					deadtime2+=step;
					if(deadtime2>6000)	//死区时间上线6us=6000ns
						deadtime2=6000;
				}
			}
			else	//减少
			{
				if(date_choose==1)
				{
					deadtime1-=step;
					if(deadtime1<=0)		//死区时间下限
						deadtime1 =0;
				}
				if(date_choose==2)
				{
					deadtime2-=step;
					if(deadtime2<=0)		//死区时间下限
						deadtime2 =0;
				}
			}
		}
		delay_ms(25);
		 EXTI_ClearITPendingBit(EXTI_Line8);	//清除该下降沿中断标志位
	}
	date_change++;
	if(date_change==100)
		date_change=1;
}
//StopFlag

//按键中断服务函数,切换数据选择
void EXTI0_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)	//判断中断线上的中断是否发生,发生则返回 1 。
	{
		delay_ms(5);
		if(PAin(0)==1)
		{
			if(date_choose==1)	//在两组数据间交换
				date_choose=2;
			else
				date_choose=1;
			//数据快速复位
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



