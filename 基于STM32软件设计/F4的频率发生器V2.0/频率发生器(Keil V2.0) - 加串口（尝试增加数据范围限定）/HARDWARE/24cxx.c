#include "24cxx.h" 
#include "delay.h" 				 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//24CXX 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//初始化IIC接口
void AT24CXX_Init(void)
{
	IIC_Init();//IIC初始化
}
//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
  IIC_Start();  
	
	if(EE_TYPE>AT24C16)		//【#define EE_TYPE AT24C02，】
	{
		IIC_Send_Byte(0XA0);	   //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//发送高地址	    
	}
	else //【如果不是MIni版，执行下面】//【开发板不同用的IIC芯片不同，期间地址不同】
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	
	
	
	IIC_Wait_Ack(); 
  IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //进入接收模式	【即发送读命令】		   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		//【读取一个数据，入口参数为0,不需要应答信号】   
	IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  
	
	if(EE_TYPE>AT24C16)			//【同样先判断板子所用IIC芯片的型号】
	{
		IIC_Send_Byte(0XA0);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址	  
	}
	else 
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 

	
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
}
//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}
//检查AT24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//避免每次开机都写AT24CXX	【读取一个数据】		
	
	if(temp==0X55)
		return 0;		   
	else//排除第一次初始化的情况【即原先没有被写入任何数据的情况】
	{
		AT24CXX_WriteOneByte(255,0X55);	//【先进行一次写入操作，看能不能正常读取】
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)
			return 0;
	}
	return 1;		//【能够正常读取IIC芯片的数据】										  
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);		//【把数据逐个存储到以 pBuffer为首地址的数组里面】
		NumToRead--;
		//【百度到 a++=b 的运算顺序为：a=b;	a++】
	}
}  
//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)	//【按字节写入】
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}

//Address :要写入的地址
//Date :要写入的数据
void WriteDate(u16 Address,u32 Date)//指定位置写入一个u32类型的数据
{
	u16 add=Address;
	u32 date;
	u8 len=sizeof(Date);
	while(len--)	//4个字节一次写入，先写高位	【？？？？？？逻辑有问题，得改】
	{
		date=Date|0xf000;
		date>>=24;
		date=(u8)date;
		AT24CXX_WriteOneByte(add,date);
		Date<<=8;
		add++;
	}
}

//Address :要读取的地址
u32 ReadDate(u16 Address)//指定位置读取一个u32类型的数据
{
	u32 date=0;
	u16 add=Address;
	u8 len=4;
	while(len--)
	{
		date<<=8;
		date+=AT24CXX_ReadOneByte(add);
		add++;
	}
	return date;
}





