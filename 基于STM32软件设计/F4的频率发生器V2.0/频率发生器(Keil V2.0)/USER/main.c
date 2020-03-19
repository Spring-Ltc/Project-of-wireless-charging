//////////////////////////////////////////////////////////////////
//@4.3寸电阻屏：分辨率800*480	【即左上角点坐标为（0,0），最右下角点坐标为（480,800）】
//@波形输出通道：高级定时器互补通道 TIM8_CH1和TIM8_CH1N	(PC7和PB14)
//@频率调节—旋转编码开关：定时器TIM4的编码器模式,CH1和CH2捕捉两相脉冲(PB6和PB7)，	按键中断PB5选择改变项
//@数据选择输出：板载按键KEY_UP进行切换

//////////////////////////////////////////////////////////////////
//@中断部分OK，可以适当增加消抖
//还在修改调试中..................................
//基本功能已经实现，旋转编码开关1次数据变化成2倍关系，待修改			
//编码开关检测翻倍已解决（解决方法：计数值模2），新问题：24C02掉电保存出了BUG，待修改		
//复制程序另外部分单独测试，上次问题很可能是更新数据后未成功写入24C02，测试能够正常读取里面数据
//24C02在while（1）中不能写的问题，测试当注释掉Key_Init()之后可以正常运行		
//发现问题：编码开关按键管脚和24C02硬件管脚发生冲突，现修改编码器按键为PB5
//硬件连接确定，已画PCB板：
//													输出：TIM8_CH2对应PC7， TIM8_CH2N对应PB14
//													输入：板载按键（四个），编码开关按键PB5, 编码开关A、B两相输入PB6和PB7,用TIM4编码器模式检测

/*功能操作说明：	按住KEY1，然后按编码开关按键实现快速置位功能
									按住KEY0或KEY2，然后旋转编码开关，可以改变步进值
*//////////////////////////////////////////////////////////////////
#include "stm32f4xx.h"
#include "LED.h"
#include "Key.h"
#include "BUZZ.h"
#include "pwm.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "24cxx.h"
#include "Display.h"
#include "coder.h"

////////////////24C02地址宏定义///////////////////
#define Address_val1 0
#define Address_zhan1 4
#define Address_deadtime1 8
#define Address_writeflag 12
#define Address_val2 16
#define Address_zhan2 20
#define Address_deadtime2 24
#define Address_datechoose 28
//////////////////////////////////////////////////


//第一组保存的数据
u32  val1=60100;
u16  zhan1=50;
u32  deadtime1=150;//范围为0到6000，且最好是周期(5.95)的整数倍
//第二组保存的数据
u32  val2=59100;
u16  zhan2=50;
u32  deadtime2=200;//范围为0到6000，且最好是周期(5.95)的整数倍

//数据选择标志
u8 now_choose=1;//当前选择的输出
u8 now_mode=1;//当前选择的编码开关改变项
u8 now_code=1;	//编码开关当前的值


//自定义函数声明
void Freq_change(u8 change,u8 date_choose);
void Duty_change(u8 change,u8 date_choose);
void Dead_change(u8 change,u8 date_choose);

int main(void)
{ 
	static u8 last_code=0;//编码开关上次的值
	static u8 last_choose;//上一次选择的输出
	static u8 last_mode=1;//上一次红色突出显示的数据项

	u8 writeflag=0;	//没有被写入24C02过标志
	u8 change=0;//用于判断编码器旋转方向
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2

	LED_Init();
	delay_init(168);
	uart_init(115200);
	LCD_Init();
	AT24CXX_Init();			//IIC初始化 
	sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id);//将LCD ID打印到lcd_id数组。
	
 	while(AT24CXX_Check())//检测不到24c02	//【检测能不能从IIC芯片读取数据，不能则返回值为1】
	{
		LCD_ShowString(20,60,200,16,16,"24C02 Check Failed!");
		delay_ms(500);
		LCD_ShowString(20,60,200,16,16,"Please Check!      ");
		delay_ms(500);
		LED0=!LED0;//DS0闪烁
	}
	//StopFlag of check 24C02
	
	LCD_ShowString(20,60,200,16,16,"24C02 Ready!");  	//【提示正常检测到IIC芯片】 
	TIM8_PWM_Init();//TIM8互补输出方波初始化

	writeflag=ReadDate(Address_writeflag);//判断是不是第一次使用掉电保存
	if(writeflag==0xaa)	//读取24C02中的数据
	{
		val1=ReadDate(Address_val1);
		zhan1=ReadDate(Address_zhan1);
		deadtime1=ReadDate(Address_deadtime1);//第一组数据读取
		
		val2=ReadDate(Address_val2);
		zhan2=ReadDate(Address_zhan2);
		deadtime2=ReadDate(Address_deadtime2);//第二组数据读取
		
		now_choose=ReadDate(Address_datechoose);//数据输出选择读取
	}
	last_choose =now_choose;
	//StopFlag of read 24C02
	
	//数据即目录显示
	DateShow(val1,zhan1,deadtime1,130);
	DateShow(val2,zhan2,deadtime2,210);
	MenuShow(now_choose,now_mode);
	
	if(now_choose==1)
		SetPwmVal(val1,zhan1,deadtime1);	//启动默认输出
	else
		SetPwmVal(val2,zhan2,deadtime2);
	
	TIM4_CoderInit();	//初始化编码开关
	Key_Init();//按键及中断初始化
	

  while(1)
	{
		now_code = TIM_GetCounter(TIM4)/2;//获取当前编码开关的值
		LCD_ShowNum(40,400,now_code,4,24);		//【提示当前计数器中的值】
		
		//只红色突出显示编码器当前改变的数据项
		if(now_mode != last_mode)	
		{
			MenuShow(now_choose,now_mode );
			last_mode = now_mode  ;	//更新历史变量
		}
		//StopFlag of judge mode
		//编码开关数据发生改变
		if(now_code != last_code)
		{
			if(now_code > last_code)
				change=1;//正转，增加
			else if(now_code < last_code)
				change=0;//反转，减少
			else
				change=4;//只有数据更新，没有转动
			//判断正传反转完毕
			
			switch(now_mode)//判断当前应该改变什么
			{
				case 1:
					Freq_change(change,now_choose);break;//当前模式改变频率
				case 2:
					Duty_change(change,now_choose);break;//当前模式改变占空比
				case 3:
					Dead_change(change,now_choose);break;//当前模式改变死区宽度
			}
			//数据改变完毕
			
			if(now_choose==1)	
			{				
				DateShow(val1,zhan1,deadtime1,130);
				WriteDate(Address_val1,val1);
				WriteDate(Address_zhan1,zhan1);
				WriteDate(Address_deadtime1,deadtime1);
				SetPwmVal(val1,zhan1,deadtime1);
				WriteDate(Address_writeflag,0xaa);	//标志已经写入过数据
			}
			else
			{
				DateShow(val2,zhan2,deadtime2,210);
				WriteDate(Address_val2,val2);
				WriteDate(Address_zhan2,zhan2);
				WriteDate(Address_deadtime2,deadtime2);
				SetPwmVal(val2,zhan2,deadtime2);
				WriteDate(Address_writeflag,0xaa);	//标志已经写入过数据
			}
			//数据显示更新及重新初始化完毕
			
			last_code = now_code ;//更新历史变量

		}
		
		//判断有没有切换输出
		if(now_choose  != last_choose)
		{
			if(now_choose==1)
				SetPwmVal(val1,zhan1,deadtime1);
			else
				SetPwmVal(val2,zhan2,deadtime2);
			WriteDate(Address_datechoose,now_choose);//记住上次的输出通道
			last_choose =now_choose;//更新历史变量
			MenuShow(now_choose,now_mode );//液晶颜色变换提示
		}
		//StopFlag of judge choose
	}
	//StopFlag of while(1)
}
//StopFlag of main


//编码开关按键中断服务函数，选择需要调节的数据
//@ now_mode  的值在1、2、3循环
void EXTI9_5_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)	//判断中断线上的中断是否发生,发生则返回 1 。
	{
		delay_ms(5);//按键消抖
		if(PBin(5)==0)
		{
			now_mode++;
			if(now_mode==4)		//循环选择需要改变的数据
				now_mode=1;
			
			if(PEin(3)==0)	//数据快速复位
			{
				delay_ms (3);//消抖
				if(PEin(3)==0)
				{
					val1=60000,zhan1=50,deadtime1 =200;
					val2=59000,zhan2=50,deadtime2 =150;
					if(now_choose==1)
						SetPwmVal(val1,zhan1,deadtime1);
					else
						SetPwmVal(val2,zhan2,deadtime2);
					
					DateShow(val1,zhan1,deadtime1,130);
					DateShow(val2,zhan2,deadtime2,210);
					MenuShow(now_choose,now_mode);
				}
			}
			
		LED0=!LED0;//数据选择改变项变化，红灯翻转提示
		}
		delay_ms(20);
		EXTI_ClearITPendingBit(EXTI_Line5);	//清除该下降沿中断标志位
	}
}
//FlagStop

//板载按键中断 KEY_UP 服务函数,切换数据选择
//@按下按键后，输出的数据再第一组和第二组数据间切换
void EXTI0_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)	//判断中断线上的中断是否发生,发生则返回 1 。
	{
		delay_ms(5);//延时消抖
		if(PAin(0)==1)//按下后为高电平
		{
			if(now_choose ==1)	//在两组数据间交换
				now_choose=2;
			else
				now_choose=1;

			LED1=!LED1;	//数据发生切换，绿灯翻转提示
		}
		//delay_ms(20);//避免未及时放开再次进入中断
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}
//FlagStop

/////////////频率改变
//@change:增加还是减少，change=1增加，change=0减少
//@date_choose:需要改变的数据是哪一组，date_choose=1表第一组；date_choose=2表第二组
void Freq_change(u8 change,u8 date_choose)
{
	u16 step=100;//步进值为100Hz

	if(PEin(2)==0)
		step-=90;//步进值为10Hz
	else if(PEin(4)==0)
		step+=900;//步进值为1000Hz

	if(date_choose == 1)
	{
		if(change == 1)//增加
		{
			val1+=step;
			if(val1>1000000)	//频率上限
				val1=1000000;
		}
		else
		{
			val1-=step;
			if(val1<10)	//频率下限
				val1= 10;
		}
	}
	else
	{
		if(change == 1)
		{
			val2+=step;
			if(val2>1000000)	//频率上限
				val2=1000000;
		}
		else
		{
			val2-=step;
			if(val2<10)	//频率下限
				val2= 10;
		}
	}
}
//Flagstop

///////占空比改变
//@change:增加还是减少，change=1增加，change=0减少
//@date_choose:需要改变的数据是哪一组，date_choose=1表第一组；date_choose=2表第二组
void Duty_change(u8 change,u8 date_choose)
{
	u16 step=1;//步进值为1%
	
	if(PEin(2)==0)
		step+=4;//步进值为5%
	else if(PEin(4)==0)
		step+=9;//步进值为10%
	
	if(date_choose == 1)
	{
		if(change==1)
		{
			zhan1+=step;
			if(zhan1>99)//占空比上限
				zhan1=99;
		}
		else
		{
			zhan1-=step;
			if(zhan1<1)//占空比下限
				zhan1=1;
		}
	}
	else
	{
		if(change==1)
		{
			zhan2+=step;
			if(zhan2>99)//占空比上限
				zhan2=99;
		}
		else
		{
			zhan2-=step;
			if(zhan2<1)//占空比下限
				zhan2=1;
		}
	}
}
//StopFlag

//////////死区宽度改变
//@change:增加还是减少，change=1增加，change=0减少
//@date_choose:需要改变的数据是哪一组，date_choose=1表第一组；date_choose=2表第二组
void Dead_change(u8 change,u8 date_choose)
{
	u16 step=5;//步进值为5ns
	
	if(PEin(2)==0)
		step+=45;//步进值为50ns
	else if(PEin(4)==0)
		step+=95;//步进值为100ns
	
	if(date_choose == 1)
	{
		if(change==1)
		{
			deadtime1+=step;
			if(deadtime1 >6000)//死区时间上限
				deadtime1 =6000;
		}
		else
		{
			deadtime1-=step;
			if(deadtime1 <5)//死区时间下限
				deadtime1 =0;
		}
	}
	else
	{
		if(change==1)
		{
			deadtime2+=step;
			if(deadtime2 >6000)//死区时间上限
				deadtime2 =6000;
		}
		else
		{
			deadtime2-=step;
			if(deadtime2 <5)//死区时间下限
				deadtime2 =0;
		}
	}
}

//StopFlag




