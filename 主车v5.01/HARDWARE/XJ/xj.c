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
** ¹¦ÄÜ£º     Ñ­¼£¶Ë¿Ú³õÊ¼»¯
** ²ÎÊý£º	  ÎÞ²ÎÊý
** ·µ»ØÖµ£º    ÎÞ
****************************************************************/
void XJ_Init()  // Ñ­¼£Êý¾ÝÍ¨¹ýCAN´«Êä
{
// 	RCC->APB2ENR|=1<<2;    //Ê¹ÄÜPORTAÊ±ÖÓ
//	
//	GPIOA->CRL&=0XFFFFFFF0;	  
// 	GPIOA->CRL|=0X00000003;//PA0 ÉèÖÃ³ÉÊä³ö 				   
// 	GPIOA->ODR|=1<<0;	   //PÊä³öÎª¸ß
//	
//  Track_Usart2_Close();
	
//	GPIOA->CRL&=0X00000000;	  
// 	GPIOA->CRL|=0X88888888;//PA0 ÉèÖÃ³ÉÊä³ö 				   
// 	GPIOA->ODR|=0x00ff;	   //PÊä³öÎª¸ß	

}

 void Track_Usart2_Open( void)  // Ñ­¼£Êý¾Ý´®¿Ú2 ´«ÊäÊ¹ÄÜ µÍÓÐÐ§
{
     TRACK_EN =0;
}

void Track_Usart2_Close( void)  // Ñ­¼£Êý¾Ý´®¿Ú2 ´«Êä½ûÖ¹ 
{
     TRACK_EN =1;
}


/***************************************************************
** ¹¦ÄÜ£º     Ñ­¼£º¯Êý
** ²ÎÊý£º	  ÎÞ²ÎÊý
** ·µ»ØÖµ£º    ÎÞ
****************************************************************/
void Track(void)
{
	//·¢ÉúÒâÍâÊ±£¬³öÏÖÊý¾ÝÎÉÂÒ
	if( gd == 0x00)	//Ñ­¼£µÆÈ«Ãð Í£Ö¹
	{	
		Track_Flag=0;
		Stop_Flag=1;	
		STOP();
		Host_Close_UpTrack( );  // ¹Ø±ÕÑ°¼£Êý¾ÝÉÏ´«
	}
	else if(gd == 0xFE||gd == 0xFC||gd == 0xF8)      //1111 1110  -  1111 1100 - 1111 1000     
	{								
		Track_Flag=0;
		Stop_Flag=1;
		STOP();
		Host_Close_UpTrack();  // ¹Ø±ÕÑ°¼£Êý¾ÝÉÏ´																		 		                
	}
	else if(gd == 0x7F||gd == 0x3F||gd == 0x1F)		//0111 1111  -  0011 1111 - 0001 1111
	{
		Track_Flag=0;
		Stop_Flag=1;
		STOP();
		Host_Close_UpTrack();  // ¹Ø±ÕÑ°¼£Êý¾ÝÉÏ´
	}
	else if(gd == 0x7E||gd == 0x3C)  				//0111 1110  -  0011 1100
	{
		Track_Flag=0;
		Stop_Flag=1;
		STOP();
		Host_Close_UpTrack();  // ¹Ø±ÕÑ°¼£Êý¾ÝÉÏ´
	}
	else if(gd == 0x7C)								//0111 1100  -  0111 1000
	{
		Track_Flag=0;
		Stop_Flag=1;
		STOP();
		Host_Close_UpTrack();  // ¹Ø±ÕÑ°¼£Êý¾ÝÉÏ´
	}
	else if(gd == 0x3E||gd == 0x1E) 				//0011 1110  -  0001 1110
	{
		Track_Flag=0;
		Stop_Flag=1;
		STOP();
		Host_Close_UpTrack();  // ¹Ø±ÕÑ°¼£Êý¾ÝÉÏ´
	}
	////ÎÞÖªµÄ·Ö½çÏß
	else
	{
	   	Stop_Flag=0;
		if(gd==0XE7 || gd==0XF7 || gd==0XEF)//1¡¢ÖÐ¼ä3/4´«¸ÐÆ÷¼ì²âµ½ºÚÏß£¬È«ËÙÔËÐÐ 1110 0111--1111 0111--1110 1111
		{ 
			LSpeed=Car_Spend;
			RSpeed=Car_Spend;
		}
//		if(Line_Flag!=2)
//		{		
			if(gd==0XF3||gd==0XFB) //2¡¢ÖÐ¼ä4¡¢3´«¸ÐÆ÷¼ì²âµ½ºÚÏß£¬Î¢ÓÒ¹Õ	1111 0011--1111 1011  
			{ 
				LSpeed=Car_Spend+40;
				RSpeed=Car_Spend-50;//
				Line_Flag=0;
			}
			else if(gd==0XF9||gd==0XFD)//3¡¢ÖÐ¼ä3¡¢2´«¸ÐÆ÷¼ì²âµ½ºÚÏß£¬ÔÙÎ¢ÓÒ¹Õ   1111 1001--1111 1101
			{ 
				 LSpeed=Car_Spend+40;
				 RSpeed=Car_Spend-60;				 
				 Line_Flag=0;
			}
			else if(gd==0XFC)//4¡¢ÖÐ¼ä2¡¢1´«¸ÐÆ÷¼ì²âµ½ºÚÏß£¬Ç¿ÓÒ¹Õ   1111 1100
			{ 
				LSpeed=Car_Spend+50;
				RSpeed=Car_Spend-90;
				 Line_Flag=0;
			}
			else if(gd==0XFE)//5¡¢×îÓÒ±ß1´«¸ÐÆ÷¼ì²âµ½ºÚÏß£¬ÔÙÇ¿ÓÒ¹Õ  1111 1110
			{ 
				 LSpeed=Car_Spend+60;
				 RSpeed=Car_Spend-120;			
				 Line_Flag=1;
			}
//		}
//		if(Line_Flag!=1)
//		{

			if(gd==0XCF)//6¡¢ÖÐ¼ä6¡¢5´«¸ÐÆ÷¼ì²âµ½ºÚÏß£¬Î¢×ó¹Õ	1100 1111
      		
			{ 
				 RSpeed=Car_Spend+40;
				 LSpeed=Car_Spend-50;
				 Line_Flag=0;

			} 
			else if(gd==0X9F||gd==0XDF)//7¡¢ÖÐ¼ä7¡¢6´«¸ÐÆ÷¼ì²âµ½ºÚÏß£¬ÔÙÎ¢×ó¹Õ	 1001 1111--1101 1111	 
			
			{ 
				RSpeed=Car_Spend+40;
				 LSpeed=Car_Spend-60;
				 Line_Flag=0;

			} 			  
			else if(gd==0X3F||gd==0XBF)//8¡¢ÖÐ¼ä8¡¢7´«¸ÐÆ÷¼ì²âµ½ºÚÏß£¬Ç¿×ó¹Õ  0011 1111--1011 1111		 
			{ 
				 RSpeed=Car_Spend+50;
				 LSpeed=Car_Spend-90;
				 Line_Flag=0;

			} 
			else if(gd==0X7F)//9¡¢×î×ó8´«¸ÐÆ÷¼ì²âµ½ºÚÏß£¬ÔÙÇ¿×ó¹Õ	0111 1111	 	
			{ 
				 RSpeed=Car_Spend+60;
				 LSpeed=Car_Spend-120;
				 Line_Flag=2;
			}
//		}		
		if(gd==0xFF)   //Ñ­¼£µÆÈ«ÁÁ
		{
				if(count>10000)
				{
					count=0;
					STOP();
					Track_Flag=0;
				    Host_Close_UpTrack( );  // ¹Ø±ÕÑ°¼£Êý¾ÝÉÏ´«	 
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
		Host_Close_UpTrack( );  // ¹Ø±ÕÑ°¼£Êý¾ÝÉÏ´«	
		//Send_Debug_Info("xn1\n",5);
	}
}




