#ifndef __HW_H
#define __HW_H	 
#include "sys.h"

#define RI_TXD PCout(6)       //红外发送控制管脚  PC15

#define RI_TXD_APB     RCC_APB2Periph_GPIOC
#define RI_TXD_GPIO    GPIOC 
#define RI_TXD_PIN     GPIO_Pin_6

void HW_Init(void);
void Transmition(u8 *s,int n);
	 				    
#endif
