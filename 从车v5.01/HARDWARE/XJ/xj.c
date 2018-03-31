#include "stm32f10x.h"
#include "usart.h"	   
#include "xj.h"
#include "key.h"
#include "led.h"
#include "djqd.h"
#include "test.h"
#include "canp_hostcom.h"
u8 gd;
u8 gd_old;

/***************************************************************
** ���ܣ�     ѭ���˿ڳ�ʼ��
** ������	  �޲���
** ����ֵ��    ��
****************************************************************/
void XJ_Init()  // ѭ������ͨ��CAN����
{
// 	RCC->APB2ENR|=1<<2;    //ʹ��PORTAʱ��
//	
//	GPIOA->CRL&=0XFFFFFFF0;	  
// 	GPIOA->CRL|=0X00000003;//PA0 ���ó���� 				   
// 	GPIOA->ODR|=1<<0;	   //P���Ϊ��
//	
//  Track_Usart2_Close();
	
//	GPIOA->CRL&=0X00000000;	  
// 	GPIOA->CRL|=0X88888888;//PA0 ���ó���� 				   
// 	GPIOA->ODR|=0x00ff;	   //P���Ϊ��	

}

 void Track_Usart2_Open( void)  // ѭ�����ݴ���2 ����ʹ�� ����Ч
{
     TRACK_EN =0;
}

void Track_Usart2_Close( void)  // ѭ�����ݴ���2 �����ֹ 
{
     TRACK_EN =1;
}


/***************************************************************
** ���ܣ�     ѭ������
** ������	  �޲���
** ����ֵ��    ��
****************************************************************/
void Track(void)
{
	//��������ʱ��������������
	if( gd == 0x00)	//ѭ����ȫ�� ֹͣ
	{	
		Track_Flag=0;
		Stop_Flag=1;	
		STOP();
		Host_Close_UpTrack( );  // �ر�Ѱ�������ϴ�
	}
	else if(gd == 0xFE||gd == 0xFC||gd == 0xF8)      //1111 1110  -  1111 1100 - 1111 1000     
	{								
		Track_Flag=0;
		Stop_Flag=1;
		STOP();
		Host_Close_UpTrack();  // �ر�Ѱ�������ϴ																		 		                
	}
	else if(gd == 0x7F||gd == 0x3F||gd == 0x1F)		//0111 1111  -  0011 1111 - 0001 1111
	{
		Track_Flag=0;
		Stop_Flag=1;
		STOP();
		Host_Close_UpTrack();  // �ر�Ѱ�������ϴ
	}
	else if(gd == 0x7E||gd == 0x3C)  				//0111 1110  -  0011 1100
	{
		Track_Flag=0;
		Stop_Flag=1;
		STOP();
		Host_Close_UpTrack();  // �ر�Ѱ�������ϴ
	}
	else if(gd == 0x7C)								//0111 1100  -  0111 1000
	{
		Track_Flag=0;
		Stop_Flag=1;
		STOP();
		Host_Close_UpTrack();  // �ر�Ѱ�������ϴ
	}
	else if(gd == 0x3E||gd == 0x1E) 				//0011 1110  -  0001 1110
	{
		Track_Flag=0;
		Stop_Flag=1;
		STOP();
		Host_Close_UpTrack();  // �ر�Ѱ�������ϴ
	}
	////��֪�ķֽ���
	else
	{
	   	Stop_Flag=0;
		if(gd==0XE7 || gd==0XF7 || gd==0XEF)//1���м�3/4��������⵽���ߣ�ȫ������ 1110 0111--1111 0111--1110 1111
		{ 
			LSpeed=Car_Spend;
			RSpeed=Car_Spend;
		}
//		if(Line_Flag!=2)
//		{		
			if(gd==0XF3||gd==0XFB) //2���м�4��3��������⵽���ߣ�΢�ҹ�	1111 0011--1111 1011  
			{ 
				LSpeed=Car_Spend+40;
				RSpeed=Car_Spend-50;//
				Line_Flag=0;
			}
			else if(gd==0XF9||gd==0XFD)//3���м�3��2��������⵽���ߣ���΢�ҹ�   1111 1001--1111 1101
			{ 
				 LSpeed=Car_Spend+40;
				 RSpeed=Car_Spend-60;				 
				 Line_Flag=0;
			}
			else if(gd==0XFC)//4���м�2��1��������⵽���ߣ�ǿ�ҹ�   1111 1100
			{ 
				LSpeed=Car_Spend+50;
				RSpeed=Car_Spend-90;
				 Line_Flag=0;
			}
			else if(gd==0XFE)//5�����ұ�1��������⵽���ߣ���ǿ�ҹ�  1111 1110
			{ 
				 LSpeed=Car_Spend+60;
				 RSpeed=Car_Spend-120;			
				 Line_Flag=1;
			}
//		}
//		if(Line_Flag!=1)
//		{

			if(gd==0XCF)//6���м�6��5��������⵽���ߣ�΢���	1100 1111
      		
			{ 
				 RSpeed=Car_Spend+40;
				 LSpeed=Car_Spend-50;
				 Line_Flag=0;

			} 
			else if(gd==0X9F||gd==0XDF)//7���м�7��6��������⵽���ߣ���΢���	 1001 1111--1101 1111	 
			
			{ 
				RSpeed=Car_Spend+40;
				 LSpeed=Car_Spend-60;
				 Line_Flag=0;

			} 			  
			else if(gd==0X3F||gd==0XBF)//8���м�8��7��������⵽���ߣ�ǿ���  0011 1111--1011 1111		 
			{ 
				 RSpeed=Car_Spend+50;
				 LSpeed=Car_Spend-90;
				 Line_Flag=0;

			} 
			else if(gd==0X7F)//9������8��������⵽���ߣ���ǿ���	0111 1111	 	
			{ 
				 RSpeed=Car_Spend+60;
				 LSpeed=Car_Spend-120;
				 Line_Flag=2;
			}
//		}		
		if(gd==0xFF)   //ѭ����ȫ��
		{
				if(count>10000)
				{
					count=0;
					STOP();
					Track_Flag=0;
				    Host_Close_UpTrack( );  // �ر�Ѱ�������ϴ�	 
					if(Line_Flag ==0) Stop_Flag=4;
				}	
				else count++;				
		}
		else	count=0;

	   	if(!Track_Flag==0)
		{
			Control(LSpeed,RSpeed);
		}

	}
	if( Track_Flag==0)
	{
		Host_Close_UpTrack( );  // �ر�Ѱ�������ϴ�	
		//Send_Debug_Info("xn1\n",5);
	}
}




