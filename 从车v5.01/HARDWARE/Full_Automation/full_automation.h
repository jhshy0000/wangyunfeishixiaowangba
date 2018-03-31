#ifndef __FULL_AUTOMATION_H
#define __FULL_AUTOMATION_H

#include "sys.h"

void Readcard_daivce_Init(void);
void Full_Go(u8 sp,u8 mp);			//全自动前进
void Full_Read_Card(void);			//全自动RFID读写卡操作
void Read_Card(u8 s);               //RFID读写卡操作
void Full_Find_cardsALL1(u8 num1);			//全自动寻卡操作
void Full_Find_cardsALL2(u8 num1,u8 num2);
void Full_Tracking(u8 sp);          //全自动循迹
void Full_Motion(void);				//全自动       
void AGV_Return_state(u8 num);  	//控制AGV返回状态
void RFID_Data_Handler(u8 *data);	//数据处理

void full_Speech(u8 *s_data,u16 lenght);
void Full_Left(u8 sp);
void Full_Right(u8 sp);
void Full_Light_Goal(u8 End_Goal);			//调光排序
void Stereo_Display(u8 *data);      		//旋转LED显示车牌和RFID卡位置坐标
void Control_Magnetic_suspension(void); 		//磁悬浮
void Control_LED_show_data(u8 X,u8 *Data);  //LED数码管显示数据

void MP_Tracking(u8 sp,u8 mp);				//指定循迹距离

extern u8 Speech_recognition_data[];
extern u32 mark;					//全自动顺序标志位
extern u8 Full_Circulate_Flag;	    //全自动循环标志位	

extern u8 Car_Position_news[3];		//小车位置坐标信息存放数组
extern u8 Car_Text[6];				//车牌数据
extern u8 Caution_Text[6];			//报警数据
extern u8 Hw_Data[6];				//红外数据
extern u8 Shape_Number[10];			//形状个数
extern u8 ETC_Open_Flag;			//允许ETC打卡返回标志位
extern u8 LI_ADR[3];
extern u8 Start_AGV_Position;					//根据RFID内信息得到AGV起始位置坐标
extern u8 End_AGV_Position;
void TFT_turn_over(u8 num);			//TFT翻页
void LED_RL(u8 timer);
u8 Look_RFID(u8 s);

void DEbugNUM(u16 xr);//Debug 显示数字不得超过两位

void AGV_Tracking(u8 sp,u16 timer);//AVG车自动开始
void AVG_Start_Station(u8 station);//AVG车车头朝向

void AGV_return_Flag(u8 num);		//avg车返回指令
void AGV_Infrared_Alarm(u8 *data);	//AVG发送红外
void AVG_Car_End_Motion(u8 station);//AVG停靠的车库


#endif


