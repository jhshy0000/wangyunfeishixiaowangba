#ifndef __CSB_H
#define __CSB_H	 
#include "sys.h"


//	#define CSB_TX PEout(3)             // PE3���ͳ���������
//	#define INT0   PEin(4)				//���峬�����ж���������


# define SPPED_INT_MODE 0   //  1 ������̼���   0�Ҳ�
#define CSB_TX PEout(6)             //���ͳ���������CSB_TX
#define INT0   PCin(7)				//���峬�����ж���������

#define INC_APB			  RCC_APB2Periph_GPIOE//INCʱ��
#define INC_GPIO 		  GPIOE                //INC GPIO��
#define INC_PIN			  GPIO_Pin_6          //INC �ܽ�

//�ǵø����жϺ�������������
//�ǵø����жϺ�������������
#define INT0_APB			  RCC_APB2Periph_GPIOC //INT0ʱ��	
#define INT0_PORTSOURCE       GPIO_PortSourceGPIOC
#define INT0_PINSOURCE    	  GPIO_PinSource7 
#define INT0_GPIO 		      GPIOC                //INT0 GPIO��
#define INT0_PIN			  GPIO_Pin_7           //INT0 �ܽ�
#define INT0_LINE             EXTI_Line7	
#define INT0_IRQ			  EXTI9_5_IRQn		   //INTO�ж�
//�ǵø����жϺ�������������
//�ǵø����жϺ�������������




#define SPEED     PBin(10)			    //���������ź���������
#define SPEED_R   PBin(0)			    //���������ź���������

extern u16 dis;

void EXTI9_5_IRQHandler(void);    // �ⲿ�ж�13�������
void EXTIX_Init(void);		        // �ⲿ�жϳ�ʼ������
void Timer3_Init(u16 arr,u16 psc);	// ͨ�ö�ʱ���жϳ�ʼ��
void TIM3_IRQHandler(void);         // ��ʱ�ж�3�������     
void tran(void); 					// ��������������
	 				    
#endif


