


#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "pwm.h"

int main(void)
{	
	u16 led_pwmval=0;
	u8 dir=1;	//PWM增减
	delay_init();
	NVIC_Configuration();//中断优先级分组
	uart_init(9600);	//串口初始化
	LED_Init();
	
	TIM1_PWM_Init();//定时器通道输出PWM初始化
	
	while(1)
	{
		delay_ms(5000);
		if(dir)
			LED=1,led_pwmval+=50;
		else
			LED=0,led_pwmval-=50;
		
		if(led_pwmval>4000)
			dir=0;
		if(led_pwmval==0)
			dir=1;
		SetPwmVal(led_pwmval,50);
		/*
		TIM_SetCompare1(TIM2,36);	
		//delay_us(led_pwmval);
		TIM_SetCompare2(TIM3,3600);	
		*/
	}
	
}










