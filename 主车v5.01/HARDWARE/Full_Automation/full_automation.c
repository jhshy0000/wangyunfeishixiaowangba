#include "stm32f10x.h"  
#include "sys.h"					
#include "usart.h"		
#include "delay_drv.h"
#include "init.h"
#include "led.h"		 	 
#include "test.h"
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
#include "rc522.h"
#include "full_automation.h"
#include "string.h"

/*
十六进制    符号      名称
  0x22       "       双引号
  0x27		  ’　    闭单引号
  0x60      ‘        开单引号
  0x72       r       小写r
  0x2F       /       斜杠
  0x5C       \       反斜杠
  0x2C       ,       逗号
  0x2E       .       句号
  0x2D       -       减号
  0x3C       <       小于
  0x3E       >       大于
  0x28		 (       开括号
  0x29       )       闭括号
  0x41       A       大写A
  0x42       B       大写B
  0x43       C       大写C
  0x44       D       大写D
  0x45       E       大写E
  0x46       F       大写F
  0x47       G       大写G
  0x30       0       数字0
  0x31       1       数字1
  0x32       2       数字2
  0x33       3       数字3
  0x34       4       数字4
  0x35       5       数字5
  0x36       6       数字6
  0x37       7       数字7
  0x38       8       数字8
  0x39       9       数字9
*/

/****************************************************************************************
*****************************************************************************************
		全自动参数修改
*****************************************************************************************
****************************************************************************************/
//主车参数
#define Go_sp    70ommit your changes or stash them before you merge.
Aborting
fatal: revert failed			//前进速度
shashfjh
#define Back_sp  70			//后退速度
#define L_sp     90		  //左转速度
#define R_sp	 90			//右转速度
#define XJ_sp    50		//循迹速度

#define Go_mp    21		    //前进码盘
#define Back_mp  100		//后退码盘

u8 Car_Text[6];			    //车牌数据
u8 Caution_Text[6];		    //报警数据
u8 Hw_Data[6];			    //红外数据

u8 RX_AGV_Data[] = {0x55,0x02,0x00,0x00,0x00,0x00,0x00,0xBB};// 从车控制控制指令数据

u8 RXRFID[16];												 //存放RFID数据
u8 LI_ADR[3];//
//u8 Read_CardOK=0;//0：没有都卡，1：读卡成功
u8 RFID_ADRSS;//RFID块地址
u8 LIGHT_Grent;//光照等级

u8 TXRFID[16] = {0x28,0x22,0x44,0x32,0x2D,0x3E,0x22,0x2C,0x22,0x44,0x32,0x2D,0x3E,0x45,0x22,0x29};//第二轮


/*
        D6
		3
		^
 F4  4<   >1  B4
	    V
		2
		D2
*/
u8 Start_AGV_Position = 1;					             //根据RFID内信息得到AGV起始位置坐标

/*1——A车库  2——B车库 3——C车库 4——D车库 5——E车库 6——F车库*/
/*其中 1——A车库 、5——E车库 已经被占用*/
u8 End_AGV_Position = 1;					//根据RFID内信息得到AGV车库位置坐标
/* 
   1——起点(D2)	2——距离起点1/4(E2)   3——中点(F2)   4——距离终点1/4(F3)   5——终点(F4)
*/
u8 RFID_Card_Position = 0;								//RFID卡片位置标志位  默认位置为5

u32 mark = 0;											//全自动顺序标志位

u8 Full_Circulate_Flag = 0;								//全自动循环标志位


//                      A a  B b  C c  D d  E e
u8 Shape_Number[10] = {10,0,11,0,12,0,13,0,14,0};	 	//形状个数

//Car_Position_news[0]  								//为主车起始坐标位置  B1 、A2、A4
// B1-------1
// A2-------2
// A4-------3
//Car_Position_news[1]  								//主车入库坐标位置      B、D、F
// D-------4
// E-------5
// F-------6
//Car_Position_news[2]									//未使用

u8 Car_Position_news[3] = {0x00,0x00,0x00};				//小车位置坐标信息存放数组

u8 ETC_Open_Flag = 0;									//允许ETC打卡返回标志位

//u8 Speech_recognition_data[] = {0xAF,0x06,0x00,0x02,0x00,0x00,0x01,0xBF}; //语音识别正确标志数据

/*****************************************************************************************
	参数修改END
*****************************************************************************************/	                                            //1——D2；2——E2；3——F2；4——F3；5——F4	

/****************************************************************
函数功能：读卡设备初始化
参	  数：sp 速度  mp 码盘
返回  值：无
*****************************************************************/
void Readcard_daivce_Init()
{
	uart1_init(9600);	// 串口初始化为9600
	delay_ms(100);
	InitRc522();		  //读卡器初始化
	delay_ms(1000);
	uart1_init(115200);
	delay_ms(100);
}
/****************************************************************
函数功能：秒单位级延时
参	  数：T = 1 即 1秒
返回  值：无
*****************************************************************/
void Delay_S(u8 T)
{
	u8 i=0;
	for(i=0;i<T;i++)
	{
		delay_ms(1000);
	}
}
/****************************************************************
函数功能：全自动停止
参	  数：sp 速度  mp 码盘
返回  值：无
*****************************************************************/
void Full_STOP()
{
	Track_Flag=0;MP=0;
	Stop_Flag=0;G_Flag=0;B_Flag=0;L_Flag=0;R_Flag=0; 
	STOP();
}
/****************************************************************
函数功能：全自动前进
参	  数：sp 速度  mp 码盘
返回  值：无
*****************************************************************/
void Full_Go(u8 sp,u8 mp)	
{
	MP=0;G_Flag=1;	Stop_Flag=0;tempMP=0;			 					
	tempMP = mp;
	Car_Spend = sp;
	Control(Car_Spend,Car_Spend);
	while(Stop_Flag != 0x03);
}
/****************************************************************
函数功能：前进
参	  数：sp 速度  mp 码盘
返回  值：无
*****************************************************************/
void Go(u8 sp,u16 mp)
{
	MP=0;G_Flag=1;	Stop_Flag=0;tempMP=0;			 					
	tempMP = mp;
	Car_Spend = sp;
	Control(Car_Spend,Car_Spend);
}
/****************************************************************
函数功能：全自动后退
参	  数：sp 速度  mp码盘
返回  值：无
*****************************************************************/
void Full_Back(u8 sp,u16 mp)
{
	MP=0;B_Flag=1;	Stop_Flag=0;tempMP=0;
	tempMP = mp;
	Car_Spend =	sp;
	Control(-Car_Spend,-Car_Spend);
	while(Stop_Flag != 0x03);
}
/****************************************************************
函数功能：全自动左转
参	  数：sp 速度
返回  值：无
*****************************************************************/
void Full_Left(u8 sp)
{
	MP=0;L_Flag=1;	Stop_Flag=0;
	Car_Spend = sp;
	Host_Open_UpTrack( Up_Track_Speed );
	Control(-Car_Spend,Car_Spend);
	while(Stop_Flag != 0x02);
}
/****************************************************************
函数功能：左转
参	  数：sp 速度
返回  值：无
*****************************************************************/
void Left(u8 sp)
{
	MP=0;L_Flag=1;	Stop_Flag=0;
	Car_Spend = sp;
	Host_Open_UpTrack( Up_Track_Speed );
	Control(-Car_Spend,Car_Spend);
}
/****************************************************************
函数功能：全自动右转
参	  数：sp 速度
返回  值：无
*****************************************************************/
void Full_Right(u8 sp)
{
	MP=0;R_Flag=1;	Stop_Flag=0;
	Car_Spend = sp;
	Host_Open_UpTrack( Up_Track_Speed );
	Control(Car_Spend,-Car_Spend);
	while(Stop_Flag != 0x02);
}
/****************************************************************
函数功能: 右转
参	  数：sp 速度
返回  值：无
*****************************************************************/
void Right(u8 sp)
{
	MP=0;R_Flag=1;	Stop_Flag=0;
	Car_Spend = sp;
	Host_Open_UpTrack( Up_Track_Speed );
	Control(Car_Spend,-Car_Spend);
}
/****************************************************************
函数功能: 全自动45度转
参	  数：sp 速度 RL = 1 左转 RL = 2 右转 timer 决定角度  经验值 timer = 28 即45度
返回  值：无
*****************************************************************/
void Full_RL_45(u8 sp,u8 RL,u8 timer)
{
	 if(RL == 1)
	 {
		Left(sp);
		while(timer>MP);
		Full_STOP();
	 } 
	 else if(RL == 2) {
		Right(sp);
		while(timer>MP);
		Full_STOP();
	 }
}
/****************************************************************
函数功能：全自动循迹
参	  数：sp 速度
返回  值：无
*****************************************************************/

void Full_Tracking(u8 sp)
{
	
	Car_Spend = sp;     //规定了转速
	Track_Flag=1;       //开启循迹
	MP=0; 				//码盘值请0	  
	Host_Open_UpTrack( Up_Track_Speed );  // 开启寻迹数据上传
	delay_ms( 100); 					  // 等待一会寻迹数据上传
	TIM_Cmd(TIM2, ENABLE);

	while(Stop_Flag != 0x01);
}
/****************************************************************
函数功能：非全自动循迹
参	  数：sp 速度
返回  值：无
*****************************************************************/
void Tracking(u8 sp)
{
	Car_Spend = sp;
	Track_Flag=1;MP=0; 					  
	Host_Open_UpTrack( Up_Track_Speed );  // 开启寻迹数据上传
	delay_ms( 100); 					  // 等待一会寻迹数据上传
	TIM_Cmd(TIM2, ENABLE);
}
/****************************************************************
函数功能：指定距离循迹函数
参	  数：sp 循迹速度  mp 循迹距离 
返回  值：无
*****************************************************************/
void MP_Tracking(u8 sp,u8 mp)
{
	Car_Spend = sp;
	Track_Flag=1;MP=0; 					  
	Host_Open_UpTrack( Up_Track_Speed );  // 开启寻迹数据上传
	delay_ms( 100); 					  // 等待一会寻迹数据上传
	TIM_Cmd(TIM2, ENABLE);
	while(MP<mp);
	Full_STOP();
}
/****************************************************************
函数功能：LED数码管控制
参	  数：Mode_num=1 打开计时  Mode_num=2 关闭计时
返回  值：无
*****************************************************************/
void Control_LED_Nixie_light(u8 Mode_num)
{
	if(Mode_num == 1){
		send_data_zigbee(SMG_JSK,8);		//打开计时
	} else if(Mode_num == 2) {
		send_data_zigbee(SMG_JSG,8);		//关闭计时
	}
}
/****************************************************************
函数功能：LED数码管显示数据
参	  数：*HW 红外指令地址
返回  值：无 
*****************************************************************/
void Control_LED_show_data(u8 X,u8 *Data)
{
  u8 LED_Data[8] = {0x55,0x04,0x00,0x00,0x00,0x00,0x00,0xBB};
  if(X == 1)					//第一排
  {
	  LED_Data[2] = 0x01;  
  } else if( X == 2 ) {	 		//第二排 
	  LED_Data[2] = 0x02;  
  }
  LED_Data[3] = (u8)((Data[0]&0x0F) << 4)|(Data[1]&0x0F);
  LED_Data[4] = (u8)((Data[2]&0x0F) << 4)|(Data[3]&0x0F);
  LED_Data[5] = (u8)((Data[4]&0x0F) << 4)|(Data[5]&0x0F);
  LED_Data[6] = (u8)(LED_Data[2] + LED_Data[3] +LED_Data[4] + LED_Data[5])%256;
  send_data_zigbee(LED_Data,8);
}
/****************************************************************
函数功能：红外控制
参	  数：*HW 红外指令地址
返回  值：无
*****************************************************************/
void Control_Infrared_alarm(u8 *HW)
{
	Transmition(HW,6);					//报警控制	
}
/****************************************************************
函数功能：立体显示车牌
参	  数：*data 车牌数据指针
返回  值：无
*****************************************************************/
void Stereo_Display(u8 *data)
{
	u8 Car_data[6] = {0xFF,0x00,0x00,0x00,0x00,0x00};
	//u8 i = 0;

	Car_data[1] = 0x20;
	Car_data[2] = data[0];
	Car_data[3] = data[1];
	Car_data[4] = data[2];
	Car_data[5] = data[3];
	Transmition(Car_data,6);
	Send_Debug_Info(Car_data,6); // 上传调试信息
	delay_ms(500);

	Car_data[1] = 0x10;
	Car_data[2] = data[4];
	Car_data[3] = data[5];

	switch(RFID_Card_Position)
	{
		case 1:					//F4
			Car_data[4] = 0x46;
			Car_data[5] = 0x34;
			break;
		case 2:					//F3
			Car_data[4] = 0x46;
			Car_data[5] = 0x33;
			break;
		case 3:					//F2
			Car_data[4] = 0x46;
			Car_data[5] = 0x32;
			break;
		case 4:					//E2
			Car_data[4] = 0x45;
			Car_data[5] = 0x32;
			break;
		case 5:					//D2
			Car_data[4] = 0x44;
			Car_data[5] = 0x32;
			break;
	}
	Transmition(Car_data,6);
	Send_Debug_Info(Car_data,6); // 上传调试信息
	Send_Debug_Info("\n",2); // 上传调试信息
}
/****************************************************************
函数功能：道闸控制
参	  数：无
返回  值：无
*****************************************************************/
void Control_Sluice_gate()
{
	send_data_zigbee(DZ_K,8);
}
/****************************************************************
函数功能：磁悬浮控制
参	  数：无
返回  值：无
*****************************************************************/
void Control_Magnetic_suspension(void)
{
	send_data_zigbee(Ms_Data,8);
}
/****************************************************************
函数功能：TFT翻页控制
参	  数：无
返回  值：无
*****************************************************************/
void TFT_turn_over(u8 num)
{
	 if(num == 1)
	 {
		send_data_zigbee(TFT_UP,8);
	 } else if(num == 2) {
		send_data_zigbee(TFT_down,8);
	 }
}
/****************************************************************
函数功能：调光排序
参	  数：End_Goal 目标档位 1~4
返回  值：无
*****************************************************************/
void Full_Light_Goal(u8 End_Goal)
{
	u16 Light_num[4];			//光强度数值存放数组
	u16 Start_Light = 0;		//起始光强度值
	u8  Light_Text[] = "0000";	//显示光强度数组
	int i=0, j=0;
	u16 temp = 0;
	
	if((End_Goal <= 8) && (End_Goal>4))		//对目标档位数据处理
	{
		End_Goal = End_Goal - 4;
	}
	if((End_Goal <= 16) && (End_Goal>8))	
	{
		End_Goal = End_Goal - 8;
	}
	
	Transmition(H_1,4);
	delay_ms(3000);
	Light_num[0] = Dispose();	//得到四个档位的光强度值
	
	Transmition(H_1,4);
	delay_ms(3000);
	Light_num[1] = Dispose();
	
	Transmition(H_1,4);
	delay_ms(3000);
	Light_num[2] = Dispose();
	
	Transmition(H_1,4);
	delay_ms(3000);
	Light_num[3] = Start_Light = Dispose();
	
	for(i=0;i<4;i++)
	{
		Light_Text[0] = Light_num[i]/1000+0x30;
		Light_Text[1] = Light_num[i]%1000/100+0x30;
		Light_Text[2] = Light_num[i]%100/10+0x30;
		Light_Text[3] = Light_num[i]%10+0x30;
		Send_Debug_Info(Light_Text,4);
		Send_Debug_Info("\n",2);
	}
	
	Send_Debug_Info("\n",2);
	
		for (j = 0; j < 3; j++)
		{
		  for (i = 0; i < 3 - j; i++)
		  {
		   if (Light_num[i] > Light_num[i + 1])
		   {
					temp = Light_num[i]; 
					Light_num[i] = Light_num[i + 1]; 
					Light_num[i + 1] = temp;
		   }
		  }
		 } 
	
	for(i=0;i<4;i++)
	{
		Light_Text[0] = Light_num[i]/1000+0x30;
		Light_Text[1] = Light_num[i]%1000/100+0x30;
		Light_Text[2] = Light_num[i]%100/10+0x30;
		Light_Text[3] = Light_num[i]%10+0x30;
		Send_Debug_Info(Light_Text,4);
		Send_Debug_Info("\n",2);
	}
	
	for(i=0;i<4;i++)
	{
		if(Start_Light == Light_num[i])
			{
				if((End_Goal+3-i)%4!=0)
				{
					if(((End_Goal+3-i)%4) == 1)
					{
						Transmition(H_1,4);
					}
					if(((End_Goal+3-i)%4) == 2)
					{
						Transmition(H_2,4);
					}
					if(((End_Goal+3-i)%4) == 3)
					{
						Transmition(H_3,4);
					}
				}
			}
	}


}

/****************************************************************
函数功能：全自动语音播报
参	  数：*s_data 数据指针 length  数据长度
返回  值：无
*****************************************************************/
void full_Speech(u8 *s_data,u16 lenght)
{   
	u16 i = 0;
	u8 Speech[200];
	Speech[0] = 0xFD;
	Speech[1] = (lenght+2)/256;
	Speech[2] = (lenght+2)%256;
	Speech[3] = 0x01;
	Speech[4] = 0x01;
	for(i=0;i<lenght;i++)
	{
		Speech[i+5] = s_data[i];
	}
	delay_ms(350);
	send_data_zigbee(Speech,lenght+5);
	delay_ms(350);
}

/****************************************************************
函数功能：全自动主车返回数据
参	  数：Data = 0XC1 到达TFT标志物处
		  Data = 0xC2 到达静态标志物
返回  值：无
*****************************************************************/
void Full_Return_Data(u8 Data)
{
	S_Tab[2] = Data;
	send_data_wifi(S_Tab , 10);
}
/****************************************************************
函数功能：AGV控制指令校验
参	  数：无
返回  值：无
*****************************************************************/
void AGV_Send_Data()
{
	RX_AGV_Data[6] = (RX_AGV_Data[2] + RX_AGV_Data[3] 
					+ RX_AGV_Data[4] + RX_AGV_Data[5])%256;		//得到校验和;
	
	send_data_zigbee(RX_AGV_Data,8);							//发送数据
}
/****************************************************************
函数功能：AGV发送红外数据
参	  数：*Infrared_Data  AGV红外数据地址
返回  值：无
*****************************************************************/

void AGV_Control_Infrared(u8 *Infrared_Data)
{
	
	RX_AGV_Data[2] = 0x10;
	RX_AGV_Data[3] = Infrared_Data[0];
	RX_AGV_Data[4] = Infrared_Data[1];
	RX_AGV_Data[5] = Infrared_Data[2];
	AGV_Send_Data();
	delay_ms(1000);
	RX_AGV_Data[2] = 0x11;
	RX_AGV_Data[3] = Infrared_Data[3];
	RX_AGV_Data[4] = Infrared_Data[4];
	RX_AGV_Data[5] = Infrared_Data[5];
	AGV_Send_Data();
	delay_ms(1000);
}
/****************************************************************
函数功能：AGV发送红外数据
参	  数：*Infrared_Data  AGV红外数据地址返回  值：无
*****************************************************************/
void AGV_Start()
{
	RX_AGV_Data[2] = 0x80;
	RX_AGV_Data[3] = Start_AGV_Position;
	RX_AGV_Data[4] = End_AGV_Position;
	RX_AGV_Data[5] = 0x00;
	AGV_Send_Data();
	delay_ms(2000);
}
/****************************************************************
函数功能：主车向AGV发送运行状态
参	  数unm  运行状态 具体参考通讯协议
返回  值：无
*****************************************************************/
void Send_AGV_Run_flag(u8 num)
{
	RX_AGV_Data[2] = num;
	RX_AGV_Data[3] = 0x00;
	RX_AGV_Data[4] = 0x00;
	RX_AGV_Data[5] = 0x00;
	AGV_Send_Data();
	delay_ms(1000);
}
/****************************************************************
函数功能：RFID数据处理
参	  数*data RFID 数据指针
返回  值：无
*****************************************************************/
void RFID_Data_Handler(u8 *data)//此处右16个数据
{	
	u8 show_analyze_data[7];	
	u8 i = 0,j = 0;
	
	for(i=0;i<10;i++)//至少需要保留6个数据储存
	{
		if(data[i] == 0x23)							//判断 #
		{
		 if((data[i+3] == 0x2B) && (data[i+4] == 0x3F) && (data[i+6] == 0x23))  //判断 +?#		
		 {
			 for(j=0;j<7;j++)
			 {			 
				show_analyze_data[j] = data[j+i];	//数据迁移					
			 }
			 Send_Debug_Info(show_analyze_data,7); 	//将解析后的RFID数据显示
			 
			switch(data[i+1])					    //AGV车起始位置方向
			{
				case 0x42: //ASCII "B"			    //B4   右转
					   // if(data[i+2] == 0x34)
						Start_AGV_Position = 1;
					break;
				case 0x44: //ASCII "D"
					if(data[i+2] == 0x32)	   	//D2    无动作       //ASCII "2"	
					{
						Start_AGV_Position = 2;							
					} 
					else if(data[i+2] == 0x36)  //D6   两个左转/右转	 //ASCII "6"
					{
						Start_AGV_Position = 3;
					}
					break;
				case 0x46: //ASCII "F"	
						//if(data[i+2] == 0x34)					//F4	左转
						Start_AGV_Position = 4;
					break;		
			}

			switch(data[i+5])					//AGV车车库位置  //
			{
				case 0x41:						//A
					End_AGV_Position = 1;
				break;
				case 0x42:						//B
					End_AGV_Position = 2;
				break;
				case 0x43:						//C
					End_AGV_Position = 3;
				break;
				case 0x44:					    //D
					End_AGV_Position = 4;
				break;
				case 0x45:					    //E
					End_AGV_Position = 5;
				break;
				case 0x46:					    //F
					End_AGV_Position = 6;
				break;
			}
		 }		 
	  }
	}
}
/****************************************************************
函数功能：寻找RFID
参	  数：
返回  值：无
*****************************************************************/
u8 Look_RFID(u8 s)
{
		char status = 0;
		u8 CT[2];									//卡类型
		u8 SN[4]; 									//卡号
		u8 KEY[6]={0xff,0xff,0xff,0xff,0xff,0xff};  //密钥
		//u8 s = 0x2c;       							// #define   Status2Reg   0x08	
		u8 op=(s/4)*4+3	;
		//u8 i;
		//u8  j = 0;
		status = PcdRequest(PICC_REQALL,CT);		//寻卡
		Send_Debug_Info("asdf0",5);
		if(status == MI_OK)							//寻卡成功
		{
			status=MI_ERR;
			CT[0] = CT[0] + 0x30;
			CT[1] = CT[1] + 0x30;
			Send_Debug_Info(CT,2);					//卡类型
			Send_Debug_Info("\n",2);
			Send_Debug_Info("Findcard",8);
			Send_Debug_Info(" OK\n",5);
		
			status = PcdAnticoll(SN);				//防冲撞
			if(status == MI_OK)
			{
				status=MI_ERR;
				Send_Debug_Info("anti",4);Send_Debug_Info("-collisi",8);
				Send_Debug_Info(" OK\n",5);
				
				status =PcdSelect(SN);				//选定此卡
				if(status == MI_OK)					  //选定成功
				{
					status=MI_ERR;
					Send_Debug_Info("Pick a",6);Send_Debug_Info(" card",5);
				  Send_Debug_Info(" OK\n",5);
					
					status =PcdAuthState(0x60,op,KEY,SN);		//验证密钥
					if(status == MI_OK)
					{
						status = MI_ERR;
						Send_Debug_Info("test",4);
						Send_Debug_Info(" and",4);
						Send_Debug_Info(" verify",7);
						Send_Debug_Info(" OK\n",5);							
						
						status=PcdRead(s,RXRFID);				//读卡
						if(status == MI_OK)
						{
							status = MI_ERR;
							BEEP = 1;
							Send_Debug_Info("read OK\n",8);
//							for(i=0;i<16;i++)
//							{
//							   DEbugNUM(RXRFID[i]);
//							}		
							return 1;
						}
					}
				}
			}
		}
		return 0;
}
/****************************************************************
函数功能：RFID读写卡
参	  数：
返回  值：无
*****************************************************************/
void Read_Card(u8 s)
{
	    u8 i;
		  u8 RXRFIDH[8];
			if(!Look_RFID(s))
			{
				//Send_Debug_Info("asdf1",5);
				delay_ms(1000);
				if(!Look_RFID(s))
				{
					//Send_Debug_Info("asdf2",5);
					delay_ms(1000);
					if(!Look_RFID(s)){}
				}
			}
			for(i=0;i<8;i++)
			{
				RXRFIDH[i] = RXRFID[i+8];
			}
			///把RFID卡内都出的信息发送到Debug显示

			Send_Debug_Info(RXRFID,8);
			Send_Debug_Info(RXRFIDH,8);
			Send_Debug_Info("\n",2);
			//将都出的信息处理
			RFID_Data_Handler(RXRFID);							
}

void Full_Read_Card()
{
			char status = 0;
			//u8 CT[2];									//卡类型
			u8 SN[4]; 									//卡号
			u8 KEY[6]={0xff,0xff,0xff,0xff,0xff,0xff};  //密钥
			u8 s = 0x01;       							// #define   Status2Reg   0x08	
			u8 RXRFIDH[8];
			
			u8 i = 0; 
			//u8 j = 0;

//		status = PcdRequest(PICC_REQALL,CT);		//寻卡
//		if(status == MI_OK)							//寻卡成功
//		{
			status=MI_ERR;
//			CT[0] = CT[0] + 0x30;
//			CT[1] = CT[1] + 0x30;
//			Send_Debug_Info(CT,2);					//卡类型
//			Send_Debug_Info("\n",2);
//			Send_Debug_Info("Findcard",8);
//			Send_Debug_Info(" OK\n",5);
		
			status = PcdAnticoll(SN);				//防冲撞
			if(status == MI_OK)
			{
				status=MI_ERR;
				Send_Debug_Info("anti",4);Send_Debug_Info("-collision",10);
				Send_Debug_Info(" OK\n",5);
				
				status =PcdSelect(SN);				//选定此卡
				if(status == MI_OK)					//选定成功
				{
					status=MI_ERR;
					Send_Debug_Info("Pick a",6);Send_Debug_Info(" card",5);
				    Send_Debug_Info(" OK\n",5);
					
					status =PcdAuthState(0x60,0x03,KEY,SN);		//验证密钥
					if(status == MI_OK)
					{
						status = MI_ERR;
						Send_Debug_Info("test",4);
						Send_Debug_Info(" and",4);
						Send_Debug_Info(" verify",7);
						Send_Debug_Info(" OK\n",5);
													
//						status = PcdWrite(s,TXRFID);			//写卡
//						if(status == MI_OK)
//						{
//							status = MI_ERR;
//							Send_Debug_Info("write OK",10);
//							Send_Debug_Info("\n",2);	
//						}	
//						delay_ms(500);
						
						status=PcdRead(s,RXRFID);				//读卡
						if(status == MI_OK)
						{
							status = MI_ERR;
							BEEP = 0;
							Send_Debug_Info("read OK\n",8);
							for(i=0;i<8;i++)
							{
								RXRFIDH[i] = RXRFID[i+8];
							}							
							///显示卡
				
							Send_Debug_Info(RXRFID,8);
							Send_Debug_Info(RXRFIDH,8);
							Send_Debug_Info("\n",2);							
						}
						delay_ms(200);
						BEEP = 1;
					}
				}
			}
//	}
}

/****************************************************************
函数功能：全自动寻卡1模式
参	  数：无
返回  值：无
*****************************************************************/
void Full_Find_cardsALL1(u8 num1)
{
	char status = 0;
	u8 RFID_TASK_Flag = 1;
	u8 CT[2];									    //卡类型
	
	Go(Go_sp-5,Go_mp);
	while(RFID_TASK_Flag)
	{
		status = PcdRequest(PICC_REQALL,CT);	    //寻卡
		if(status == MI_OK)							//寻卡成功
		{
			delay_ms(150);
			STOP();
			RFID_Card_Position = num1;					//RFID 位置坐标为 1——F2
			Send_Debug_Info("(1)\n",5);
			delay_ms(1000);
//			Full_Read_Card();						//读卡操作
			Read_Card(RFID_ADRSS);	    //读写卡
			delay_ms(1000);
			BEEP = 1;
			Full_Go(Go_sp,5);				//继续完成前进
			RFID_TASK_Flag = 0;
		} 
		else if(Stop_Flag == 0x03)					//前进完成
		{
			 Send_Debug_Info("RFID\n",8); // 上传调试信息
			 RFID_TASK_Flag = 0;
		}
	}
	Send_Debug_Info("CHU\n",5); // 上传调试信息
	RFID_Data_Handler(RXRFID);		//对RFID内数据进行处理
	Send_Debug_Info("LI\n",4); // 上传调试信息
}
/****************************************************************
函数功能：全自动寻卡
参	   数：无
返回  值：无2,3
*****************************************************************/
void Full_Find_cardsALL2(u8 num1,u8 num2)
{
	char status = 0;
	u8 RFID_TASK_Flag = 1;
	u8 CT[2];									    //卡类型
			
	Tracking(XJ_sp-5);								//循迹
	delay_ms(500);									//等待一段时间再开启寻卡
	RFID_TASK_Flag = 1;
	
	while(RFID_TASK_Flag)
	{
		status = PcdRequest(PICC_REQALL,CT);	    //寻卡
		if(status == MI_OK)						    //寻卡成功
		{
			delay_ms(150);
			STOP();
			RFID_Card_Position = num1;					//RFID 位置坐标为 2——C2
			
			Send_Debug_Info("(2)\n",5);
			
			delay_ms(1000);
			
			 Read_Card(RFID_ADRSS);	    //读写卡
			//Full_Read_Card();						//读卡操作
			delay_ms(1000);
			BEEP = 1;
			Full_Tracking(XJ_sp-5);					//继续循迹
			RFID_TASK_Flag = 0;
		} 
		else if(Stop_Flag == 0x01)					//循迹完成
		{			
			RFID_TASK_Flag = 0;
		}
	}
	
	Go(Go_sp-5,Go_mp);
	RFID_TASK_Flag = 1;
	while(RFID_TASK_Flag)
	{
		status = PcdRequest(PICC_REQALL,CT);	    //寻卡
		if(status == MI_OK)							//寻卡成功
		{
			delay_ms(150);
			STOP();
			RFID_Card_Position = num2;					//RFID 位置坐标为 3——D2
			Send_Debug_Info("(3)\n",5);
			delay_ms(1000);
			
			Read_Card(RFID_ADRSS);	    //读写卡
			//Full_Read_Card();						   //读卡操作
			delay_ms(1000);
			BEEP = 1;
			Full_Go(Go_sp-5,5);						//继续完成前进
			RFID_TASK_Flag = 0;
		} 
		else if(Stop_Flag == 0x03)					//前进完成
		{
			RFID_TASK_Flag = 0;
		}
	}	
	RFID_Data_Handler(RXRFID);		//对RFID内数据进行处理
}

/****************************************************************
函数功能：无延时性寻卡
参	   数：无
返回  值：无
*****************************************************************/
void Full_Find_cardsALL3(u8 num1,u8 num2)
{
	char status = 0;
	u8 RFID_TASK_Flag = 1;
	u8 CT[2];									    //卡类型
			
	Tracking(XJ_sp-5);								//循迹
	RFID_TASK_Flag = 1;
	
	while(RFID_TASK_Flag)
	{
		status = PcdRequest(PICC_REQALL,CT);	    //寻卡
		if(status == MI_OK)						    //寻卡成功
		{
			delay_ms(150);
			STOP();
			RFID_Card_Position = num1;					//RFID 位置坐标为 2——C2
			
			Send_Debug_Info("(2)\n",5);
			
			delay_ms(1000);
			
			 Read_Card(RFID_ADRSS);	    //读写卡
			//Full_Read_Card();						//读卡操作
			delay_ms(1000);
			BEEP = 1;
			Full_Tracking(XJ_sp-5);					//继续循迹
			RFID_TASK_Flag = 0;
		} 
		else if(Stop_Flag == 0x01)					//循迹完成
		{			
			RFID_TASK_Flag = 0;
		}
	}
	
	Go(Go_sp-5,Go_mp);
	RFID_TASK_Flag = 1;
	while(RFID_TASK_Flag)
	{
		status = PcdRequest(PICC_REQALL,CT);	    //寻卡
		if(status == MI_OK)							//寻卡成功
		{
			delay_ms(150);
			STOP();
			RFID_Card_Position = num2;					//RFID 位置坐标为 3——D2
			Send_Debug_Info("(3)\n",5);
			delay_ms(1000);
			
			Read_Card(RFID_ADRSS);	    //读写卡
			//Full_Read_Card();						   //读卡操作
			delay_ms(1000);
			BEEP = 1;
			Full_Go(Go_sp-5,5);						//继续完成前进
			RFID_TASK_Flag = 0;
		} 
		else if(Stop_Flag == 0x03)					//前进完成
		{
			RFID_TASK_Flag = 0;
		}
	}	
	RFID_Data_Handler(RXRFID);		//对RFID内数据进行处理
}


void Full_Find_cardsALL4(u8 num1,u8 num2,u16 timer_del)//定距寻卡
{

	char status = 0;
	u8 RFID_TASK_Flag = 1;
	u8 CT[2];									    //卡类型
			
	Tracking(XJ_sp-5);								//循迹
	delay_ms(timer_del);									//等待一段时间再开启寻卡
	RFID_TASK_Flag = 1;
	
	while(RFID_TASK_Flag)
	{
		status = PcdRequest(PICC_REQALL,CT);	    //寻卡
		if(status == MI_OK)						    //寻卡成功
		{
			delay_ms(150);
			STOP();
			RFID_Card_Position = num1;					//RFID 位置坐标为 2——C2
			
			Send_Debug_Info("(2)\n",5);
			
			delay_ms(1000);
			
			 Read_Card(RFID_ADRSS);	    //读写卡
			//Full_Read_Card();						//读卡操作
			delay_ms(1000);
			BEEP = 1;
			Full_Tracking(XJ_sp-5);					//继续循迹
			RFID_TASK_Flag = 0;
		} 
		else if(Stop_Flag == 0x01)					//循迹完成
		{			
			RFID_TASK_Flag = 0;
		}
	}
	
	Go(Go_sp-5,Go_mp);
	RFID_TASK_Flag = 1;
	while(RFID_TASK_Flag)
	{
		status = PcdRequest(PICC_REQALL,CT);	    //寻卡
		if(status == MI_OK)							//寻卡成功
		{
			delay_ms(150);
			STOP();
			RFID_Card_Position = num2;					//RFID 位置坐标为 3——D2
			Send_Debug_Info("(3)\n",5);
			delay_ms(1000);
			
			Read_Card(RFID_ADRSS);	    //读写卡
			//Full_Read_Card();						   //读卡操作
			delay_ms(1000);
			BEEP = 1;
			Full_Go(Go_sp-5,5);						//继续完成前进
			RFID_TASK_Flag = 0;
		} 
		else if(Stop_Flag == 0x03)					//前进完成
		{
			RFID_TASK_Flag = 0;
		}
	}	
	RFID_Data_Handler(RXRFID);		//对RFID内数据进行处理
}

/****************************************************************
函数功能：全自动主车入库
参	  数：无
// D-------4
// E-------5
// F-------6
返回  值：无
*****************************************************************/
void Full_Car_End_Motion()
{
	if(Car_Position_news[1] == 1)	//如果主车进入A车库	
	{
		if(End_AGV_Position == 1)   //如果AGV车已经存在A车库
		{
			Full_Tracking(XJ_sp);	//主车进入B车库		
			Full_Go(Go_sp,Go_mp);
			Full_Tracking(XJ_sp-10);
			Full_Left(L_sp);
			delay_ms(500);
			Full_Tracking(XJ_sp-10);
			Full_Back(Back_sp,Back_mp-60);	
		}
		else 
		{						//主车进入A车库
			Full_Tracking(XJ_sp);		
			Full_Go(Go_sp,Go_mp);			
			Full_Left(L_sp);
			Full_Tracking(XJ_sp);		
			Full_Go(Go_sp,Go_mp);
			Full_Right(R_sp);				//右转
			Full_Tracking(XJ_sp-10);
			Full_Left(L_sp);
			delay_ms(500);
			Full_Tracking(XJ_sp-10);
			Full_Back(Back_sp,Back_mp-60);	
		}
	} 
	else if(Car_Position_news[1] == 2)//如果主车进入B车库	
	{//如果主车进入E车库
		if(End_AGV_Position == 2)		 //如果AGV车在B车库,主车进入C车库
		{
			Full_Tracking(XJ_sp);		
			Full_Go(Go_sp,Go_mp);
            Full_Right(R_sp);
			Full_Tracking(XJ_sp);		
			Full_Go(Go_sp,Go_mp);			
            Full_Left(L_sp);
			Full_Tracking(XJ_sp-10);
			Full_Left(L_sp);
			delay_ms(500);
			Full_Tracking(XJ_sp-10);
			Full_Back(Back_sp,Back_mp-60);			
		} 
		else 
		{//  主车进入	B车库
			
			Full_Tracking(XJ_sp);	//主车进入B车库		
			Full_Go(Go_sp,Go_mp);
			Full_Tracking(XJ_sp-10);
			Full_Left(L_sp);
			delay_ms(500);
			Full_Tracking(XJ_sp-10);
			Full_Back(Back_sp,Back_mp-60);
		}
	} 
	else if(Car_Position_news[1] == 3) //如果主车进入C车库
	{	
		if(End_AGV_Position == 3)			//如果AGV车在C车库 //主车到达 D车库
		{
			Full_Left(L_sp);
			delay_ms(500);
			Full_Left(L_sp);
			Full_Tracking(XJ_sp);		
			Full_Go(Go_sp,Go_mp);
			Full_Right(R_sp);
			Full_Tracking(XJ_sp);			
			Full_Go(Go_sp,Go_mp);
			Full_Left(L_sp);
			Full_Tracking(XJ_sp-10);
			Full_Left(L_sp);
			delay_ms(500);
			Full_Tracking(XJ_sp-10);
			Full_Back(Back_sp,Back_mp-60);			
		} 
		else //主车到达 C车库
		{
			Full_Tracking(XJ_sp);		
			Full_Go(Go_sp,Go_mp);
            Full_Right(R_sp);
			Full_Tracking(XJ_sp);		
			Full_Go(Go_sp,Go_mp);			
            Full_Left(L_sp);
			Full_Tracking(XJ_sp-10);
			Full_Left(L_sp);
			delay_ms(500);
			Full_Tracking(XJ_sp-10);
			Full_Back(Back_sp,Back_mp-60);	
		}
	}
	else if(Car_Position_news[1] == 4) //如果主车进入D车库
	{	
		if(End_AGV_Position == 4)			//如果AGV车在D车库 //主车到达 E车库
		{
			Full_Left(L_sp);
			delay_ms(500);
			Full_Left(L_sp);
			Full_Tracking(XJ_sp);		
			Full_Go(Go_sp,Go_mp);
			Full_Tracking(XJ_sp-10);
			Full_Left(L_sp);
			delay_ms(500);
			Full_Tracking(XJ_sp-10);
			Full_Back(Back_sp,Back_mp-60);				
		} 
		else //主车到达 D车库
		{
			Full_Left(L_sp);
			delay_ms(500);
			Full_Left(L_sp);
			Full_Tracking(XJ_sp);		
			Full_Go(Go_sp,Go_mp);
			Full_Right(R_sp);
			Full_Tracking(XJ_sp);			
			Full_Go(Go_sp,Go_mp);
			Full_Left(L_sp);
			Full_Tracking(XJ_sp-10);
			Full_Left(L_sp);
			delay_ms(500);
			Full_Tracking(XJ_sp-10);
			Full_Back(Back_sp,Back_mp-60);	
		}
	}
	else if(Car_Position_news[1] == 5) //如果主车进入E车库
	{	
		if(End_AGV_Position == 5)			//如果AGV车在E车库 //主车到达 F车库
		{
			Full_Left(L_sp);
			delay_ms(500);
			Full_Left(L_sp);
			Full_Tracking(XJ_sp);		
			Full_Go(Go_sp,Go_mp);
			Full_Left(L_sp);
			Full_Tracking(XJ_sp);			
			Full_Go(Go_sp,Go_mp);
			Full_Right(R_sp);
			Full_Tracking(XJ_sp-10);
			Full_Left(L_sp);
			delay_ms(500);
			Full_Tracking(XJ_sp-10);
			Full_Back(Back_sp,Back_mp-60);				
		} 
		else //主车到达 E车库
		{
			Full_Left(L_sp);
			delay_ms(500);
			Full_Left(L_sp);
			Full_Tracking(XJ_sp);		
			Full_Go(Go_sp,Go_mp);
			Full_Tracking(XJ_sp-10);
			Full_Left(L_sp);
			delay_ms(500);
			Full_Tracking(XJ_sp-10);
			Full_Back(Back_sp,Back_mp-60);
		}
	}
	else if(Car_Position_news[1] == 6) //如果主车进入F车库
	{	
		if(End_AGV_Position == 6)			//如果AGV车在F车库 //主车到达 A车库
		{
			Full_Tracking(XJ_sp);		
			Full_Go(Go_sp,Go_mp);			
			Full_Left(L_sp);
			Full_Tracking(XJ_sp);		
			Full_Go(Go_sp,Go_mp);
			Full_Right(R_sp);				//右转
			Full_Tracking(XJ_sp-10);
			Full_Left(L_sp);
			delay_ms(500);
			Full_Tracking(XJ_sp-10);
			Full_Back(Back_sp,Back_mp-60);				
		} 
		else //主车到达 F车库
		{
			Full_Left(L_sp);
			delay_ms(500);
			Full_Left(L_sp);
			Full_Tracking(XJ_sp);		
			Full_Go(Go_sp,Go_mp);
			Full_Left(L_sp);
			Full_Tracking(XJ_sp);			
			Full_Go(Go_sp,Go_mp);
			Full_Right(R_sp);
			Full_Tracking(XJ_sp-10);
			Full_Left(L_sp);
			delay_ms(500);
			Full_Tracking(XJ_sp-10);
			Full_Back(Back_sp,Back_mp-60);		
		}
	}
}

/****************************************************************
函数功能：主车起始位置判断响应函数
参	  数：无
// B1-------1
// A2-------2
// A4-------3
返回  值：无
*****************************************************************/
 void Full_Car_Strat_Position_HandlerMsg1()
{
	if(Car_Position_news[0] == 1)				//B1
	{
		Full_Tracking(XJ_sp);  //50
		//Full_Find_cards1();						//进入寻卡模式1  前进寻卡
		Full_Left(L_sp);						//左转
		//Full_Find_cards2_3();					//进入寻卡模式 2_3
		Full_Left(L_sp);						//左转
		delay_ms(500);							//延时
		Full_Left(L_sp);						//左转
		Full_Tracking(XJ_sp);					//循迹
		Full_Go(Go_sp,Go_mp);					//前进	
		
	} else if(Car_Position_news[0] == 2) 
	{		//A2
		
		
		Full_Tracking(XJ_sp);
		
		//Full_Find_cards1();						//进入寻卡模式1  前进寻卡
		Full_RL_45(L_sp,1,33);					//左转45度
	
	} else if(Car_Position_news[0] == 3) {		//A4
		
		Full_Tracking(XJ_sp);
		Full_Go(Go_sp,Go_mp);
		Full_Left(L_sp);						//左转
		Full_Tracking(XJ_sp);
		//Full_Find_cards1();						//进入寻卡模式1  前进寻卡
		Full_Right(R_sp);						//右转
		//Full_Find_cards2_3();					//进入寻卡模式 2_3
		Full_Left(L_sp);						//左转
		delay_ms(500);							//延时
		Full_Left(L_sp);						//左转
		Full_Tracking(XJ_sp);					//循迹
		Full_Go(Go_sp,Go_mp);					//前进
	}
}

/****************************************************************
函数功能：主车起始位置判断响应函数
参	  数：无
// B1-------1
// A2-------2
// A4-------3
返回  值：无
*****************************************************************/
void Full_Car_Strat_Position_HandlerMsg2()
{
	if(Car_Position_news[0] == 1 || Car_Position_news[0] == 3)				//B1 或 A4
	{
		Full_Left(L_sp);						//左转
		delay_ms(500);							//延时
		Full_Left(L_sp);						//左转
		
	} else if(Car_Position_news[0] == 2) {		//A2
		
		Full_Right(R_sp);						//右转
		//Full_Find_cards2_3();					//进入寻卡模式 2_3
		Full_Left(L_sp);						//左转
		delay_ms(500);							//延时
		Full_Left(L_sp);						//左转
		Full_Tracking(XJ_sp);					//循迹
		Full_Go(Go_sp,Go_mp);					//前进
		Full_Left(L_sp);						//左转
		delay_ms(500);							//延时
		Full_Left(L_sp);						//左转	
	} 
}


void LED_RL(u8 timer)
{
	while(1)
	{
	 #if SYSTEM_TASK_TYPE==TASK_TYPE1
		 LED_SET=0;
		 delay_ms(timer);
		 LED_SET=1;
		 delay_ms(timer);
	 #elif SYSTEM_TASK_TYPE==TASK_TYPE0
		if(timer!=0)
		{
			 LED_R = 0;				
			 LED_L = 0;
		}
		else
		{
			 LED_R = 1;				
			 LED_L = 1;		
		}
		
	#endif		
	}
}



void LIGHT_disPLAY(u8 *str,u16 ligh_temp)
{
   u8 temp[4];
	
	temp[0]=ligh_temp/1000+0x30;
	temp[1]=ligh_temp%1000/100+0x30;
	temp[2]=ligh_temp%100/10+0x30;
	temp[3]=ligh_temp%10+0x30;
	
	Send_Debug_Info(str,5);
    Send_Debug_Info(temp,4);
	Send_Debug_Info("\n",2);
}






/****************************************************************
函数功能：全自动起始动作
参	  数：无
返回  值：无
*****************************************************************/
void Full_Motion()
{
	switch(mark)
	{
		case 5://给计时LED开始消息
			Control_LED_Nixie_light(1);				//打开计时 55
			delay_ms(500);
			mark = 10;
			break;
		case 10://小车到达F4，右转45度
		
		    Full_Tracking(XJ_sp);
		    Full_Go(Go_sp,Go_mp);
			Full_Right(R_sp);
		    Full_Tracking(XJ_sp);
		    Full_Go(Go_sp,Go_mp);
			Full_RL_45(R_sp,2,23);					//右转45度
			mark = 15;
			break;//
		case 15://小车向平板发送已到F4处
			Full_Return_Data(0xC1);					   //到达TFT标志物位置
			Full_Circulate_Flag = 0;				   //等待 颜色形状、车牌识别结果
			mark = 20;
			break;
		case 20://开始识别车牌号和形状颜色
			Control_LED_show_data(2,Shape_Number);	//数码管 显示形状个数
			delay_ms(1000);
			Control_LED_show_data(2,Shape_Number);	//数码管 显示形状个数
			delay_ms(1000);
			mark =  25;
			break;
		case 25://小车到达F2，车头左边
			Full_Left(L_sp);
		    Full_Tracking(XJ_sp);
			Full_Go(Go_sp,Go_mp);
			Full_Left(L_sp);
			mark = 30;
			break;
		case 30://开始识别二维码
			MP_Tracking(XJ_sp,60);					//小车循迹E2处，
			delay_ms(500);
			Full_Return_Data(0xC2);					//返回到达二维码标志物处
			delay_ms(500);
			Full_Circulate_Flag = 0;				//等待二维码解析结果
			mark = 35;
			break;
		case 35://寻卡阶段
			delay_ms(1000);
			Full_Find_cardsALL3(4,5);
		    if(RFID_Card_Position==4||RFID_Card_Position==5)
			{
			
			
			
			
			}
			else
			{
				Full_Left(L_sp);
				delay_ms(500);
				Full_Left(L_sp);
				delay_ms(500);
			    Full_Find_cardsALL4(4,3,1500);					//循迹	
				if(RFID_Card_Position==3||RFID_Card_Position==4)
				{
					Full_Left(L_sp);
					delay_ms(500);
					Full_Left(L_sp);
					delay_ms(500); 
					Full_Tracking(XJ_sp);					//循迹			
			        Full_Go(Go_sp,Go_mp);					//前进	D2
				}
				else
				{				
				    Full_Right(R_sp);
					Full_Find_cardsALL4(2,1,1500);					//循迹	
					Full_Left(L_sp);
					delay_ms(500);
					Full_Left(L_sp);
					delay_ms(500); 
					Full_Tracking(XJ_sp);					//循迹			
			        Full_Go(Go_sp,Go_mp);					//前进	D2
					Full_Left(L_sp);
					Full_Tracking(XJ_sp);					//循迹			
			        Full_Go(Go_sp,Go_mp);					//前进	D2
				}
			}
//			Full_Tracking(XJ_sp);					//循迹			
//			Full_Go(Go_sp,Go_mp);					//前进	D2
//		    Full_Left(L_sp);
//			delay_ms(500);
//		    Full_Left(L_sp);
//		    delay_ms(200);
//		    Full_Tracking(XJ_sp);					//循迹			
//			Full_Go(Go_sp,Go_mp);					//前进  F2
//		    Full_Right(R_sp);
//			Full_Tracking(XJ_sp);					//循迹	


//            ////////此处需要优化		
//			Full_Go(Go_sp,Go_mp);					//前进  F4
//		    Full_Left(L_sp);
//		    delay_ms(500);
//		    Full_Left(L_sp);						//到达 F4 车头朝上
//		    Full_Find_cardsALL2(2,3);
//		    Full_Left(L_sp);					//右转
//		    Full_Find_cardsALL2(4,5);
			mark = 40;

			break;
		case 40://到达B2
	        Full_Tracking(XJ_sp);					//循迹	
			Full_Go(Go_sp,Go_mp);					//前进			
			mark = 45;
			break;
		case 45://小车在B2开始调光
			Full_Light_Goal(LIGHT_Grent);  		
			Send_Debug_Info("Stall:",10);
			if(LI_ADR[0]==1)      Send_Debug_Info("1\n",3); 
			else if(LI_ADR[0]==2) Send_Debug_Info("2\n",3); 
			else if(LI_ADR[0]==3) Send_Debug_Info("3\n",3); 
			else if(LI_ADR[0]==4) Send_Debug_Info("4\n",3);
			Full_Left(L_sp); 
			mark = 50;
			break;
		case 50://打开道闸
			Control_Sluice_gate();					//打开道闸
		    Control_Sluice_gate();					//打开道闸
			delay_ms(200);
			mark = 55;
			break;
		case 55://小车到达B4
			Full_Tracking(XJ_sp);					//循迹
			Full_Go(Go_sp,Go_mp);					//前进 F6
			mark = 60;
			break;
		case 60://小车到达C4
			Full_Left(L_sp); 
		    MP_Tracking(XJ_sp,70);
			mark = 65;
			break;
		case 65://操作AGV
			AGV_Control_Infrared(Caution_Text);		//向AGV发送红外报警数据
			AGV_Control_Infrared(Caution_Text);		//向AGV发送红外报警数据
			AGV_Start();//有AVG车的开始位置和停放位置							//开启AGV车全自动
			AGV_Start();
			AGV_Start();
			Full_Circulate_Flag = 0;				//等待AGV车完成任务
			mark = 70;
			break;
		case 70:     //
			Control_Sluice_gate();					//打开道闸
		    Control_Sluice_gate();					//打开道闸
			delay_ms(500);
			Send_AGV_Run_flag(0x81);				//向AGV发送入库命令
			Send_AGV_Run_flag(0x81);				//向AGV发送入库命令
			Send_AGV_Run_flag(0x81);				//向AGV发送入库命令
		    Send_Debug_Info("Send OK\n",5);			//Send
			Full_Circulate_Flag = 0;	
			mark = 75;
			break;
		case 75://AVG过道闸
			Control_Magnetic_suspension();			//磁悬浮
			delay_ms(500);
			Control_Magnetic_suspension();			//磁悬浮
			delay_ms(500);
			mark = 80;
			break;
		case 80://等待打开ETC
			Full_Tracking(XJ_sp);					  //循迹
			Full_Go(Go_sp,14);						  //前进  D4
			ETC_Open_Flag = 1;
			delay_ms(1000);
			Full_Circulate_Flag = 0;		    	  //等待ETC打开	
			mark = 85;
			break;
		case 85://过ETC
			Full_Tracking(XJ_sp);					//循迹
			Full_Go(Go_sp,Go_mp);					//前进 F6
			mark = 90;
			break;
		case 90://小车到达D6点，右转45度
			Full_Right(R_sp);
			Full_Tracking(XJ_sp);					  //循迹
			Full_Go(Go_sp,Go_mp);					//前进 F6					  //前进  D4
		    Full_Right(R_sp);
			Full_Tracking(XJ_sp);					  //循迹
			Full_Go(Go_sp,Go_mp);					//前进 F6				  //前进  D4
		    Full_Right(R_sp);
			delay_ms(1000);
			Full_RL_45(R_sp,2,33);					//右转45度
			mark = 95;
			break;
		case 95://发送红外代码
			Stereo_Display(Car_Text);		   		//立体显示
			delay_ms(1500);
			Stereo_Display(Car_Text);		   		//立体显示
			delay_ms(1500);
			Stereo_Display(Car_Text);		   		//立体显示
			delay_ms(1500);
		    Full_Left(L_sp);
			mark = 100;
			break;
		case 100://小车语音识别，行驶至D4
		  	MP_Tracking(XJ_sp,50);					//小车循迹到C2处，
		
		    SYN_7318_One_test(1,0);					//语音识别
		
		    Full_Tracking(XJ_sp);					  //循迹
			Full_Go(Go_sp,Go_mp);					//前进 F6				  //前进  D4
			mark = 105;
			break;		
		case 105://判断进入车库
			Full_Car_End_Motion();					//主车停车入库
			mark = 110;
			break;
		case 110:
			Control_LED_Nixie_light(2);				//关闭计时
			delay_ms(1000);
			Control_LED_Nixie_light(2);				//关闭计时
			delay_ms(1000);
			mark = 115;
			break;
		case 115:
			BEEP = 0;
			delay_ms(1100);
			BEEP = 1;
			delay_ms(200);
		    LED_RL(120);
			mark = -120;
			break;
		default :
			break;
	}

}


void DEbugNUM(u16 xr)//小于6位数哦
{
	u8 hu[6];
	sprintf((char*)hu,"%d",(char*)xr);	
	Send_Debug_Info((u8 * )hu,strlen(hu));
}

