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
									
添加了串口功能，已经能够正常发送接收，缺少对收到数据的处理		2019/09/09/23：05			
能够对发送的数据进行识别，判断需要改变的是什么，但是对字符转换为数字还存在问题		2019/09/12/	0：15
串口对字符识别转化已完成，增加了对数据范围的判定，可直接发送	v1=59000 即可改变频率为59000Hz		//2019/09/12/08：37
*//////////////////////////////////////////////////////////////////
/***************************************************************/

//@把串口3改为波特率检测
//@固定频率最小值为3000Hz


/////////关于波特率和频率保持延时(us)的关系
/*     BOOD			Max_Time
			38400-------->24
			19200-------->50
			14400-------->68
			9600-------->102
			4800-------->206
			2400-------->414
			1200-------->830
			 600-------->1660	
*******************************************************************/


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
#include "My_usart.h"

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

#define DateAssert 0		//为1表示对相关变量定义为 int32_t 类型，增加对数据溢出的判断
#define DateNoAssert 1		//为零定义为 u32 类型，对数据溢出不做处理

u32 Min_Time=1500;		//通信时，频率改变后至少需要保持得时间

#if	DateNoAssert
//第一组保存的数据
u32  val1=60100;
u16  zhan1=50;
u32  deadtime1=150;//范围为0到6000，且最好是周期(5.95)的整数倍
//第二组保存的数据
u32  val2=59100;
u16  zhan2=50;
u32  deadtime2=200;//范围为0到6000，且最好是周期(5.95)的整数倍
#endif
 
#if	DateAssert
//第一组保存的数据
int32_t  val1=60100;
int16_t  zhan1=50;
int32_t  deadtime1=150;//范围为0到6000，且最好是周期(5.95)的整数倍
//第二组保存的数据
int32_t  val2=59100;
int16_t  zhan2=50;
int32_t  deadtime2=200;//范围为0到6000，且最好是周期(5.95)的整数倍
#endif


//数据选择标志
u8 now_choose=1;//当前选择的输出
u8 now_mode=1;//当前选择的编码开关改变项
u8 now_code=1;	//编码开关当前的值

#define USART_RX_LEN 200
u8 USART_RX_BUF[USART_RX_LEN];	//接收数据缓存，USART_RX_LEN表示最大字节数
u16 USART_RX_STA=0;//接收状态标志位
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目

u8 USART_Send[3]="OK";


//自定义函数声明
void Freq_change(u8 change,u8 date_choose);
void Duty_change(u8 change,u8 date_choose);
void Dead_change(u8 change,u8 date_choose);
void SolveDate(u8 date[],u8 len);
void SloveFreq(u8 date[],u8 len);
void SloveDuty(u8 date[],u8 len);
void SloveDead(u8 date[],u8 len);
int32_t Judgevalue(u8 date[],u8 len);



int main(void)
{ 
	static u8 last_code=0;//编码开关上次的值
	static u8 last_choose;//上一次选择的输出
	static u8 last_mode=1;//上一次红色突出显示的数据项

	u8 writeflag=0;	//没有被写入24C02过标志
	u8 change=0;//用于判断编码器旋转方向
	
	u8 len=0;//串口接收到数据的实际字节长度
	u8 i=0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2

	LED_Init();
	delay_init(168);
	
	My_UsartInit();
	My_UsartInit1();	//初始化两个串口

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
	DateShow((u32)val1,(u16)zhan1,(u32)deadtime1,130);
	DateShow((u32)val2,(u16)zhan2,(u32)deadtime2,210);
	MenuShow(now_choose,now_mode);
	TIM8_PWM_Init();//TIM8互补输出方波初始化
	if(now_choose==1)
		SetPwmVal((u32)val1,(u16)zhan1,(u32)deadtime1);	//启动默认输出
	else
		SetPwmVal((u32)val2,(u16)zhan2,(u32)deadtime2);
	
	TIM4_CoderInit();	//初始化编码开关
	Key_Init();//按键及中断初始化
	

  while(1)
	{
		now_code = TIM_GetCounter(TIM4)/2;//获取当前编码开关的值
		LCD_ShowNum(40,400,now_code,4,24);		//【提示当前计数器中的值】
		
		
		if(USART_RX_STA&0x8000)//成功接收到一次数据
		{
			len=USART_RX_STA&0x3FFF;//得到数据长度
			printf("\r\n发送的信息为：\r\n");
			for(i=0;i<len;i++)//逐字节把接收到的数据发送出去
			{
				USART_SendData(USART1,USART_RX_BUF[i]);
				while(!(USART_GetFlagStatus(USART1,USART_FLAG_TC)));//该字节发送完毕后，返回值为1
			}
			printf("\r\n\r\n");
			SolveDate(USART_RX_BUF,len);//对接受的数据进行处理
			USART_RX_STA=0;//清标志位，准备下一次接收
		}
		

		
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
				case 4:		//当前模式改变通信延迟时间
				{
					if(change==1)
						Min_Time+=5;
					else if(change==0)
						Min_Time-=5;
					LCD_ShowString(235,250,600,24,24,"     ");
					LCD_ShowNum(235,250,Min_Time,4,24);
					break;
				}
			}
			//数据改变完毕
			

			
			if(now_choose==1)	
			{		
				Calculate_Regester((u32)val1,(u16)zhan1,(u32)deadtime1,1);	//计算写入寄存器的值
				DateShow((u32)val1,(u16)zhan1,(u32)deadtime1,130);
				WriteDate(Address_val1,(u32)val1);
				WriteDate(Address_zhan1,(u16)zhan1);
				WriteDate(Address_deadtime1,(u32)deadtime1);
				SetPwmVal((u32)val1,(u16)zhan1,(u32)deadtime1);
				WriteDate(Address_writeflag,0xaa);	//标志已经写入过数据
			}
			else
			{
				Calculate_Regester((u32)val2,(u16)zhan2,(u32)deadtime2,2);	//计算写入寄存器的值
				DateShow((u32)val2,(u16)zhan2,(u32)deadtime2,210);
				WriteDate(Address_val2,(u32)val2);
				WriteDate(Address_zhan2,(u16)zhan2);
				WriteDate(Address_deadtime2,(u32)deadtime2);
				SetPwmVal((u32)val2,(u16)zhan2,(u32)deadtime2);
				WriteDate(Address_writeflag,0xaa);	//标志已经写入过数据
			}
			//数据显示更新及重新初始化完毕
			
			last_code = now_code ;//更新历史变量

		}
		
		//判断有没有切换输出
		if(now_choose  != last_choose)
		{
			if(now_choose==1)
				SetPwmVal((u32)val1,(u16)zhan1,(u32)deadtime1);
			else
				SetPwmVal((u32)val2,(u16)zhan2,(u32)deadtime2);
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
			if(now_mode==5)		//循环选择需要改变的数据
				now_mode=1;
			
			if(PEin(3)==0)	//数据快速复位	【复位后记得把数据写进24C02】
			{
				delay_ms (3);//消抖
				if(PEin(3)==0)
				{
					val1=60000,zhan1=50,deadtime1 =200;
					val2=59000,zhan2=50,deadtime2 =150;
					if(now_choose==1)
						SetPwmVal((u32)val1,(u16)zhan1,(u32)deadtime1);
					else
						SetPwmVal((u32)val2,(u16)zhan2,(u32)deadtime2);
					
					DateShow((u32)val1,(u16)zhan1,(u32)deadtime1,130);
					DateShow((u32)val2,(u16)zhan2,(u32)deadtime2,210);
					
					WriteDate(Address_val2,(u32)val2);
					WriteDate(Address_zhan2,(u16)zhan2);
					WriteDate(Address_deadtime2,(u32)deadtime2);
					WriteDate(Address_val1,(u32)val1);
					WriteDate(Address_zhan1,(u16)zhan1);
					WriteDate(Address_deadtime1,(u32)deadtime1);	//把复位后的数据写入到24C02中
					
					MenuShow(now_choose,now_mode);
				}
			}
//		LED0=!LED0;//数据选择改变项变化，红灯翻转提示
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

//串口1中断服务函数
void USART1_IRQHandler(void)
{
	u8 res;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)	//判断是不是接受中断
	{
//		LED0=!LED0;	//进入中断指示
		
		res=USART_ReceiveData(USART1);	//读取接收到的数据【只收到一个字节】
		if(!(USART_RX_STA&0x8000))//判断是否接收完毕【最高位置1，表示接收完毕】
		{
			if(USART_RX_STA&0x4000)	//判断上一次是否接收到了标志数据0x0d
			{
				if(res==0x0a)	//判断本次数据是不是结束标志
					USART_RX_STA|=0x8000;//正常接收数据完毕
				else
					USART_RX_STA=0;//接收异常（最后数据有0x0d而无0x0a）,清标志位，重来
			}
			else	//上次未接受到0x0d
			{
				if(res==0x0d)
					USART_RX_STA|=0x4000;//把bit14置为1
				else//不是结束标志，则存储数据
				{
					USART_RX_BUF[USART_RX_STA&0x3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>=USART_RX_LEN)
						USART_RX_STA=0;//接收数据长度超出最大限制，清标志位重新接收
				}
			}
		}
	}	
}
//StopFlag

//对接收到的串口数据进行处理
//@入口参数：USART_Send为存储接受到数据的起始地址，len为接受的数据的长度
void SolveDate(u8 date[],u8 len)
{
	u8 i=0;
	
	if(date[0]=='v')	//改变频率
		SloveFreq(date+1,len-1);
	else if(date[0]=='z')//改变占空比
		SloveDuty(date+1,len-1);
	else if(date[0]=='d')//改变死区宽度
		SloveDead(date+1,len-1);
	else	//非关键信息，两个灯同时闪灭两次提示
	{
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		return;//非关键信息，两个灯同时闪灭两次提示
	}
	//上面只对数据进行判断改变，未更新和设置输出
	
	
	DateShow((u32)val1,(u16)zhan1,(u32)deadtime1,130);
	DateShow((u32)val2,(u16)zhan2,(u32)deadtime2,210);
	MenuShow(now_choose,now_mode);
	if(now_choose==1)
		SetPwmVal((u32)val1,(u16)zhan1,(u32)deadtime1);	//启动默认输出
	else
		SetPwmVal((u32)val2,(u16)zhan2,(u32)deadtime2);
	
		WriteDate(Address_val2,(u32)val2);
		WriteDate(Address_zhan2,(u16)zhan2);
		WriteDate(Address_deadtime2,(u32)deadtime2);
		WriteDate(Address_val1,(u32)val1);
		WriteDate(Address_zhan1,(u16)zhan1);
		WriteDate(Address_deadtime1,(u32)deadtime1);	//把串口发送的的数据写入到24C02中
	
	for(i=0;i<3;i++)//发送提示信息，表示已经对接收到的数据进行了处理
	{
		USART_SendData(USART1,USART_Send[i]);
		while(!(USART_GetFlagStatus(USART1,USART_FLAG_TC)));//该字节发送完毕后，返回值为1
		USART_SendData(USART3,USART_Send[i]);
		while(!(USART_GetFlagStatus(USART3,USART_FLAG_TC)));//该字节发送完毕后，返回值为1
	}
	printf("\r\n\r\n\r\n\r\n");
}

//串口改变频率函数
void SloveFreq(u8 date[],u8 len)
{
	int32_t now_value;
	now_value=Judgevalue(date+2,len-2);
	if(now_value<10||now_value>1000000)//数据溢出
		return;
	if(date[1]!='=')
	{
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		return;//非关键信息，两个灯同时闪灭两次提示
	}
	
	if(date[0]=='1')
			val1 =(u32)now_value;
	else if(date[0]=='2')
			val2 =(u32)now_value;
	else	//非关键信息，两个灯同时闪灭两次提示
	{
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		return;//非关键信息，两个灯同时闪灭两次提示
	}
}
//串口改变占空比函数
void SloveDuty(u8 date[],u8 len)
{
	int32_t now_value;
	now_value=Judgevalue(date+2,len-2);
	if(now_value<1||now_value>99)//数据溢出
		return;
	if(date[1]!='=')
	{
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		return;//非关键信息，两个灯同时闪灭两次提示
	}
	
	if(date[0]=='1')
			zhan1 =(u16)now_value;
	else if(date[0]=='2')
			zhan2 =(u16)now_value;
	else	//非关键信息，两个灯同时闪灭两次提示
	{
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		return;//非关键信息，两个灯同时闪灭两次提示
	}
}
//串口改变死区时间函数
void SloveDead(u8 date[],u8 len)
{
	int32_t now_value;
	now_value=Judgevalue(date+2,len-2);
	if(now_value<0||now_value>6000)//数据溢出
		return;
	
	if(date[1]!='=')
	{
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		return;//非关键信息，两个灯同时闪灭两次提示
	}
	
	if(date[0]=='1')
			deadtime1 =(u32)now_value;
	else if(date[0]=='2')
			deadtime2 =(u32)now_value;
	else	//非关键信息，两个灯同时闪灭两次提示
	{
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		return;//非关键信息，两个灯同时闪灭两次提示
	}
}

//把字符转换成数字函数
int32_t Judgevalue(u8 date[],u8 len)
{
	int32_t number=0;
	u8 i=0,Len=len;
	for(i=0;i<Len;i++)
	{
		number*=10;
		if(date[i]>='0'&&date[i]<='9')
			number+=date[i]-'0';
		else
			return 0;
	}
	return number;
}




//StopFlag


void EXTI15_10_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line11) != RESET)	//判断中断线上的中断是否发生,发生则返回 1 。
	{
		
		if(PBin(11)==0)
		{
			SIGN=0;
			TIM_SetAutoreload(TIM8,Period1);//重新设置自动重装初值
			TIM_SetCompare1(TIM8,Pulse1);
			TIM_SetCompare2(TIM8,Pulse1);
			//delay_us(Min_Time);	//改变频率后需要保存的时间
		}
	 if(PBin(11)==1)
		{
			SIGN=1;
			TIM_SetAutoreload(TIM8,Period2);//重新设置自动重装初值
			TIM_SetCompare1(TIM8,Pulse2);
			TIM_SetCompare2(TIM8,Pulse2);
			//delay_us(Min_Time);//改变频率后需要保存的时间
		}
		EXTI_ClearITPendingBit(EXTI_Line11);	//清除该下降沿中断标志位
	}
}
//FlagStop





