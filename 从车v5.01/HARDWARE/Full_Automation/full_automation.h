#ifndef __FULL_AUTOMATION_H
#define __FULL_AUTOMATION_H

#include "sys.h"

void Readcard_daivce_Init(void);
void Full_Go(u8 sp,u8 mp);			//ȫ�Զ�ǰ��
void Full_Read_Card(void);			//ȫ�Զ�RFID��д������
void Read_Card(u8 s);               //RFID��д������
void Full_Find_cardsALL1(u8 num1);			//ȫ�Զ�Ѱ������
void Full_Find_cardsALL2(u8 num1,u8 num2);
void Full_Tracking(u8 sp);          //ȫ�Զ�ѭ��
void Full_Motion(void);				//ȫ�Զ�       
void AGV_Return_state(u8 num);  	//����AGV����״̬
void RFID_Data_Handler(u8 *data);	//���ݴ���

void full_Speech(u8 *s_data,u16 lenght);
void Full_Left(u8 sp);
void Full_Right(u8 sp);
void Full_Light_Goal(u8 End_Goal);			//��������
void Stereo_Display(u8 *data);      		//��תLED��ʾ���ƺ�RFID��λ������
void Control_Magnetic_suspension(void); 		//������
void Control_LED_show_data(u8 X,u8 *Data);  //LED�������ʾ����

void MP_Tracking(u8 sp,u8 mp);				//ָ��ѭ������

extern u8 Speech_recognition_data[];
extern u32 mark;					//ȫ�Զ�˳���־λ
extern u8 Full_Circulate_Flag;	    //ȫ�Զ�ѭ����־λ	

extern u8 Car_Position_news[3];		//С��λ��������Ϣ�������
extern u8 Car_Text[6];				//��������
extern u8 Caution_Text[6];			//��������
extern u8 Hw_Data[6];				//��������
extern u8 Shape_Number[10];			//��״����
extern u8 ETC_Open_Flag;			//����ETC�򿨷��ر�־λ
extern u8 LI_ADR[3];
extern u8 Start_AGV_Position;					//����RFID����Ϣ�õ�AGV��ʼλ������
extern u8 End_AGV_Position;
void TFT_turn_over(u8 num);			//TFT��ҳ
void LED_RL(u8 timer);
u8 Look_RFID(u8 s);

void DEbugNUM(u16 xr);//Debug ��ʾ���ֲ��ó�����λ

void AGV_Tracking(u8 sp,u16 timer);//AVG���Զ���ʼ
void AVG_Start_Station(u8 station);//AVG����ͷ����

void AGV_return_Flag(u8 num);		//avg������ָ��
void AGV_Infrared_Alarm(u8 *data);	//AVG���ͺ���
void AVG_Car_End_Motion(u8 station);//AVGͣ���ĳ���


#endif


