/************************************************************
**  名称: 样题7主车底层程序
	版本: V5.21-15:45
	修改: 无
*************************************************************/
#include "stm32f10x.h"  
#include "sys.h"					
#include "usart.h"		
#include "delay_drv.h"
#include "init.h"
#include "led.h"		 	 
#include "test.h"
#include "full_automation.h"
#include "djqd.h"
#include "key.h"
#include "xj.h"
#include "csb.h"
#include "hw.h"
#include "74hc595.h"
#include "bh1750.h"
#include "uart_my.h"
#include "canp_hostcom.h"
#include "can_drv.h"
#include "fifo_drv.h"
#include "data_channel.h"
#include "power_check.h"
#include "syn7318.h"
#include "iic.h"
#include "hmc5883l.h"
#include "full_automation.h"


#define  ZCKZ_ADDR    0xAA  // 定义运输标志物地址编号
#define  YSBZW_ADDR   0x02  // 定义运输标志物地址编号
#define  DZ_ADDR      0x03  // 定义道闸标志物地址编号
#define  LEDXS_ADDR   0x04  // 定义LED显示标志物地址编号
#define  JXB_ADDR     0x05  // 定义机械臂标志物地址编号

void Key_Onclink(void);
void IO_Init(void); //IO初始化	
void DIY_BEEP(u16 T_Hz,u8 timer);


u8 G_Tab[6];	   //定义红外发射数组
u8 S_Tab[NUM]; 	   //定义主返回数据数组
u8 C_Tab[NUM]; 	   //定义运动标志物返回数据数组

u8 Stop_Flag=0;    //状态标志位
u8 Track_Flag=0;     //循迹标志位
u8 G_Flag=0;	   //前进标志位
u8 B_Flag=0;	   //后退标志位
u8 L_Flag=0;	   //左转标志位
u8 R_Flag=0;	   //右转标志位
u8 SD_Flag=1;      //运动标志物数据返回允许标志位
u8 Rx_Flag =0;

u16 CodedDisk=0;   //码盘值统计
u16 tempMP=0;	   //接收码盘值
u16 MP;			   //控制码盘值
int Car_Spend = 50;//小车速度默认值
u32 count = 0;	   //在循迹时  计数遇到循迹灯全亮的次数  调节此参数可防止循迹灯误判
int LSpeed;		   //循迹左轮速度
int RSpeed;		   //循迹右轮速度
u8 Line_Flag=0;	   // 
u8 send_Flag=0;	   // 发送标志位

unsigned Light=0; //光照度

u16 error_Flag = 0;

// 主函数

RCC_ClocksTypeDef RCC_Clocks;

void SysTick_Handler(void)
{
	global_times++;
	if(delay_ms_const)
		delay_ms_const--;
}




int main(void)
{

	u8 i;

	global_times = 0;
	SystemInit();
	Delay_us_Init();
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);	
	
	NVIC_Configuration(); 	 //设置NVIC中断分组0:2位抢占优先级，2位响应优先级
		
	
	SYN7318_Init();//语音初始化
	

	IO_Init();                  //IO初始化

	S_Tab[0]=0x55;
	S_Tab[1]=0xaa;

	C_Tab[0]=0x55;
	C_Tab[1]=0x02;
	
	CanP_Init();
	
	Power_Check();  //电量检测  上电先检测一次电量
	Send_Electric( Electric_Buf[0],Electric_Buf[1]);

	STOP();
	Host_Close_UpTrack();
	
 	Set_Track_Init();			         //设置循迹初始化参数
	//   Buart1_init(115200);
	Readcard_daivce_Init();
	
    Send_Debug_Info("A1B2C3\n",8); // 上传调试信息
    //LED_RL(120);
	while(1)
	{	 
		Key_Onclink();
		
		while(Full_Circulate_Flag)
		{
			LED1 = 1;
			LED0 = 0;
			Full_Motion();
			error_Flag = 0;				//超时状态标志位清零
			Wifi_Rx_flag = 0;			//WIFI接收状态标志位清零
			Zigbee_Rx_flag = 0;			//ZigBee接收状态标志位清零
		}
		if(error_Flag == 900)			//90 秒 
		{
			error_Flag = 0;
			Full_Circulate_Flag = 1;   //自动进入全自动
			DIY_BEEP(4,25);
		}
		error_Flag++;
		if(ETC_Open_Flag == 1)	//如果感应不到，小车就前进一点知道感应到为止				
		{
			if(error_Flag == 25 || error_Flag == 50 || 
			   error_Flag == 75 || error_Flag == 100 || error_Flag == 125)		// 5s 10s 15s
			{
				Full_Go(40,3);
			}
			
		}
		
		LED0 = !LED0;					//程序状态	
		delay_ms(100);
		LED1 = 0;
		
		////////////////////////
		if(Wifi_Rx_flag ==1)  			// wifi 接收标 记
		{
			  Wifi_Rx_flag =0;
			  if(Wifi_Rx_Buf[0]==0x55)  // 接收到55开头数据
			  {			  	 
			  	     Normal_data(); 	//正常数据处理
			  }
		}

		if(Rx_Flag ==1)						   //接收到控制指令
		{
			if(Wifi_Rx_Buf[1]==ZCKZ_ADDR) 	   //主车控制
			{
				switch(Wifi_Rx_Buf[2])
				{
					case 0xB1:				   //开始指令   1
						Car_Position_news[0] = Wifi_Rx_Buf[3];		//得到主车起始位置 02
						Car_Position_news[1] = Wifi_Rx_Buf[4];		//得到主车入库位置 05
						Car_Position_news[2] = Wifi_Rx_Buf[5];		//未使用          00
						mark = 5;
						Full_Circulate_Flag = 1;
						break;
					case 0xB2:				                         //形状车牌识别完成
						for(i=0;i<6;i++)
						{
							Car_Text[i] = Hw_Data[i];		//保存车牌
						}		
						break;
					case 0xB3:	//								//保存形状个数
						Shape_Number[1] = Hw_Data[0];			//矩形
						Shape_Number[3] = Hw_Data[1];			//圆
						Shape_Number[5] = Hw_Data[2];   		//三角形
						Shape_Number[7] = Hw_Data[3];			//菱形
						Shape_Number[9] = Hw_Data[4];			//五角星
						Full_Circulate_Flag = 1;
						break;
					case 0x10:  			//红外数据前三位
						Hw_Data[0]=Wifi_Rx_Buf[3];
						Hw_Data[1]=Wifi_Rx_Buf[4];
						Hw_Data[2]=Wifi_Rx_Buf[5];
					break;
					case 0x11: 				//红外数据后三位
						Hw_Data[3]=Wifi_Rx_Buf[3];
						Hw_Data[4]=Wifi_Rx_Buf[4];
						Hw_Data[5]=Wifi_Rx_Buf[5];
					break;
					case 0x12: 				//红外数据后三位
						LI_ADR[0]=Wifi_Rx_Buf[3];//调光挡位
						LI_ADR[1]=Wifi_Rx_Buf[4];//RFID地址
//					    LI_ADR[0]=0x02;//调光挡位
//					    LI_ADR[1]=0x14;//RFID地址
						LI_ADR[2]=0;
					break;
					case 0x20://命令TFT翻页
						if(Wifi_Rx_Buf[3] == 0x01)
						{
							//TFT_turn_over(1);//TFT上一页
							Transmition(H_S,4);
						}
						else if(Wifi_Rx_Buf[3] == 0x02)
						{
							//TFT_turn_over(2);//TFT下一页
							Transmition(H_X,4);
						}
						break;
					case 0xB4:
						for(i=0;i<6;i++)
						{
							//Caution_Text[i] = Hw_Data[i];		//保存报警数据
							Caution_Text[i]=HW_K[i];
							Send_Debug_Info("有红外数据",8); // 上传调试信息
							
						}
						Send_Debug_Info(Caution_Text,6); // 上传调试信息
						Full_Circulate_Flag = 1;
						break;
					case 0xB5:
							
						break;
				}
	    	Rx_Flag =0;
			}
		} 
		///////////////////////////////////
		if(Zigbee_Rx_flag ==1)	 //等到ETC感应完成
		{
		    Send_Debug_Info("one\n",6); // 上传调试信息
			Zigbee_Rx_flag =0;
			if((Zigb_Rx_Buf[1]==0x0c) && (ETC_Open_Flag == 1)) 			//ETC
			{
			   if(Zigb_Rx_Buf[2]==0x01)
			   {
					if(Zigb_Rx_Buf[3]==0x01)							//ETC打开
					{
						Full_Circulate_Flag = 1;//进行下一步骤						
					    Send_Debug_Info("ETC\n",6); // 上传调试信息
					}							
			   }
			}
			else if(Zigb_Rx_Buf[1]==0x21)	 	//AGV返回数据
			{
			    Send_Debug_Info("two\n",6); // 上传调试信息
				switch(Zigb_Rx_Buf[2])
				{
					case 0x01:							  //启动成功
						Full_Circulate_Flag = 1;
						break;
					case 0x02:							  //任务完成
						Full_Circulate_Flag = 1;
						break;
					case 0x03:							  //入库完成
						Full_Circulate_Flag = 1;
						break;
				}		
			}	
		}
	}
	
}

void Key_Onclink(void)
{

		if(!KEY0)			  //按键K1按下
		{
			 delay_ms(30);
			 if(!KEY0)
			 {
				 //SYN_7318_One_test(1,0);					//语音识别
				   //Start_AGV_Position = 1;
				   Send_Debug_Info("AGV=1,B4\n",10); // 上传调试信息
			 }
		}
	   if(!KEY1)			//按键K2按下
		 {  
			 delay_ms(30);
			 if(!KEY1)
			 {
			    Start_AGV_Position = 2;
				  Send_Debug_Info("AGV=2,D2\n",10); // 上传调试信息
			 }
		}
	  if(!KEY2)			  //按键K3按下
		{
			delay_ms(30);
			if(!KEY2)
			{ 
				 Start_AGV_Position = 4;
				 Send_Debug_Info("AGV=4,F4\n",10); // 上传调试信息	
			}
		}
		if(!KEY3)			  //按键K4按下
		{
			delay_ms(30);
			if(!KEY3)
			{
				  Start_AGV_Position = 3;
				  Send_Debug_Info("AGV=3,D6\n",10); // 上传调试信息
			}
		}
}

void DIY_BEEP(u16 T_Hz,u8 timer)
{
	u16 i = 0;
	for(i=0;i<timer;i++)
	{
		BEEP = 0;
		delay_ms(timer);
		BEEP = 1;
		delay_ms(timer);
	}
}
/***************************************************************
** 功能：     初始化核心板所使用端口
** 参数：	  无参数
** 返回值：    无
****************************************************************/
void IO_Init(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
    YJ_INIT();			         //初始化硬件
	
    RCC_APB2PeriphClockCmd(PSS_APB|LED_L_APB| BEEP_APB|LED_R_APB|RCC_APB2Periph_GPIOD|LED_SET_APB,ENABLE);							  
   
    GPIO_InitStructure.GPIO_Pin  = PSS_PIN;			//光敏状态
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   //设置上拉输入
 	GPIO_Init(PSS_GPIO, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin  = BEEP_PIN;		 //任务板蜂鸣器
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //设置推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BEEP_GPIO, &GPIO_InitStructure);

   	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;		 //核心板蜂鸣器
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //设置推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	#if SYSTEM_TASK_TYPE==TASK_TYPE1
	
	GPIO_InitStructure.GPIO_Pin  = LED_SET_PIN;		 //任务板左灯
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //设置推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_SET_GPIO, &GPIO_InitStructure);	
	LED_SET=1;


	#elif SYSTEM_TASK_TYPE==TASK_TYPE0
	GPIO_InitStructure.GPIO_Pin  = LED_L_PIN;		 //任务板左灯
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //设置推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_L_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin  = LED_R_PIN;		 //任务板右灯
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //设置推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_R_GPIO, &GPIO_InitStructure);
	
	LED_L=1;	
	LED_R=1;
	#endif
	
	
	BEEP=1;
	beep=0;//关闭任务版蜂鸣器
		
}

