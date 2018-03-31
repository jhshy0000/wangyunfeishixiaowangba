#ifndef __BH1750_H
#define __BH1750_H	 
#include "sys.h"


//NG1750端口定义

//SDA 方向设置	   //不要使用GPIOB->CRL|=3<<7,不然IIC会不起作用
#define SDA_IN()  
#define SDA_OUT()  
//IO操作函数	 
#define IIC_SCL    PCout(7) //SCL 
#define IIC_SDA    PCout(6)  //输出SDA 	 
#define READ_SDA   PCin(6)   //输入SDA 
#define ADDR       PCout(8)  //ADDR	

#define SDA_APB			  RCC_APB2Periph_GPIOC //SDA时钟
#define SCL_APB			  RCC_APB2Periph_GPIOC //SCL时钟
#define ADDR_APB		  RCC_APB2Periph_GPIOC //ADDR时钟

#define SDA_GPIO 		  GPIOC //SDA GPIO组
#define SCL_GPIO		  GPIOC //SCL GPIO组
#define ADDR_GPIO		  GPIOC //ADDR GPIO组

#define SDA_PIN			  GPIO_Pin_6  //SDA 管脚
#define SCL_PIN			  GPIO_Pin_7 //SCL 管脚
#define ADDR_PIN		  GPIO_Pin_8  //ADDR 管脚


//IIC所有操作函数
void IIC_Init(void);
void BH1750_Init(void);
void conversion(u16 temp_data);
void  Single_Write_BH1750(u8 REG_Address);            //单个写入数据
u8 Single_Read_BH1750(u8 REG_Address);                //单个读取内部寄存器数据
void  Multiple_Read_BH1750(void);                     //连续的读取内部寄存器数据
//------------------------------------

void BH1750_Start(void);                //起始信号
void BH1750_Stop(void);                 //停止信号
void BH1750_SendACK(u8 ack);            //应答ACK
u8  BH1750_RecvACK(void);               //读ack
void BH1750_SendByte(u8 dat);        	//IIC单个字节写
u8 BH1750_RecvByte(void);               //IIC单个字节读
u16 Dispose(void);						//得到关照强度
		 				    
void Bh1750_Test(void);								
								
#endif
