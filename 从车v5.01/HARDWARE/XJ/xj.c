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
** 功能：     循迹端口初始化
** 参数：	  无参数
** 返回值：    无
****************************************************************/
void XJ_Init()  // 循迹数据通过CAN传输
{
// 	RCC->APB2ENR|=1<<2;    //使能PORTA时钟
//	
//	GPIOA->CRL&=0XFFFFFFF0;	  
// 	GPIOA->CRL|=0X00000003;//PA0 设置成输出 				   
// 	GPIOA->ODR|=1<<0;	   //P输出为高
//	
//  Track_Usart2_Close();
	
//	GPIOA->CRL&=0X00000000;	  
// 	GPIOA->CRL|=0X88888888;//PA0 设置成输出 				   
// 	GPIOA->ODR|=0x00ff;	   //P输出为高	

}

 void Track_Usart2_Open( void)  // 循迹数据串口2 传输使能 低有效
{
     TRACK_EN =0;
}

void Track_Usart2_Close( void)  // 循迹数据串口2 传输禁止 
{
     TRACK_EN =1;
}


/***************************************************************
** 功能：     循迹函数
** 参数：	  无参数
** 返回值：    无
****************************************************************/
void Track(void)
{
	//发生意外时，出现数据紊乱
	if( gd == 0x00)	//循迹灯全灭 停止
	{	
		Track_Flag=0;
		Stop_Flag=1;	
		STOP();
		Host_Close_UpTrack( );  // 关闭寻迹数据上传
	}
	else if(gd == 0xFE||gd == 0xFC||gd == 0xF8)      //1111 1110  -  1111 1100 - 1111 1000     
	{								
		Track_Flag=0;
		Stop_Flag=1;
		STOP();
		Host_Close_UpTrack();  // 关闭寻迹数据上�																		 		                
	}
	else if(gd == 0x7F||gd == 0x3F||gd == 0x1F)		//0111 1111  -  0011 1111 - 0001 1111
	{
		Track_Flag=0;
		Stop_Flag=1;
		STOP();
		Host_Close_UpTrack();  // 关闭寻迹数据上�
	}
	else if(gd == 0x7E||gd == 0x3C)  				//0111 1110  -  0011 1100
	{
		Track_Flag=0;
		Stop_Flag=1;
		STOP();
		Host_Close_UpTrack();  // 关闭寻迹数据上�
	}
	else if(gd == 0x7C)								//0111 1100  -  0111 1000
	{
		Track_Flag=0;
		Stop_Flag=1;
		STOP();
		Host_Close_UpTrack();  // 关闭寻迹数据上�
	}
	else if(gd == 0x3E||gd == 0x1E) 				//0011 1110  -  0001 1110
	{
		Track_Flag=0;
		Stop_Flag=1;
		STOP();
		Host_Close_UpTrack();  // 关闭寻迹数据上�
	}
	////无知的分界线
	else
	{
	   	Stop_Flag=0;
		if(gd==0XE7 || gd==0XF7 || gd==0XEF)//1、中间3/4传感器检测到黑线，全速运行 1110 0111--1111 0111--1110 1111
		{ 
			LSpeed=Car_Spend;
			RSpeed=Car_Spend;
		}
//		if(Line_Flag!=2)
//		{		
			if(gd==0XF3||gd==0XFB) //2、中间4、3传感器检测到黑线，微右拐	1111 0011--1111 1011  
			{ 
				LSpeed=Car_Spend+40;
				RSpeed=Car_Spend-50;//
				Line_Flag=0;
			}
			else if(gd==0XF9||gd==0XFD)//3、中间3、2传感器检测到黑线，再微右拐   1111 1001--1111 1101
			{ 
				 LSpeed=Car_Spend+40;
				 RSpeed=Car_Spend-60;				 
				 Line_Flag=0;
			}
			else if(gd==0XFC)//4、中间2、1传感器检测到黑线，强右拐   1111 1100
			{ 
				LSpeed=Car_Spend+50;
				RSpeed=Car_Spend-90;
				 Line_Flag=0;
			}
			else if(gd==0XFE)//5、最右边1传感器检测到黑线，再强右拐  1111 1110
			{ 
				 LSpeed=Car_Spend+60;
				 RSpeed=Car_Spend-120;			
				 Line_Flag=1;
			}
//		}
//		if(Line_Flag!=1)
//		{

			if(gd==0XCF)//6、中间6、5传感器检测到黑线，微左拐	1100 1111
      		
			{ 
				 RSpeed=Car_Spend+40;
				 LSpeed=Car_Spend-50;
				 Line_Flag=0;

			} 
			else if(gd==0X9F||gd==0XDF)//7、中间7、6传感器检测到黑线，再微左拐	 1001 1111--1101 1111	 
			
			{ 
				RSpeed=Car_Spend+40;
				 LSpeed=Car_Spend-60;
				 Line_Flag=0;

			} 			  
			else if(gd==0X3F||gd==0XBF)//8、中间8、7传感器检测到黑线，强左拐  0011 1111--1011 1111		 
			{ 
				 RSpeed=Car_Spend+50;
				 LSpeed=Car_Spend-90;
				 Line_Flag=0;

			} 
			else if(gd==0X7F)//9、最左8传感器检测到黑线，再强左拐	0111 1111	 	
			{ 
				 RSpeed=Car_Spend+60;
				 LSpeed=Car_Spend-120;
				 Line_Flag=2;
			}
//		}		
		if(gd==0xFF)   //循迹灯全亮
		{
				if(count>10000)
				{
					count=0;
					STOP();
					Track_Flag=0;
				    Host_Close_UpTrack( );  // 关闭寻迹数据上传	 
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
		Host_Close_UpTrack( );  // 关闭寻迹数据上传	
		//Send_Debug_Info("xn1\n",5);
	}
}




