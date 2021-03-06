#include <string.h>
//#include "stm32_lib_config.h"
#include "stm32f10x.h"
#include "sys.h"
#include "usart.h"
#include "test.h"
#include "led.h"
#include "uart_my.h"
#include "exit.h"
#include "xj.h"
#define  RXD1_MAX_NUM  200

u8 USART1_RX_BUF[RXD1_MAX_NUM];     //接收缓冲,最大200个字节.  	
u8 USART2_RX_BUF[MAX_NUM];     //接收缓冲,最大8个字节.
//接收状态
//bit7:接收是否完成标志
u8 flag1=0,flag2=0,rx2zt_flag=0;   //串口接收状态;
u8 flag3=0;		 //add by ygm



u8 USART_RX_STA=0;       // 接收状态标记	 
u8 RX_num1=0,RX_num2=0;     // 接收到的有效字节数目
u8 RX2_MAX=8;			// 定义接收的最长字节
u8 USART1_RXNUM=0;      // 定义串口1接收有效数据长度


//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 


//void USART1_IRQHandler(void)
//{			

//   	u8 res;

//	rxd1_timer4_1ms=0;
//	res=USART1->DR;

////	if(rxd1_timer4_flag==1) //  2ms时间 数据帧重新开始
////	{
////		 rxd1_timer4_flag=0;
////		 TIM4->CR1|=0x01;     //使能定时器4
////		 USART1_RXNUM=0;
////		 USART1_RX_BUF[USART1_RXNUM]=res;

////	}
////	else if(USART1_RXNUM<RXD1_MAX_NUM)	
////	{
////		  USART1_RX_BUF[++USART1_RXNUM]=res;	 // 接收数据帧
////	}

////	else   // 数据超出最大接收数据长度时，舍去不要
////	{
////			res=0;
////	}  

//	 								 
//}  
 void USART2_IRQHandler(void)
 {
   //static u8 res;
    //static u8 sum;
 	//res= USART2->DR;
//	 gd =res;
//	USART2_RX_BUF[RX_num2++] =res;
//	 if(RX_num2 >2 ) // RX_num2 =3
//	 {

//		 RX_num2 =0;
//		 if(USART2_RX_BUF[2]  == (USART2_RX_BUF[0] ^ USART2_RX_BUF[1]))
//		 {
//		    flag2 =1;
//			 rx2zt_flag =1;
////			 LED1 =!LED1;
//		 }
//		 else {
//			 
//			 flag2 =0;
//			 rx2zt_flag =0;
//		   USART2_RX_BUF[0] =USART2_RX_BUF[1];
//			 USART2_RX_BUF[1] =USART2_RX_BUF[2];
//		   RX_num2 =2;
//		 }
//	 
//	 }		 

// 	if(RX_num2>0)
// 	{
// 	   USART2_RX_BUF[RX_num2]=res;
// 	   RX_num2++;
// 	}
// 	else if (res==0x55)		// 寻找包头
// 	{
// 		
// 	   USART2_RX_BUF[0]=res;
// 	   RX_num2=1;
// 	}
// 	
//   if(RX_num2>=RX2_MAX)
// 	{
//  		RX_num2=0;	
// 		if(USART2_RX_BUF[RX2_MAX-1]==0xbb)	 // 判断包尾
// 		{									  
// 		    //主指令与三位副指令左求和校验
// 			//注意：在求和溢出时应该对和做256取余。
// 			RX_num2=0;	  // 计数清零
// 			rx2zt_flag=0;  // 接收状态清零
// 			sum=(USART2_RX_BUF[2]+USART2_RX_BUF[3]+USART2_RX_BUF[4]+USART2_RX_BUF[5])%256;
// 			if(sum==USART2_RX_BUF[6])
// 			{
// 				//	USART_RX_STA|=0x80;
// 					flag2=1;	//   指令验证正确，标志位置1
//          rx2zt_flag =1;
// 					LED2=0;		//关闭蜂鸣器
// 			}
// 			else {LED1=2; flag2=0;rx2zt_flag=0;}
// 		}		
// 		else {LED1=2; flag2=0;rx2zt_flag=0;}		 //接收错误指令，打开蜂鸣器
// 	}
 					  											 
 } 
void uart1_init(u32 bound)
{  	 
   //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	//NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
 	USART_DeInit(USART1);  //复位串口1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10


   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口

    USART_Cmd(USART1, ENABLE);                    //使能串口 

}										 
//初始化IO 串口2
//pclk2:PCLK2时钟频率(Mhz)
//bound:波特率
//CHECK OK
// mode 初始化方式 0 开中断     TXD -> PA2   RXD -> PA3
//                 1 不开中断   TXD -> PD5   RXD -> PD6
void uart2_init(u32 bound )
{  	 
//	if( mode ) //不开中断   TXD -> PD5   RXD -> PD6
//	{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART2，GPIOD时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOD, ENABLE);

  GPIO_PinRemapConfig(GPIO_Remap_USART2 , ENABLE);
	 //USART2_TX   PD.5
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //PD5
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOD, &GPIO_InitStructure); //初始化PD5
   
    //USART2_RX	  PD6
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOD, &GPIO_InitStructure);  //初始化PD6

   //USART 初始化设置
	USART_DeInit(USART2);  //复位串口2
	
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART2, &USART_InitStructure); //初始化串口
		
  USART_Cmd(USART2, ENABLE);                    //使能串口 
}
void uart3_init(u32 bound )
{  	 

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3，GPIOC时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOC, ENABLE);

  GPIO_PinRemapConfig(GPIO_PartialRemap_USART3 , ENABLE); // USART3 部分重映射
	 //USART3_TX   PC.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化//PC10
   
    //USART3_RX	  PC11
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化PC11

   //USART 初始化设置
	USART_DeInit(USART3);  //复位串口3
	
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART3, &USART_InitStructure); //初始化串口
		
  USART_Cmd(USART3, ENABLE);                    //使能串口 

}

void Delay(vu32 nCount) 
{
  for(; nCount != 0; nCount--);
}
int U1SendChar(int ch) 
{

  while (!(USART1->SR & USART_FLAG_TXE));
  USART1->DR = (ch & 0x1FF);

  return (ch);
}
//发送一个字符
int U2SendChar(int ch) 
{

  while (!(USART2->SR & USART_FLAG_TXE));
  USART2->DR = (ch & 0x1FF);

  return (ch);
}
int U3SendChar(int ch) 
{

  while (!(USART3->SR & USART_FLAG_TXE));
  USART3->DR = (ch & 0x1FF);

  return (ch);
}
int GetKey(void) 
{
  while (!(USART2->SR & USART_FLAG_RXNE));  

  return ((int)(USART2->DR & 0x1FF));
}

void U1Send_String( u8 *s)
{
     while ( *s)
		 {
         U1SendChar( *s++);

     }
 }

 
