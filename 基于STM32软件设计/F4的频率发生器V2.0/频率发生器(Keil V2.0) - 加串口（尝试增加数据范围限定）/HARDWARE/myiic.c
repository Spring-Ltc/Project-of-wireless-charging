#include "myiic.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//IIC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//初始化IIC
void IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟

	
	//【硬件平台上，存储器U14的SCL连接PB8,SDA连接PB9】
  //GPIOB8,B9初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
	IIC_SCL=1;
	IIC_SDA=1;	//【初始状态都设为1(高)，等待起始信号】
}



//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出 	【该IO口配置为输出模式】
	
	IIC_SDA=1;	  	  
	IIC_SCL=1;		//【拉高等待SDA的下降沿(起始信号)】
	
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 	//【SDA产生下降沿】
	delay_us(4);
	//【起始信号产生完毕】
	
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 	//【？？？？等待把时钟拉高读写数据？？？】
}	  

//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出	【该IO口配置为输出模式】
	
	IIC_SCL=0;			//【？？？？？这句是不是多余的？？？？？？】
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high	//【等待SDA产生上升沿】
 	delay_us(4);
	
	IIC_SCL=1; 		//【SDA的上升沿在SCL的高电平期间才是有效的停止信号】
	IIC_SDA=1;//发送I2C总线结束信号	【SDA产生上升沿】
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)		//【#define READ_SDA   PBin(9)  //输入SDA】
	{
		//【没有接受到低脉冲进入该循环】
		ucErrTime++;
		if(ucErrTime>250)	//【250次读取，都没有接受到低脉冲(应答信号)，则返回1，表示接受应答失败】
		{
			IIC_Stop();		//【产生IIC停止信号】
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	 //【接受到低脉冲，返回0，表示接受应答成功】  
	return 0;  
} 


//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();	//【配置为输出模式】
	
	IIC_SDA=0;	//【产生低电平脉冲信号】
	delay_us(2);
	IIC_SCL=1;		//【使SDA的低脉冲在时钟SCL的一个矩形高电平期间】
	delay_us(2);
	IIC_SCL=0;
}
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答	
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;	//【不产生低电平脉冲】
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 				     
		  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
		SDA_OUT(); 		//【配置为输出模式】
	
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)	//【一个字节8位从高到低依次发送】
    {              
        IIC_SDA=(txd&0x80)>>7;	//【发送最高位数据】
        txd<<=1; 	 
			
				delay_us(2);   //对TEA5767这三个延时都是必须的
				IIC_SCL=1;
				delay_us(2); 
				IIC_SCL=0;		//【使数据在SCL的一个高电平期间保持稳定】
				delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK  
//【入口参数表示需不需要ACK应答信号，为1表示需要应答信号】
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
	
  for(i=0;i<8;i++ )	//【一个字节分8位依次读取】
	{
		IIC_SCL=0; 
		delay_us(2);
		IIC_SCL=1;
		receive<<=1;		//【从高位开始读取，每读取一位数据左移一位（相当于 乘以2），最后得到实际的8位数据】
		if(READ_SDA)	//【接受到的数据为1，】
			receive++;   
		delay_us(1); 
  }	
		//【有效数据接受完毕，下面给发送方发送应答信号】
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
		
    return receive;	//返回读取的数据
}



























