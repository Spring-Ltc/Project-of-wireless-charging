#include "myiic.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//IIC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//��ʼ��IIC
void IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��

	
	//��Ӳ��ƽ̨�ϣ��洢��U14��SCL����PB8,SDA����PB9��
  //GPIOB8,B9��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	
	IIC_SCL=1;
	IIC_SDA=1;	//����ʼ״̬����Ϊ1(��)���ȴ���ʼ�źš�
}



//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda����� 	����IO������Ϊ���ģʽ��
	
	IIC_SDA=1;	  	  
	IIC_SCL=1;		//�����ߵȴ�SDA���½���(��ʼ�ź�)��
	
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 	//��SDA�����½��ء�
	delay_us(4);
	//����ʼ�źŲ�����ϡ�
	
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 	//�����������ȴ���ʱ�����߶�д���ݣ�������
}	  

//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����	����IO������Ϊ���ģʽ��
	
	IIC_SCL=0;			//����������������ǲ��Ƕ���ģ�������������
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high	//���ȴ�SDA���������ء�
 	delay_us(4);
	
	IIC_SCL=1; 		//��SDA����������SCL�ĸߵ�ƽ�ڼ������Ч��ֹͣ�źš�
	IIC_SDA=1;//����I2C���߽����ź�	��SDA���������ء�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)		//��#define READ_SDA   PBin(9)  //����SDA��
	{
		//��û�н��ܵ�����������ѭ����
		ucErrTime++;
		if(ucErrTime>250)	//��250�ζ�ȡ����û�н��ܵ�������(Ӧ���ź�)���򷵻�1����ʾ����Ӧ��ʧ�ܡ�
		{
			IIC_Stop();		//������IICֹͣ�źš�
			return 1;
		}
	}
	IIC_SCL=0;//ʱ�����0 	 //�����ܵ������壬����0����ʾ����Ӧ��ɹ���  
	return 0;  
} 


//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();	//������Ϊ���ģʽ��
	
	IIC_SDA=0;	//�������͵�ƽ�����źš�
	delay_us(2);
	IIC_SCL=1;		//��ʹSDA�ĵ�������ʱ��SCL��һ�����θߵ�ƽ�ڼ䡿
	delay_us(2);
	IIC_SCL=0;
}
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��	
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;	//���������͵�ƽ���塿
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 				     
		  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
		SDA_OUT(); 		//������Ϊ���ģʽ��
	
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)	//��һ���ֽ�8λ�Ӹߵ������η��͡�
    {              
        IIC_SDA=(txd&0x80)>>7;	//���������λ���ݡ�
        txd<<=1; 	 
			
				delay_us(2);   //��TEA5767��������ʱ���Ǳ����
				IIC_SCL=1;
				delay_us(2); 
				IIC_SCL=0;		//��ʹ������SCL��һ���ߵ�ƽ�ڼ䱣���ȶ���
				delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK  
//����ڲ�����ʾ�費��ҪACKӦ���źţ�Ϊ1��ʾ��ҪӦ���źš�
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
	
  for(i=0;i<8;i++ )	//��һ���ֽڷ�8λ���ζ�ȡ��
	{
		IIC_SCL=0; 
		delay_us(2);
		IIC_SCL=1;
		receive<<=1;		//���Ӹ�λ��ʼ��ȡ��ÿ��ȡһλ��������һλ���൱�� ����2�������õ�ʵ�ʵ�8λ���ݡ�
		if(READ_SDA)	//�����ܵ�������Ϊ1����
			receive++;   
		delay_us(1); 
  }	
		//����Ч���ݽ�����ϣ���������ͷ�����Ӧ���źš�
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
		
    return receive;	//���ض�ȡ������
}



























