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

u8 USART1_RX_BUF[RXD1_MAX_NUM];     //���ջ���,���200���ֽ�.  	
u8 USART2_RX_BUF[MAX_NUM];     //���ջ���,���8���ֽ�.
//����״̬
//bit7:�����Ƿ���ɱ�־
u8 flag1=0,flag2=0,rx2zt_flag=0;   //���ڽ���״̬;
u8 flag3=0;		 //add by ygm



u8 USART_RX_STA=0;       // ����״̬���	 
u8 RX_num1=0,RX_num2=0;     // ���յ�����Ч�ֽ���Ŀ
u8 RX2_MAX=8;			// ������յ���ֽ�
u8 USART1_RXNUM=0;      // ���崮��1������Ч���ݳ���


//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 


//void USART1_IRQHandler(void)
//{			

//   	u8 res;

//	rxd1_timer4_1ms=0;
//	res=USART1->DR;

////	if(rxd1_timer4_flag==1) //  2msʱ�� ����֡���¿�ʼ
////	{
////		 rxd1_timer4_flag=0;
////		 TIM4->CR1|=0x01;     //ʹ�ܶ�ʱ��4
////		 USART1_RXNUM=0;
////		 USART1_RX_BUF[USART1_RXNUM]=res;

////	}
////	else if(USART1_RXNUM<RXD1_MAX_NUM)	
////	{
////		  USART1_RX_BUF[++USART1_RXNUM]=res;	 // ��������֡
////	}

////	else   // ���ݳ������������ݳ���ʱ����ȥ��Ҫ
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
// 	else if (res==0x55)		// Ѱ�Ұ�ͷ
// 	{
// 		
// 	   USART2_RX_BUF[0]=res;
// 	   RX_num2=1;
// 	}
// 	
//   if(RX_num2>=RX2_MAX)
// 	{
//  		RX_num2=0;	
// 		if(USART2_RX_BUF[RX2_MAX-1]==0xbb)	 // �жϰ�β
// 		{									  
// 		    //��ָ������λ��ָ�������У��
// 			//ע�⣺��������ʱӦ�öԺ���256ȡ�ࡣ
// 			RX_num2=0;	  // ��������
// 			rx2zt_flag=0;  // ����״̬����
// 			sum=(USART2_RX_BUF[2]+USART2_RX_BUF[3]+USART2_RX_BUF[4]+USART2_RX_BUF[5])%256;
// 			if(sum==USART2_RX_BUF[6])
// 			{
// 				//	USART_RX_STA|=0x80;
// 					flag2=1;	//   ָ����֤��ȷ����־λ��1
//          rx2zt_flag =1;
// 					LED2=0;		//�رշ�����
// 			}
// 			else {LED1=2; flag2=0;rx2zt_flag=0;}
// 		}		
// 		else {LED1=2; flag2=0;rx2zt_flag=0;}		 //���մ���ָ��򿪷�����
// 	}
 					  											 
 } 
void uart1_init(u32 bound)
{  	 
   //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	//NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(USART1);  //��λ����1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10


   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure); //��ʼ������

    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}										 
//��ʼ��IO ����2
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������
//CHECK OK
// mode ��ʼ����ʽ 0 ���ж�     TXD -> PA2   RXD -> PA3
//                 1 �����ж�   TXD -> PD5   RXD -> PD6
void uart2_init(u32 bound )
{  	 
//	if( mode ) //�����ж�   TXD -> PD5   RXD -> PD6
//	{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART2��GPIODʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOD, ENABLE);

  GPIO_PinRemapConfig(GPIO_Remap_USART2 , ENABLE);
	 //USART2_TX   PD.5
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //PD5
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOD, &GPIO_InitStructure); //��ʼ��PD5
   
    //USART2_RX	  PD6
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOD, &GPIO_InitStructure);  //��ʼ��PD6

   //USART ��ʼ������
	USART_DeInit(USART2);  //��λ����2
	
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART2, &USART_InitStructure); //��ʼ������
		
  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 
}
void uart3_init(u32 bound )
{  	 

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//ʹ��USART3��GPIOCʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOC, ENABLE);

  GPIO_PinRemapConfig(GPIO_PartialRemap_USART3 , ENABLE); // USART3 ������ӳ��
	 //USART3_TX   PC.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��//PC10
   
    //USART3_RX	  PC11
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOC, &GPIO_InitStructure);  //��ʼ��PC11

   //USART ��ʼ������
	USART_DeInit(USART3);  //��λ����3
	
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART3, &USART_InitStructure); //��ʼ������
		
  USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 

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
//����һ���ַ�
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

 
