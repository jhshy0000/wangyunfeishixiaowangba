#ifndef __BH1750_H
#define __BH1750_H	 
#include "sys.h"


//NG1750�˿ڶ���

//SDA ��������	   //��Ҫʹ��GPIOB->CRL|=3<<7,��ȻIIC�᲻������
#define SDA_IN()  
#define SDA_OUT()  
//IO��������	 
#define IIC_SCL    PCout(7) //SCL 
#define IIC_SDA    PCout(6)  //���SDA 	 
#define READ_SDA   PCin(6)   //����SDA 
#define ADDR       PCout(8)  //ADDR	

#define SDA_APB			  RCC_APB2Periph_GPIOC //SDAʱ��
#define SCL_APB			  RCC_APB2Periph_GPIOC //SCLʱ��
#define ADDR_APB		  RCC_APB2Periph_GPIOC //ADDRʱ��

#define SDA_GPIO 		  GPIOC //SDA GPIO��
#define SCL_GPIO		  GPIOC //SCL GPIO��
#define ADDR_GPIO		  GPIOC //ADDR GPIO��

#define SDA_PIN			  GPIO_Pin_6  //SDA �ܽ�
#define SCL_PIN			  GPIO_Pin_7 //SCL �ܽ�
#define ADDR_PIN		  GPIO_Pin_8  //ADDR �ܽ�


//IIC���в�������
void IIC_Init(void);
void BH1750_Init(void);
void conversion(u16 temp_data);
void  Single_Write_BH1750(u8 REG_Address);            //����д������
u8 Single_Read_BH1750(u8 REG_Address);                //������ȡ�ڲ��Ĵ�������
void  Multiple_Read_BH1750(void);                     //�����Ķ�ȡ�ڲ��Ĵ�������
//------------------------------------

void BH1750_Start(void);                //��ʼ�ź�
void BH1750_Stop(void);                 //ֹͣ�ź�
void BH1750_SendACK(u8 ack);            //Ӧ��ACK
u8  BH1750_RecvACK(void);               //��ack
void BH1750_SendByte(u8 dat);        	//IIC�����ֽ�д
u8 BH1750_RecvByte(void);               //IIC�����ֽڶ�
u16 Dispose(void);						//�õ�����ǿ��
		 				    
void Bh1750_Test(void);								
								
#endif
