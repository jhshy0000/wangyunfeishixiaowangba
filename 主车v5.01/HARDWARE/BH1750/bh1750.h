#ifndef __BH1750_H
#define __BH1750_H	 
#include "sys.h"


//NG1750�˿ڶ���

//SDA ��������	   //��Ҫʹ��GPIOB->CRL|=3<<7,��ȻIIC�᲻������
#define SDA_IN()  
#define SDA_OUT()  
//IO��������	 
#define IIC_SCL    PEout(5) //SCL 
#define IIC_SDA    PEout(2)  //���SDA 	 
#define READ_SDA   PEin(2)   //����SDA 
#define ADDR       PEout(7)  //ADDR	

#define SDA_APB			  RCC_APB2Periph_GPIOE //SDAʱ��
#define SCL_APB			  RCC_APB2Periph_GPIOE //SCLʱ��
#define ADDR_APB		  RCC_APB2Periph_GPIOE //ADDRʱ��

#define SDA_GPIO 		  GPIOE //SDA GPIO��
#define SCL_GPIO		  GPIOE //SCL GPIO��
#define ADDR_GPIO		  GPIOE//ADDR GPIO��

#define SDA_PIN			  GPIO_Pin_2  //SDA �ܽ�
#define SCL_PIN			  GPIO_Pin_5 //SCL �ܽ�
#define ADDR_PIN		  GPIO_Pin_7  //ADDR �ܽ�

//IIC���в�������
void IIC_Init(void);
void BH1750_Init(void);
void conversion(unsigned int temp_data);
void  Single_Write_BH1750(u8 REG_Address);            //����д������
u8 Single_Read_BH1750(u8 REG_Address);                //������ȡ�ڲ��Ĵ�������
void  Multiple_Read_BH1750(void);                     //�����Ķ�ȡ�ڲ��Ĵ�������
//------------------------------------

void BH1750_Start(void);                    //��ʼ�ź�
void BH1750_Stop(void);                     //ֹͣ�ź�
void BH1750_SendACK(u8 ack);                //Ӧ��ACK
u8  BH1750_RecvACK(void);                   //��ack
void BH1750_SendByte(u8 dat);               //IIC�����ֽ�д
u8 BH1750_RecvByte(void);                   //IIC�����ֽڶ�
unsigned int Dispose(void);
		 				    
#endif
