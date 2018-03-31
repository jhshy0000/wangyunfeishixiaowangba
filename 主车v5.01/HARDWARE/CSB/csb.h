#ifndef __CSB_H
#define __CSB_H	 
#include "sys.h"


//	#define CSB_TX PEout(3)             // PE3发送超声波引脚
//	#define INT0   PEin(4)				//定义超声波中断输入引脚


# define SPPED_INT_MODE 0   //  1 左侧码盘计数   0右侧
#define CSB_TX PEout(6)             //发送超声波引脚CSB_TX
#define INT0   PCin(7)				//定义超声波中断输入引脚

#define INC_APB			  RCC_APB2Periph_GPIOE//INC时钟
#define INC_GPIO 		  GPIOE                //INC GPIO组
#define INC_PIN			  GPIO_Pin_6          //INC 管脚

//记得更改中断函数！！！！！
//记得更改中断函数！！！！！
#define INT0_APB			  RCC_APB2Periph_GPIOC //INT0时钟	
#define INT0_PORTSOURCE       GPIO_PortSourceGPIOC
#define INT0_PINSOURCE    	  GPIO_PinSource7 
#define INT0_GPIO 		      GPIOC                //INT0 GPIO组
#define INT0_PIN			  GPIO_Pin_7           //INT0 管脚
#define INT0_LINE             EXTI_Line7	
#define INT0_IRQ			  EXTI9_5_IRQn		   //INTO中断
//记得更改中断函数！！！！！
//记得更改中断函数！！！！！




#define SPEED     PBin(10)			    //定义码盘信号输入引脚
#define SPEED_R   PBin(0)			    //定义码盘信号输入引脚

extern u16 dis;

void EXTI9_5_IRQHandler(void);    // 外部中断13服务程序
void EXTIX_Init(void);		        // 外部中断初始化程序
void Timer3_Init(u16 arr,u16 psc);	// 通用定时器中断初始化
void TIM3_IRQHandler(void);         // 定时中断3服务程序     
void tran(void); 					// 左发生超声波函数
	 				    
#endif


