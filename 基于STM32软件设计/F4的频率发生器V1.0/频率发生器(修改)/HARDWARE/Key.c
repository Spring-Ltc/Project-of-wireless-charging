#include "Key.h"
#include "delay.h"

void Key_Init(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOA,GPIOEʱ��
 
	//������ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4; //KEY0 KEY1 KEY2��Ӧ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//WK_UP��Ӧ����PA0
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA0
	
}


//������ֵ��ȡ����
u8 key_scanf(void)
{
	if(KEY0==0||KEY1==0||KEY2==0||WK_UP==1)
	{
		delay_ms(5);//��������
		if(KEY0==0)
		{
			while(KEY0==0);
			return 1;
		}
		else if(KEY1==0)
		{
			while(KEY1==0);
			return 2;
		}
		else if(KEY2==0)
		{
			while(KEY2==0);
			return 3;
		}
		else if(WK_UP==1)
		{
			while(WK_UP==1);
			return 4;
		}
	}
	return 0;	//û�а������·���ֵΪ0	
}








