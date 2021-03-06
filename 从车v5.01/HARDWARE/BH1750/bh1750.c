#include "stm32f10x.h"
#include "delay.h"
#include "bh1750.h"
#include "usart.h"
#include "data_channel.h"

#define	 SlaveAddress   0x46 //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
u8  BUF[4];                  //接收数据缓存区


/***************************************************************
** 功能：     IIC端口初始化
** 参数：	  无参数
** 返回值：    无
****************************************************************/
void IIC_Init(void)
{					     							 

//	RCC->APB2ENR |= 1<<4;     
//	GPIOC->CRL&=0X00FFFFFF;  // PC6 /PC7 --SDA / SCL
//	GPIOC->CRL|=0X77000000;	
//	GPIOC->CRH&=0XFFFFFFF0;  // PC8  ADDR
//	GPIOC->CRH|=0X00000003;
	
 GPIO_InitTypeDef GPIO_InitStructure; 
 RCC_APB2PeriphClockCmd(SDA_APB,ENABLE);
 RCC_APB2PeriphClockCmd(SCL_APB,ENABLE);
 RCC_APB2PeriphClockCmd(ADDR_APB,ENABLE);

 GPIO_InitStructure.GPIO_Pin  =SDA_PIN;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //通用开漏输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(SDA_GPIO, &GPIO_InitStructure);

 GPIO_InitStructure.GPIO_Pin  =SCL_PIN;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //通用开漏输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(SCL_GPIO, &GPIO_InitStructure);      

 GPIO_InitStructure.GPIO_Pin  =ADDR_PIN;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //通用开漏输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(ADDR_GPIO, &GPIO_InitStructure);


}
/***************************************************************
** 功能：     产生IIC起始信号
** 参数：	  无参数
** 返回值：    无
****************************************************************/
void BH1750_Start()
{
    SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}
/***************************************************************
** 功能：     产生IIC停止信号
** 参数：	  无参数
** 返回值：    无
****************************************************************/
void BH1750_Stop()
{
	SDA_OUT();//sda线输出
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
	IIC_SCL=1;
 	delay_us(4);
	IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);
}
/***************************************************************
** 功能：     产生IIC应答信号
** 参数：	  无参数
** 返回值：    无
****************************************************************/
void BH1750_SendACK(u8 ack)
{
    SDA_OUT();          //sda线输出
	if(ack)IIC_SDA=1;   //写应答信号
	else IIC_SDA=0;          
    IIC_SCL=1;          //拉高时钟线
    delay_us(2);        //延时
    IIC_SCL=0;          //拉低时钟线
    delay_us(2);        //延时
}

/***************************************************************
** 功能：     产生IIC接收信号
** 参数：	  无参数
** 返回值：    无
****************************************************************/
u8 BH1750_RecvACK()
{
    u8 data;
	  SDA_IN();         //SDA设置为输入
    IIC_SCL=1;        //拉高时钟线
    delay_us(2);      //延时
    data = READ_SDA;  //读应答信号
    IIC_SCL=0;        //拉低时钟线
    delay_us(2);      //延时
    return data;
}
/***************************************************************
** 功能：     向IIC总线发送一个字节数据
** 参数：	  dat：一字节数据
** 返回值：    无
****************************************************************/
void BH1750_SendByte(u8 dat)
{
    u8 i,bit;
	  SDA_OUT();               //sda线输出
    for (i=0; i<8; i++)      //8位计数器
    {
	    bit=dat&0x80;
		if(bit) IIC_SDA=1;
		else IIC_SDA=0;
        dat <<= 1;           //移出数据的最高位
        IIC_SCL=1;           //拉高时钟线
        delay_us(2);         //延时
        IIC_SCL=0;           //拉低时钟线
        delay_us(2);         //延时
    }
    BH1750_RecvACK();
}
/***************************************************************
** 功能：     从IIC总线接收一个字节数据
** 参数：	  无参数
** 返回值：   dat：接收一字节数据
****************************************************************/
u8 BH1750_RecvByte()
{
    u8 i;
    u8 dat = 0;
	SDA_IN();               //SDA设置为输入
    IIC_SDA=1;              //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)     //8位计数器
    {
        dat <<= 1;
        IIC_SCL=1;          //拉高时钟线
        delay_us(2);        //延时
		if(READ_SDA) dat+=1;               
        IIC_SCL=0;          //拉低时钟线
        delay_us(2);        //延时
    }
    return dat;
}

/***************************************************************
** 功能：     向bh1750写入命令
** 参数：	  无参数
** 返回值：   无
****************************************************************/
void Single_Write_BH1750(u8 REG_Address)
{
    BH1750_Start();                  //起始信号
    BH1750_SendByte(SlaveAddress);   //发送设备地址+写信号
    BH1750_SendByte(REG_Address);    //内部寄存器地址，
    BH1750_Stop();                   //发送停止信号
}

/***************************************************************
** 功能：     连续读出BH1750内部数据 
** 参数：	  无参数
** 返回值：   无
****************************************************************/
void Multiple_Read_BH1750(void)
{   u8 i;	
    BH1750_Start();                          //起始信号
    BH1750_SendByte(SlaveAddress+1);         //发送设备地址+读信号
	
	for (i=0; i<3; i++)                      //连续读取2个地址数据，存储中BUF
    {
        BUF[i] = BH1750_RecvByte();          //BUF[0]存储0x32地址中的数据
        if (i == 3)
        {
           BH1750_SendACK(1);                //最后一个数据需要回NOACK
        }
        else
        {		
          BH1750_SendACK(0);                //回应ACK
       }
   }


    BH1750_Stop();                          //停止信号
    delay_ms(150);
}

/***************************************************************
** 功能：     初始化BH1750
** 参数：	  无参数
** 返回值：   无
****************************************************************/
void BH1750_Init()
{
   Single_Write_BH1750(0x01);  
   ADDR = 0;  //将ADDR位初始化拉低
}

/***************************************************************
** 功能：     读取光照度
** 参数：	  无参数
** 返回值：   data：返回光照度值
****************************************************************/
u16 Dispose(void)
{  
	static float temp;
	u16 data;
	int dis_data ;
    Single_Write_BH1750(0x01);   // power on
    Single_Write_BH1750(0x10);   // H- resolution mode
    delay_ms(200);              //延时200ms
    Multiple_Read_BH1750();       //连续读出数据，存储在BUF中
    dis_data=BUF[0];
    dis_data=(dis_data<<8)+BUF[1];//合成数据，即光照数据
    temp=(float)dis_data/1.2;
	data=(int)temp;
    return data;        
}



