#include "stm32f10x.h"   
#include "led.h"

/***************************************************************
** 功能：     核心板LED端口初始化
** 参数：	  无参数
** 返回值：    无
****************************************************************/
void LED_Init(void)
{	   	 
	RCC->APB2ENR|=1<<5;    //使能PORTD时钟	
	   	 
											  
	GPIOD->CRH&=0Xffff0000;//清掉原来的设置，同时不影响其它位设置。
	GPIOD->CRH|=0X00003333;//PD8~11推挽输出
	GPIOD->ODR &=0xf0ff;    //PD8~11输出高 
	
	
}






