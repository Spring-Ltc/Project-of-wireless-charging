//////////////////////////////////////////////////////////////////
//@4.3����������ֱ���800*480	�������Ͻǵ�����Ϊ��0,0���������½ǵ�����Ϊ��480,800����
//@�������ͨ�����߼���ʱ������ͨ�� TIM8_CH1��TIM8_CH1N	(PC7��PB14)
//@Ƶ�ʵ��ڡ���ת���뿪�أ���ʱ��TIM4�ı�����ģʽ,CH1��CH2��׽��������(PB6��PB7)��	�����ж�PB5ѡ��ı���
//@����ѡ����������ذ���KEY_UP�����л�

//////////////////////////////////////////////////////////////////
//@�жϲ���OK�������ʵ���������
//�����޸ĵ�����..................................
//���������Ѿ�ʵ�֣���ת���뿪��1�����ݱ仯��2����ϵ�����޸�			
//���뿪�ؼ�ⷭ���ѽ�����������������ֵģ2���������⣺24C02���籣�����BUG�����޸�		
//���Ƴ������ⲿ�ֵ������ԣ��ϴ�����ܿ����Ǹ������ݺ�δ�ɹ�д��24C02�������ܹ�������ȡ��������
//24C02��while��1���в���д�����⣬���Ե�ע�͵�Key_Init()֮�������������		
//�������⣺���뿪�ذ����ܽź�24C02Ӳ���ܽŷ�����ͻ�����޸ı���������ΪPB5
//Ӳ������ȷ�����ѻ�PCB�壺
//													�����TIM8_CH2��ӦPC7�� TIM8_CH2N��ӦPB14
//													���룺���ذ������ĸ��������뿪�ذ���PB5, ���뿪��A��B��������PB6��PB7,��TIM4������ģʽ���

/*���ܲ���˵����	��סKEY1��Ȼ�󰴱��뿪�ذ���ʵ�ֿ�����λ����
									��סKEY0��KEY2��Ȼ����ת���뿪�أ����Ըı䲽��ֵ
									
����˴��ڹ��ܣ��Ѿ��ܹ��������ͽ��գ�ȱ�ٶ��յ����ݵĴ���		2019/09/09/23��05			
�ܹ��Է��͵����ݽ���ʶ���ж���Ҫ�ı����ʲô�����Ƕ��ַ�ת��Ϊ���ֻ���������		2019/09/12/	0��15
���ڶ��ַ�ʶ��ת������ɣ������˶����ݷ�Χ���ж�����ֱ�ӷ���	v1=59000 ���ɸı�Ƶ��Ϊ59000Hz		//2019/09/12/08��37
*//////////////////////////////////////////////////////////////////
/***************************************************************/

//@�Ѵ���3��Ϊ�����ʼ��
//@�̶�Ƶ����СֵΪ3000Hz


/////////���ڲ����ʺ�Ƶ�ʱ�����ʱ(us)�Ĺ�ϵ
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

////////////////24C02��ַ�궨��///////////////////
#define Address_val1 0
#define Address_zhan1 4
#define Address_deadtime1 8
#define Address_writeflag 12
#define Address_val2 16
#define Address_zhan2 20
#define Address_deadtime2 24
#define Address_datechoose 28
//////////////////////////////////////////////////

#define DateAssert 0		//Ϊ1��ʾ����ر�������Ϊ int32_t ���ͣ����Ӷ�����������ж�
#define DateNoAssert 1		//Ϊ�㶨��Ϊ u32 ���ͣ������������������

u32 Min_Time=1500;		//ͨ��ʱ��Ƶ�ʸı��������Ҫ���ֵ�ʱ��

#if	DateNoAssert
//��һ�鱣�������
u32  val1=60100;
u16  zhan1=50;
u32  deadtime1=150;//��ΧΪ0��6000�������������(5.95)��������
//�ڶ��鱣�������
u32  val2=59100;
u16  zhan2=50;
u32  deadtime2=200;//��ΧΪ0��6000�������������(5.95)��������
#endif
 
#if	DateAssert
//��һ�鱣�������
int32_t  val1=60100;
int16_t  zhan1=50;
int32_t  deadtime1=150;//��ΧΪ0��6000�������������(5.95)��������
//�ڶ��鱣�������
int32_t  val2=59100;
int16_t  zhan2=50;
int32_t  deadtime2=200;//��ΧΪ0��6000�������������(5.95)��������
#endif


//����ѡ���־
u8 now_choose=1;//��ǰѡ������
u8 now_mode=1;//��ǰѡ��ı��뿪�ظı���
u8 now_code=1;	//���뿪�ص�ǰ��ֵ

#define USART_RX_LEN 200
u8 USART_RX_BUF[USART_RX_LEN];	//�������ݻ��棬USART_RX_LEN��ʾ����ֽ���
u16 USART_RX_STA=0;//����״̬��־λ
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ

u8 USART_Send[3]="OK";


//�Զ��庯������
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
	static u8 last_code=0;//���뿪���ϴε�ֵ
	static u8 last_choose;//��һ��ѡ������
	static u8 last_mode=1;//��һ�κ�ɫͻ����ʾ��������

	u8 writeflag=0;	//û�б�д��24C02����־
	u8 change=0;//�����жϱ�������ת����
	
	u8 len=0;//���ڽ��յ����ݵ�ʵ���ֽڳ���
	u8 i=0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2

	LED_Init();
	delay_init(168);
	
	My_UsartInit();
	My_UsartInit1();	//��ʼ����������

	LCD_Init();
	AT24CXX_Init();			//IIC��ʼ�� 
	sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id);//��LCD ID��ӡ��lcd_id���顣
	
 	while(AT24CXX_Check())//��ⲻ��24c02	//������ܲ��ܴ�IICоƬ��ȡ���ݣ������򷵻�ֵΪ1��
	{
		LCD_ShowString(20,60,200,16,16,"24C02 Check Failed!");
		delay_ms(500);
		LCD_ShowString(20,60,200,16,16,"Please Check!      ");
		delay_ms(500);
		LED0=!LED0;//DS0��˸
	}
	//StopFlag of check 24C02
	
	LCD_ShowString(20,60,200,16,16,"24C02 Ready!");  	//����ʾ������⵽IICоƬ�� 
	
	writeflag=ReadDate(Address_writeflag);//�ж��ǲ��ǵ�һ��ʹ�õ��籣��
	if(writeflag==0xaa)	//��ȡ24C02�е�����
	{
		val1=ReadDate(Address_val1);
		zhan1=ReadDate(Address_zhan1);
		deadtime1=ReadDate(Address_deadtime1);//��һ�����ݶ�ȡ
		
		val2=ReadDate(Address_val2);
		zhan2=ReadDate(Address_zhan2);
		deadtime2=ReadDate(Address_deadtime2);//�ڶ������ݶ�ȡ
		
		now_choose=ReadDate(Address_datechoose);//�������ѡ���ȡ
	}
	last_choose =now_choose;
	//StopFlag of read 24C02
	
	//���ݼ�Ŀ¼��ʾ
	DateShow((u32)val1,(u16)zhan1,(u32)deadtime1,130);
	DateShow((u32)val2,(u16)zhan2,(u32)deadtime2,210);
	MenuShow(now_choose,now_mode);
	TIM8_PWM_Init();//TIM8�������������ʼ��
	if(now_choose==1)
		SetPwmVal((u32)val1,(u16)zhan1,(u32)deadtime1);	//����Ĭ�����
	else
		SetPwmVal((u32)val2,(u16)zhan2,(u32)deadtime2);
	
	TIM4_CoderInit();	//��ʼ�����뿪��
	Key_Init();//�������жϳ�ʼ��
	

  while(1)
	{
		now_code = TIM_GetCounter(TIM4)/2;//��ȡ��ǰ���뿪�ص�ֵ
		LCD_ShowNum(40,400,now_code,4,24);		//����ʾ��ǰ�������е�ֵ��
		
		
		if(USART_RX_STA&0x8000)//�ɹ����յ�һ������
		{
			len=USART_RX_STA&0x3FFF;//�õ����ݳ���
			printf("\r\n���͵���ϢΪ��\r\n");
			for(i=0;i<len;i++)//���ֽڰѽ��յ������ݷ��ͳ�ȥ
			{
				USART_SendData(USART1,USART_RX_BUF[i]);
				while(!(USART_GetFlagStatus(USART1,USART_FLAG_TC)));//���ֽڷ�����Ϻ󣬷���ֵΪ1
			}
			printf("\r\n\r\n");
			SolveDate(USART_RX_BUF,len);//�Խ��ܵ����ݽ��д���
			USART_RX_STA=0;//���־λ��׼����һ�ν���
		}
		

		
		//ֻ��ɫͻ����ʾ��������ǰ�ı��������
		if(now_mode != last_mode)	
		{
			MenuShow(now_choose,now_mode );
			last_mode = now_mode  ;	//������ʷ����
		}
		//StopFlag of judge mode
		//���뿪�����ݷ����ı�
		if(now_code != last_code)
		{
			if(now_code > last_code)
				change=1;//��ת������
			else if(now_code < last_code)
				change=0;//��ת������
			else
				change=4;//ֻ�����ݸ��£�û��ת��
			//�ж�������ת���
			
			switch(now_mode)//�жϵ�ǰӦ�øı�ʲô
			{
				case 1:
					Freq_change(change,now_choose);break;//��ǰģʽ�ı�Ƶ��
				case 2:
					Duty_change(change,now_choose);break;//��ǰģʽ�ı�ռ�ձ�
				case 3:
					Dead_change(change,now_choose);break;//��ǰģʽ�ı��������
				case 4:		//��ǰģʽ�ı�ͨ���ӳ�ʱ��
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
			//���ݸı����
			

			
			if(now_choose==1)	
			{		
				Calculate_Regester((u32)val1,(u16)zhan1,(u32)deadtime1,1);	//����д��Ĵ�����ֵ
				DateShow((u32)val1,(u16)zhan1,(u32)deadtime1,130);
				WriteDate(Address_val1,(u32)val1);
				WriteDate(Address_zhan1,(u16)zhan1);
				WriteDate(Address_deadtime1,(u32)deadtime1);
				SetPwmVal((u32)val1,(u16)zhan1,(u32)deadtime1);
				WriteDate(Address_writeflag,0xaa);	//��־�Ѿ�д�������
			}
			else
			{
				Calculate_Regester((u32)val2,(u16)zhan2,(u32)deadtime2,2);	//����д��Ĵ�����ֵ
				DateShow((u32)val2,(u16)zhan2,(u32)deadtime2,210);
				WriteDate(Address_val2,(u32)val2);
				WriteDate(Address_zhan2,(u16)zhan2);
				WriteDate(Address_deadtime2,(u32)deadtime2);
				SetPwmVal((u32)val2,(u16)zhan2,(u32)deadtime2);
				WriteDate(Address_writeflag,0xaa);	//��־�Ѿ�д�������
			}
			//������ʾ���¼����³�ʼ�����
			
			last_code = now_code ;//������ʷ����

		}
		
		//�ж���û���л����
		if(now_choose  != last_choose)
		{
			if(now_choose==1)
				SetPwmVal((u32)val1,(u16)zhan1,(u32)deadtime1);
			else
				SetPwmVal((u32)val2,(u16)zhan2,(u32)deadtime2);
			WriteDate(Address_datechoose,now_choose);//��ס�ϴε����ͨ��
			last_choose =now_choose;//������ʷ����
			MenuShow(now_choose,now_mode );//Һ����ɫ�任��ʾ
		}
		//StopFlag of judge choose
	}
	//StopFlag of while(1)
}
//StopFlag of main


//���뿪�ذ����жϷ�������ѡ����Ҫ���ڵ�����
//@ now_mode  ��ֵ��1��2��3ѭ��
void EXTI9_5_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)	//�ж��ж����ϵ��ж��Ƿ���,�����򷵻� 1 ��
	{
		delay_ms(5);//��������
		if(PBin(5)==0)
		{
			now_mode++;
			if(now_mode==5)		//ѭ��ѡ����Ҫ�ı������
				now_mode=1;
			
			if(PEin(3)==0)	//���ݿ��ٸ�λ	����λ��ǵð�����д��24C02��
			{
				delay_ms (3);//����
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
					WriteDate(Address_deadtime1,(u32)deadtime1);	//�Ѹ�λ�������д�뵽24C02��
					
					MenuShow(now_choose,now_mode);
				}
			}
//		LED0=!LED0;//����ѡ��ı���仯����Ʒ�ת��ʾ
		}
		delay_ms(20);
		EXTI_ClearITPendingBit(EXTI_Line5);	//������½����жϱ�־λ
	}
}
//FlagStop

//���ذ����ж� KEY_UP ������,�л�����ѡ��
//@���°���������������ٵ�һ��͵ڶ������ݼ��л�
void EXTI0_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)	//�ж��ж����ϵ��ж��Ƿ���,�����򷵻� 1 ��
	{
		delay_ms(5);//��ʱ����
		if(PAin(0)==1)//���º�Ϊ�ߵ�ƽ
		{
			if(now_choose ==1)	//���������ݼ佻��
				now_choose=2;
			else
				now_choose=1;

			LED1=!LED1;	//���ݷ����л����̵Ʒ�ת��ʾ
		}
		//delay_ms(20);//����δ��ʱ�ſ��ٴν����ж�
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}
//FlagStop

/////////////Ƶ�ʸı�
//@change:���ӻ��Ǽ��٣�change=1���ӣ�change=0����
//@date_choose:��Ҫ�ı����������һ�飬date_choose=1���һ�飻date_choose=2��ڶ���
void Freq_change(u8 change,u8 date_choose)
{
	u16 step=100;//����ֵΪ100Hz

	if(PEin(2)==0)
		step-=90;//����ֵΪ10Hz
	else if(PEin(4)==0)
		step+=900;//����ֵΪ1000Hz

	if(date_choose == 1)
	{
		if(change == 1)//����
		{
			val1+=step;
			if(val1>1000000)	//Ƶ������
				val1=1000000;
		}
		else
		{
			val1-=step;
			if(val1<10)	//Ƶ������
				val1= 10;
		}
	}
	else
	{
		if(change == 1)
		{
			val2+=step;
			if(val2>1000000)	//Ƶ������
				val2=1000000;
		}
		else
		{
			val2-=step;
			if(val2<10)	//Ƶ������
				val2= 10;
		}
	}
}
//Flagstop

///////ռ�ձȸı�
//@change:���ӻ��Ǽ��٣�change=1���ӣ�change=0����
//@date_choose:��Ҫ�ı����������һ�飬date_choose=1���һ�飻date_choose=2��ڶ���
void Duty_change(u8 change,u8 date_choose)
{
	u16 step=1;//����ֵΪ1%
	
	if(PEin(2)==0)
		step+=4;//����ֵΪ5%
	else if(PEin(4)==0)
		step+=9;//����ֵΪ10%
	
	if(date_choose == 1)
	{
		if(change==1)
		{
			zhan1+=step;
			if(zhan1>99)//ռ�ձ�����
				zhan1=99;
		}
		else
		{
			zhan1-=step;
			if(zhan1<1)//ռ�ձ�����
				zhan1=1;
		}
	}
	else
	{
		if(change==1)
		{
			zhan2+=step;
			if(zhan2>99)//ռ�ձ�����
				zhan2=99;
		}
		else
		{
			zhan2-=step;
			if(zhan2<1)//ռ�ձ�����
				zhan2=1;
		}
	}
}
//StopFlag

//////////������ȸı�
//@change:���ӻ��Ǽ��٣�change=1���ӣ�change=0����
//@date_choose:��Ҫ�ı����������һ�飬date_choose=1���һ�飻date_choose=2��ڶ���
void Dead_change(u8 change,u8 date_choose)
{
	u16 step=5;//����ֵΪ5ns
	
	if(PEin(2)==0)
		step+=45;//����ֵΪ50ns
	else if(PEin(4)==0)
		step+=95;//����ֵΪ100ns
	
	if(date_choose == 1)
	{
		if(change==1)
		{
			deadtime1+=step;
			if(deadtime1 >6000)//����ʱ������
				deadtime1 =6000;
		}
		else
		{
			deadtime1-=step;
			if(deadtime1 <5)//����ʱ������
				deadtime1 =0;
		}
	}
	else
	{
		if(change==1)
		{
			deadtime2+=step;
			if(deadtime2 >6000)//����ʱ������
				deadtime2 =6000;
		}
		else
		{
			deadtime2-=step;
			if(deadtime2 <5)//����ʱ������
				deadtime2 =0;
		}
	}
}

//StopFlag

//����1�жϷ�����
void USART1_IRQHandler(void)
{
	u8 res;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)	//�ж��ǲ��ǽ����ж�
	{
//		LED0=!LED0;	//�����ж�ָʾ
		
		res=USART_ReceiveData(USART1);	//��ȡ���յ������ݡ�ֻ�յ�һ���ֽڡ�
		if(!(USART_RX_STA&0x8000))//�ж��Ƿ������ϡ����λ��1����ʾ������ϡ�
		{
			if(USART_RX_STA&0x4000)	//�ж���һ���Ƿ���յ��˱�־����0x0d
			{
				if(res==0x0a)	//�жϱ��������ǲ��ǽ�����־
					USART_RX_STA|=0x8000;//���������������
				else
					USART_RX_STA=0;//�����쳣�����������0x0d����0x0a��,���־λ������
			}
			else	//�ϴ�δ���ܵ�0x0d
			{
				if(res==0x0d)
					USART_RX_STA|=0x4000;//��bit14��Ϊ1
				else//���ǽ�����־����洢����
				{
					USART_RX_BUF[USART_RX_STA&0x3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>=USART_RX_LEN)
						USART_RX_STA=0;//�������ݳ��ȳ���������ƣ����־λ���½���
				}
			}
		}
	}	
}
//StopFlag

//�Խ��յ��Ĵ������ݽ��д���
//@��ڲ�����USART_SendΪ�洢���ܵ����ݵ���ʼ��ַ��lenΪ���ܵ����ݵĳ���
void SolveDate(u8 date[],u8 len)
{
	u8 i=0;
	
	if(date[0]=='v')	//�ı�Ƶ��
		SloveFreq(date+1,len-1);
	else if(date[0]=='z')//�ı�ռ�ձ�
		SloveDuty(date+1,len-1);
	else if(date[0]=='d')//�ı��������
		SloveDead(date+1,len-1);
	else	//�ǹؼ���Ϣ��������ͬʱ����������ʾ
	{
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		return;//�ǹؼ���Ϣ��������ͬʱ����������ʾ
	}
	//����ֻ�����ݽ����жϸı䣬δ���º��������
	
	
	DateShow((u32)val1,(u16)zhan1,(u32)deadtime1,130);
	DateShow((u32)val2,(u16)zhan2,(u32)deadtime2,210);
	MenuShow(now_choose,now_mode);
	if(now_choose==1)
		SetPwmVal((u32)val1,(u16)zhan1,(u32)deadtime1);	//����Ĭ�����
	else
		SetPwmVal((u32)val2,(u16)zhan2,(u32)deadtime2);
	
		WriteDate(Address_val2,(u32)val2);
		WriteDate(Address_zhan2,(u16)zhan2);
		WriteDate(Address_deadtime2,(u32)deadtime2);
		WriteDate(Address_val1,(u32)val1);
		WriteDate(Address_zhan1,(u16)zhan1);
		WriteDate(Address_deadtime1,(u32)deadtime1);	//�Ѵ��ڷ��͵ĵ�����д�뵽24C02��
	
	for(i=0;i<3;i++)//������ʾ��Ϣ����ʾ�Ѿ��Խ��յ������ݽ����˴���
	{
		USART_SendData(USART1,USART_Send[i]);
		while(!(USART_GetFlagStatus(USART1,USART_FLAG_TC)));//���ֽڷ�����Ϻ󣬷���ֵΪ1
		USART_SendData(USART3,USART_Send[i]);
		while(!(USART_GetFlagStatus(USART3,USART_FLAG_TC)));//���ֽڷ�����Ϻ󣬷���ֵΪ1
	}
	printf("\r\n\r\n\r\n\r\n");
}

//���ڸı�Ƶ�ʺ���
void SloveFreq(u8 date[],u8 len)
{
	int32_t now_value;
	now_value=Judgevalue(date+2,len-2);
	if(now_value<10||now_value>1000000)//�������
		return;
	if(date[1]!='=')
	{
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		return;//�ǹؼ���Ϣ��������ͬʱ����������ʾ
	}
	
	if(date[0]=='1')
			val1 =(u32)now_value;
	else if(date[0]=='2')
			val2 =(u32)now_value;
	else	//�ǹؼ���Ϣ��������ͬʱ����������ʾ
	{
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		return;//�ǹؼ���Ϣ��������ͬʱ����������ʾ
	}
}
//���ڸı�ռ�ձȺ���
void SloveDuty(u8 date[],u8 len)
{
	int32_t now_value;
	now_value=Judgevalue(date+2,len-2);
	if(now_value<1||now_value>99)//�������
		return;
	if(date[1]!='=')
	{
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		return;//�ǹؼ���Ϣ��������ͬʱ����������ʾ
	}
	
	if(date[0]=='1')
			zhan1 =(u16)now_value;
	else if(date[0]=='2')
			zhan2 =(u16)now_value;
	else	//�ǹؼ���Ϣ��������ͬʱ����������ʾ
	{
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		return;//�ǹؼ���Ϣ��������ͬʱ����������ʾ
	}
}
//���ڸı�����ʱ�亯��
void SloveDead(u8 date[],u8 len)
{
	int32_t now_value;
	now_value=Judgevalue(date+2,len-2);
	if(now_value<0||now_value>6000)//�������
		return;
	
	if(date[1]!='=')
	{
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		return;//�ǹؼ���Ϣ��������ͬʱ����������ʾ
	}
	
	if(date[0]=='1')
			deadtime1 =(u32)now_value;
	else if(date[0]=='2')
			deadtime2 =(u32)now_value;
	else	//�ǹؼ���Ϣ��������ͬʱ����������ʾ
	{
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		LED0_ON ,LED1_ON ,delay_ms(100),LED0_OFF ,LED1_OFF,delay_ms(100);
		return;//�ǹؼ���Ϣ��������ͬʱ����������ʾ
	}
}

//���ַ�ת�������ֺ���
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
	if(EXTI_GetITStatus(EXTI_Line11) != RESET)	//�ж��ж����ϵ��ж��Ƿ���,�����򷵻� 1 ��
	{
		
		if(PBin(11)==0)
		{
			SIGN=0;
			TIM_SetAutoreload(TIM8,Period1);//���������Զ���װ��ֵ
			TIM_SetCompare1(TIM8,Pulse1);
			TIM_SetCompare2(TIM8,Pulse1);
			//delay_us(Min_Time);	//�ı�Ƶ�ʺ���Ҫ�����ʱ��
		}
	 if(PBin(11)==1)
		{
			SIGN=1;
			TIM_SetAutoreload(TIM8,Period2);//���������Զ���װ��ֵ
			TIM_SetCompare1(TIM8,Pulse2);
			TIM_SetCompare2(TIM8,Pulse2);
			//delay_us(Min_Time);//�ı�Ƶ�ʺ���Ҫ�����ʱ��
		}
		EXTI_ClearITPendingBit(EXTI_Line11);	//������½����жϱ�־λ
	}
}
//FlagStop





