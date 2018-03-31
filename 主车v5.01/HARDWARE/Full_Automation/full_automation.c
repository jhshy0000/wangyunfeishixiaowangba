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
ʮ������    ����      ����
  0x22       "       ˫����
  0x27		  ����    �յ�����
  0x60      ��        ��������
  0x72       r       Сдr
  0x2F       /       б��
  0x5C       \       ��б��
  0x2C       ,       ����
  0x2E       .       ���
  0x2D       -       ����
  0x3C       <       С��
  0x3E       >       ����
  0x28		 (       ������
  0x29       )       ������
  0x41       A       ��дA
  0x42       B       ��дB
  0x43       C       ��дC
  0x44       D       ��дD
  0x45       E       ��дE
  0x46       F       ��дF
  0x47       G       ��дG
  0x30       0       ����0
  0x31       1       ����1
  0x32       2       ����2
  0x33       3       ����3
  0x34       4       ����4
  0x35       5       ����5
  0x36       6       ����6
  0x37       7       ����7
  0x38       8       ����8
  0x39       9       ����9
*/

/****************************************************************************************
*****************************************************************************************
		ȫ�Զ������޸�
*****************************************************************************************
****************************************************************************************/
//��������
#define Go_sp    70ommit your changes or stash them before you merge.
Aborting
fatal: revert failed			//ǰ���ٶ�
shashfjh
#define Back_sp  70			//�����ٶ�
#define L_sp     90		  //��ת�ٶ�
#define R_sp	 90			//��ת�ٶ�
#define XJ_sp    50		//ѭ���ٶ�

#define Go_mp    21		    //ǰ������
#define Back_mp  100		//��������

u8 Car_Text[6];			    //��������
u8 Caution_Text[6];		    //��������
u8 Hw_Data[6];			    //��������

u8 RX_AGV_Data[] = {0x55,0x02,0x00,0x00,0x00,0x00,0x00,0xBB};// �ӳ����ƿ���ָ������

u8 RXRFID[16];												 //���RFID����
u8 LI_ADR[3];//
//u8 Read_CardOK=0;//0��û�ж�����1�������ɹ�
u8 RFID_ADRSS;//RFID���ַ
u8 LIGHT_Grent;//���յȼ�

u8 TXRFID[16] = {0x28,0x22,0x44,0x32,0x2D,0x3E,0x22,0x2C,0x22,0x44,0x32,0x2D,0x3E,0x45,0x22,0x29};//�ڶ���


/*
        D6
		3
		^
 F4  4<   >1  B4
	    V
		2
		D2
*/
u8 Start_AGV_Position = 1;					             //����RFID����Ϣ�õ�AGV��ʼλ������

/*1����A����  2����B���� 3����C���� 4����D���� 5����E���� 6����F����*/
/*���� 1����A���� ��5����E���� �Ѿ���ռ��*/
u8 End_AGV_Position = 1;					//����RFID����Ϣ�õ�AGV����λ������
/* 
   1�������(D2)	2�����������1/4(E2)   3�����е�(F2)   4���������յ�1/4(F3)   5�����յ�(F4)
*/
u8 RFID_Card_Position = 0;								//RFID��Ƭλ�ñ�־λ  Ĭ��λ��Ϊ5

u32 mark = 0;											//ȫ�Զ�˳���־λ

u8 Full_Circulate_Flag = 0;								//ȫ�Զ�ѭ����־λ


//                      A a  B b  C c  D d  E e
u8 Shape_Number[10] = {10,0,11,0,12,0,13,0,14,0};	 	//��״����

//Car_Position_news[0]  								//Ϊ������ʼ����λ��  B1 ��A2��A4
// B1-------1
// A2-------2
// A4-------3
//Car_Position_news[1]  								//�����������λ��      B��D��F
// D-------4
// E-------5
// F-------6
//Car_Position_news[2]									//δʹ��

u8 Car_Position_news[3] = {0x00,0x00,0x00};				//С��λ��������Ϣ�������

u8 ETC_Open_Flag = 0;									//����ETC�򿨷��ر�־λ

//u8 Speech_recognition_data[] = {0xAF,0x06,0x00,0x02,0x00,0x00,0x01,0xBF}; //����ʶ����ȷ��־����

/*****************************************************************************************
	�����޸�END
*****************************************************************************************/	                                            //1����D2��2����E2��3����F2��4����F3��5����F4	

/****************************************************************
�������ܣ������豸��ʼ��
��	  ����sp �ٶ�  mp ����
����  ֵ����
*****************************************************************/
void Readcard_daivce_Init()
{
	uart1_init(9600);	// ���ڳ�ʼ��Ϊ9600
	delay_ms(100);
	InitRc522();		  //��������ʼ��
	delay_ms(1000);
	uart1_init(115200);
	delay_ms(100);
}
/****************************************************************
�������ܣ��뵥λ����ʱ
��	  ����T = 1 �� 1��
����  ֵ����
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
�������ܣ�ȫ�Զ�ֹͣ
��	  ����sp �ٶ�  mp ����
����  ֵ����
*****************************************************************/
void Full_STOP()
{
	Track_Flag=0;MP=0;
	Stop_Flag=0;G_Flag=0;B_Flag=0;L_Flag=0;R_Flag=0; 
	STOP();
}
/****************************************************************
�������ܣ�ȫ�Զ�ǰ��
��	  ����sp �ٶ�  mp ����
����  ֵ����
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
�������ܣ�ǰ��
��	  ����sp �ٶ�  mp ����
����  ֵ����
*****************************************************************/
void Go(u8 sp,u16 mp)
{
	MP=0;G_Flag=1;	Stop_Flag=0;tempMP=0;			 					
	tempMP = mp;
	Car_Spend = sp;
	Control(Car_Spend,Car_Spend);
}
/****************************************************************
�������ܣ�ȫ�Զ�����
��	  ����sp �ٶ�  mp����
����  ֵ����
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
�������ܣ�ȫ�Զ���ת
��	  ����sp �ٶ�
����  ֵ����
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
�������ܣ���ת
��	  ����sp �ٶ�
����  ֵ����
*****************************************************************/
void Left(u8 sp)
{
	MP=0;L_Flag=1;	Stop_Flag=0;
	Car_Spend = sp;
	Host_Open_UpTrack( Up_Track_Speed );
	Control(-Car_Spend,Car_Spend);
}
/****************************************************************
�������ܣ�ȫ�Զ���ת
��	  ����sp �ٶ�
����  ֵ����
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
��������: ��ת
��	  ����sp �ٶ�
����  ֵ����
*****************************************************************/
void Right(u8 sp)
{
	MP=0;R_Flag=1;	Stop_Flag=0;
	Car_Spend = sp;
	Host_Open_UpTrack( Up_Track_Speed );
	Control(Car_Spend,-Car_Spend);
}
/****************************************************************
��������: ȫ�Զ�45��ת
��	  ����sp �ٶ� RL = 1 ��ת RL = 2 ��ת timer �����Ƕ�  ����ֵ timer = 28 ��45��
����  ֵ����
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
�������ܣ�ȫ�Զ�ѭ��
��	  ����sp �ٶ�
����  ֵ����
*****************************************************************/

void Full_Tracking(u8 sp)
{
	
	Car_Spend = sp;     //�涨��ת��
	Track_Flag=1;       //����ѭ��
	MP=0; 				//����ֵ��0	  
	Host_Open_UpTrack( Up_Track_Speed );  // ����Ѱ�������ϴ�
	delay_ms( 100); 					  // �ȴ�һ��Ѱ�������ϴ�
	TIM_Cmd(TIM2, ENABLE);

	while(Stop_Flag != 0x01);
}
/****************************************************************
�������ܣ���ȫ�Զ�ѭ��
��	  ����sp �ٶ�
����  ֵ����
*****************************************************************/
void Tracking(u8 sp)
{
	Car_Spend = sp;
	Track_Flag=1;MP=0; 					  
	Host_Open_UpTrack( Up_Track_Speed );  // ����Ѱ�������ϴ�
	delay_ms( 100); 					  // �ȴ�һ��Ѱ�������ϴ�
	TIM_Cmd(TIM2, ENABLE);
}
/****************************************************************
�������ܣ�ָ������ѭ������
��	  ����sp ѭ���ٶ�  mp ѭ������ 
����  ֵ����
*****************************************************************/
void MP_Tracking(u8 sp,u8 mp)
{
	Car_Spend = sp;
	Track_Flag=1;MP=0; 					  
	Host_Open_UpTrack( Up_Track_Speed );  // ����Ѱ�������ϴ�
	delay_ms( 100); 					  // �ȴ�һ��Ѱ�������ϴ�
	TIM_Cmd(TIM2, ENABLE);
	while(MP<mp);
	Full_STOP();
}
/****************************************************************
�������ܣ�LED����ܿ���
��	  ����Mode_num=1 �򿪼�ʱ  Mode_num=2 �رռ�ʱ
����  ֵ����
*****************************************************************/
void Control_LED_Nixie_light(u8 Mode_num)
{
	if(Mode_num == 1){
		send_data_zigbee(SMG_JSK,8);		//�򿪼�ʱ
	} else if(Mode_num == 2) {
		send_data_zigbee(SMG_JSG,8);		//�رռ�ʱ
	}
}
/****************************************************************
�������ܣ�LED�������ʾ����
��	  ����*HW ����ָ���ַ
����  ֵ���� 
*****************************************************************/
void Control_LED_show_data(u8 X,u8 *Data)
{
  u8 LED_Data[8] = {0x55,0x04,0x00,0x00,0x00,0x00,0x00,0xBB};
  if(X == 1)					//��һ��
  {
	  LED_Data[2] = 0x01;  
  } else if( X == 2 ) {	 		//�ڶ��� 
	  LED_Data[2] = 0x02;  
  }
  LED_Data[3] = (u8)((Data[0]&0x0F) << 4)|(Data[1]&0x0F);
  LED_Data[4] = (u8)((Data[2]&0x0F) << 4)|(Data[3]&0x0F);
  LED_Data[5] = (u8)((Data[4]&0x0F) << 4)|(Data[5]&0x0F);
  LED_Data[6] = (u8)(LED_Data[2] + LED_Data[3] +LED_Data[4] + LED_Data[5])%256;
  send_data_zigbee(LED_Data,8);
}
/****************************************************************
�������ܣ��������
��	  ����*HW ����ָ���ַ
����  ֵ����
*****************************************************************/
void Control_Infrared_alarm(u8 *HW)
{
	Transmition(HW,6);					//��������	
}
/****************************************************************
�������ܣ�������ʾ����
��	  ����*data ��������ָ��
����  ֵ����
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
	Send_Debug_Info(Car_data,6); // �ϴ�������Ϣ
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
	Send_Debug_Info(Car_data,6); // �ϴ�������Ϣ
	Send_Debug_Info("\n",2); // �ϴ�������Ϣ
}
/****************************************************************
�������ܣ���բ����
��	  ������
����  ֵ����
*****************************************************************/
void Control_Sluice_gate()
{
	send_data_zigbee(DZ_K,8);
}
/****************************************************************
�������ܣ�����������
��	  ������
����  ֵ����
*****************************************************************/
void Control_Magnetic_suspension(void)
{
	send_data_zigbee(Ms_Data,8);
}
/****************************************************************
�������ܣ�TFT��ҳ����
��	  ������
����  ֵ����
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
�������ܣ���������
��	  ����End_Goal Ŀ�굵λ 1~4
����  ֵ����
*****************************************************************/
void Full_Light_Goal(u8 End_Goal)
{
	u16 Light_num[4];			//��ǿ����ֵ�������
	u16 Start_Light = 0;		//��ʼ��ǿ��ֵ
	u8  Light_Text[] = "0000";	//��ʾ��ǿ������
	int i=0, j=0;
	u16 temp = 0;
	
	if((End_Goal <= 8) && (End_Goal>4))		//��Ŀ�굵λ���ݴ���
	{
		End_Goal = End_Goal - 4;
	}
	if((End_Goal <= 16) && (End_Goal>8))	
	{
		End_Goal = End_Goal - 8;
	}
	
	Transmition(H_1,4);
	delay_ms(3000);
	Light_num[0] = Dispose();	//�õ��ĸ���λ�Ĺ�ǿ��ֵ
	
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
�������ܣ�ȫ�Զ���������
��	  ����*s_data ����ָ�� length  ���ݳ���
����  ֵ����
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
�������ܣ�ȫ�Զ�������������
��	  ����Data = 0XC1 ����TFT��־�ﴦ
		  Data = 0xC2 ���ﾲ̬��־��
����  ֵ����
*****************************************************************/
void Full_Return_Data(u8 Data)
{
	S_Tab[2] = Data;
	send_data_wifi(S_Tab , 10);
}
/****************************************************************
�������ܣ�AGV����ָ��У��
��	  ������
����  ֵ����
*****************************************************************/
void AGV_Send_Data()
{
	RX_AGV_Data[6] = (RX_AGV_Data[2] + RX_AGV_Data[3] 
					+ RX_AGV_Data[4] + RX_AGV_Data[5])%256;		//�õ�У���;
	
	send_data_zigbee(RX_AGV_Data,8);							//��������
}
/****************************************************************
�������ܣ�AGV���ͺ�������
��	  ����*Infrared_Data  AGV�������ݵ�ַ
����  ֵ����
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
�������ܣ�AGV���ͺ�������
��	  ����*Infrared_Data  AGV�������ݵ�ַ����  ֵ����
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
�������ܣ�������AGV��������״̬
��	  ���nunm  ����״̬ ����ο�ͨѶЭ��
����  ֵ����
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
�������ܣ�RFID���ݴ���
��	  ���d*data RFID ����ָ��
����  ֵ����
*****************************************************************/
void RFID_Data_Handler(u8 *data)//�˴���16������
{	
	u8 show_analyze_data[7];	
	u8 i = 0,j = 0;
	
	for(i=0;i<10;i++)//������Ҫ����6�����ݴ���
	{
		if(data[i] == 0x23)							//�ж� #
		{
		 if((data[i+3] == 0x2B) && (data[i+4] == 0x3F) && (data[i+6] == 0x23))  //�ж� +?#		
		 {
			 for(j=0;j<7;j++)
			 {			 
				show_analyze_data[j] = data[j+i];	//����Ǩ��					
			 }
			 Send_Debug_Info(show_analyze_data,7); 	//���������RFID������ʾ
			 
			switch(data[i+1])					    //AGV����ʼλ�÷���
			{
				case 0x42: //ASCII "B"			    //B4   ��ת
					   // if(data[i+2] == 0x34)
						Start_AGV_Position = 1;
					break;
				case 0x44: //ASCII "D"
					if(data[i+2] == 0x32)	   	//D2    �޶���       //ASCII "2"	
					{
						Start_AGV_Position = 2;							
					} 
					else if(data[i+2] == 0x36)  //D6   ������ת/��ת	 //ASCII "6"
					{
						Start_AGV_Position = 3;
					}
					break;
				case 0x46: //ASCII "F"	
						//if(data[i+2] == 0x34)					//F4	��ת
						Start_AGV_Position = 4;
					break;		
			}

			switch(data[i+5])					//AGV������λ��  //
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
�������ܣ�Ѱ��RFID
��	  ����
����  ֵ����
*****************************************************************/
u8 Look_RFID(u8 s)
{
		char status = 0;
		u8 CT[2];									//������
		u8 SN[4]; 									//����
		u8 KEY[6]={0xff,0xff,0xff,0xff,0xff,0xff};  //��Կ
		//u8 s = 0x2c;       							// #define   Status2Reg   0x08	
		u8 op=(s/4)*4+3	;
		//u8 i;
		//u8  j = 0;
		status = PcdRequest(PICC_REQALL,CT);		//Ѱ��
		Send_Debug_Info("asdf0",5);
		if(status == MI_OK)							//Ѱ���ɹ�
		{
			status=MI_ERR;
			CT[0] = CT[0] + 0x30;
			CT[1] = CT[1] + 0x30;
			Send_Debug_Info(CT,2);					//������
			Send_Debug_Info("\n",2);
			Send_Debug_Info("Findcard",8);
			Send_Debug_Info(" OK\n",5);
		
			status = PcdAnticoll(SN);				//����ײ
			if(status == MI_OK)
			{
				status=MI_ERR;
				Send_Debug_Info("anti",4);Send_Debug_Info("-collisi",8);
				Send_Debug_Info(" OK\n",5);
				
				status =PcdSelect(SN);				//ѡ���˿�
				if(status == MI_OK)					  //ѡ���ɹ�
				{
					status=MI_ERR;
					Send_Debug_Info("Pick a",6);Send_Debug_Info(" card",5);
				  Send_Debug_Info(" OK\n",5);
					
					status =PcdAuthState(0x60,op,KEY,SN);		//��֤��Կ
					if(status == MI_OK)
					{
						status = MI_ERR;
						Send_Debug_Info("test",4);
						Send_Debug_Info(" and",4);
						Send_Debug_Info(" verify",7);
						Send_Debug_Info(" OK\n",5);							
						
						status=PcdRead(s,RXRFID);				//����
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
�������ܣ�RFID��д��
��	  ����
����  ֵ����
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
			///��RFID���ڶ�������Ϣ���͵�Debug��ʾ

			Send_Debug_Info(RXRFID,8);
			Send_Debug_Info(RXRFIDH,8);
			Send_Debug_Info("\n",2);
			//����������Ϣ����
			RFID_Data_Handler(RXRFID);							
}

void Full_Read_Card()
{
			char status = 0;
			//u8 CT[2];									//������
			u8 SN[4]; 									//����
			u8 KEY[6]={0xff,0xff,0xff,0xff,0xff,0xff};  //��Կ
			u8 s = 0x01;       							// #define   Status2Reg   0x08	
			u8 RXRFIDH[8];
			
			u8 i = 0; 
			//u8 j = 0;

//		status = PcdRequest(PICC_REQALL,CT);		//Ѱ��
//		if(status == MI_OK)							//Ѱ���ɹ�
//		{
			status=MI_ERR;
//			CT[0] = CT[0] + 0x30;
//			CT[1] = CT[1] + 0x30;
//			Send_Debug_Info(CT,2);					//������
//			Send_Debug_Info("\n",2);
//			Send_Debug_Info("Findcard",8);
//			Send_Debug_Info(" OK\n",5);
		
			status = PcdAnticoll(SN);				//����ײ
			if(status == MI_OK)
			{
				status=MI_ERR;
				Send_Debug_Info("anti",4);Send_Debug_Info("-collision",10);
				Send_Debug_Info(" OK\n",5);
				
				status =PcdSelect(SN);				//ѡ���˿�
				if(status == MI_OK)					//ѡ���ɹ�
				{
					status=MI_ERR;
					Send_Debug_Info("Pick a",6);Send_Debug_Info(" card",5);
				    Send_Debug_Info(" OK\n",5);
					
					status =PcdAuthState(0x60,0x03,KEY,SN);		//��֤��Կ
					if(status == MI_OK)
					{
						status = MI_ERR;
						Send_Debug_Info("test",4);
						Send_Debug_Info(" and",4);
						Send_Debug_Info(" verify",7);
						Send_Debug_Info(" OK\n",5);
													
//						status = PcdWrite(s,TXRFID);			//д��
//						if(status == MI_OK)
//						{
//							status = MI_ERR;
//							Send_Debug_Info("write OK",10);
//							Send_Debug_Info("\n",2);	
//						}	
//						delay_ms(500);
						
						status=PcdRead(s,RXRFID);				//����
						if(status == MI_OK)
						{
							status = MI_ERR;
							BEEP = 0;
							Send_Debug_Info("read OK\n",8);
							for(i=0;i<8;i++)
							{
								RXRFIDH[i] = RXRFID[i+8];
							}							
							///��ʾ��
				
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
�������ܣ�ȫ�Զ�Ѱ��1ģʽ
��	  ������
����  ֵ����
*****************************************************************/
void Full_Find_cardsALL1(u8 num1)
{
	char status = 0;
	u8 RFID_TASK_Flag = 1;
	u8 CT[2];									    //������
	
	Go(Go_sp-5,Go_mp);
	while(RFID_TASK_Flag)
	{
		status = PcdRequest(PICC_REQALL,CT);	    //Ѱ��
		if(status == MI_OK)							//Ѱ���ɹ�
		{
			delay_ms(150);
			STOP();
			RFID_Card_Position = num1;					//RFID λ������Ϊ 1����F2
			Send_Debug_Info("(1)\n",5);
			delay_ms(1000);
//			Full_Read_Card();						//��������
			Read_Card(RFID_ADRSS);	    //��д��
			delay_ms(1000);
			BEEP = 1;
			Full_Go(Go_sp,5);				//�������ǰ��
			RFID_TASK_Flag = 0;
		} 
		else if(Stop_Flag == 0x03)					//ǰ�����
		{
			 Send_Debug_Info("RFID\n",8); // �ϴ�������Ϣ
			 RFID_TASK_Flag = 0;
		}
	}
	Send_Debug_Info("CHU\n",5); // �ϴ�������Ϣ
	RFID_Data_Handler(RXRFID);		//��RFID�����ݽ��д���
	Send_Debug_Info("LI\n",4); // �ϴ�������Ϣ
}
/****************************************************************
�������ܣ�ȫ�Զ�Ѱ��
��	   ������
����  ֵ����2,3
*****************************************************************/
void Full_Find_cardsALL2(u8 num1,u8 num2)
{
	char status = 0;
	u8 RFID_TASK_Flag = 1;
	u8 CT[2];									    //������
			
	Tracking(XJ_sp-5);								//ѭ��
	delay_ms(500);									//�ȴ�һ��ʱ���ٿ���Ѱ��
	RFID_TASK_Flag = 1;
	
	while(RFID_TASK_Flag)
	{
		status = PcdRequest(PICC_REQALL,CT);	    //Ѱ��
		if(status == MI_OK)						    //Ѱ���ɹ�
		{
			delay_ms(150);
			STOP();
			RFID_Card_Position = num1;					//RFID λ������Ϊ 2����C2
			
			Send_Debug_Info("(2)\n",5);
			
			delay_ms(1000);
			
			 Read_Card(RFID_ADRSS);	    //��д��
			//Full_Read_Card();						//��������
			delay_ms(1000);
			BEEP = 1;
			Full_Tracking(XJ_sp-5);					//����ѭ��
			RFID_TASK_Flag = 0;
		} 
		else if(Stop_Flag == 0x01)					//ѭ�����
		{			
			RFID_TASK_Flag = 0;
		}
	}
	
	Go(Go_sp-5,Go_mp);
	RFID_TASK_Flag = 1;
	while(RFID_TASK_Flag)
	{
		status = PcdRequest(PICC_REQALL,CT);	    //Ѱ��
		if(status == MI_OK)							//Ѱ���ɹ�
		{
			delay_ms(150);
			STOP();
			RFID_Card_Position = num2;					//RFID λ������Ϊ 3����D2
			Send_Debug_Info("(3)\n",5);
			delay_ms(1000);
			
			Read_Card(RFID_ADRSS);	    //��д��
			//Full_Read_Card();						   //��������
			delay_ms(1000);
			BEEP = 1;
			Full_Go(Go_sp-5,5);						//�������ǰ��
			RFID_TASK_Flag = 0;
		} 
		else if(Stop_Flag == 0x03)					//ǰ�����
		{
			RFID_TASK_Flag = 0;
		}
	}	
	RFID_Data_Handler(RXRFID);		//��RFID�����ݽ��д���
}

/****************************************************************
�������ܣ�����ʱ��Ѱ��
��	   ������
����  ֵ����
*****************************************************************/
void Full_Find_cardsALL3(u8 num1,u8 num2)
{
	char status = 0;
	u8 RFID_TASK_Flag = 1;
	u8 CT[2];									    //������
			
	Tracking(XJ_sp-5);								//ѭ��
	RFID_TASK_Flag = 1;
	
	while(RFID_TASK_Flag)
	{
		status = PcdRequest(PICC_REQALL,CT);	    //Ѱ��
		if(status == MI_OK)						    //Ѱ���ɹ�
		{
			delay_ms(150);
			STOP();
			RFID_Card_Position = num1;					//RFID λ������Ϊ 2����C2
			
			Send_Debug_Info("(2)\n",5);
			
			delay_ms(1000);
			
			 Read_Card(RFID_ADRSS);	    //��д��
			//Full_Read_Card();						//��������
			delay_ms(1000);
			BEEP = 1;
			Full_Tracking(XJ_sp-5);					//����ѭ��
			RFID_TASK_Flag = 0;
		} 
		else if(Stop_Flag == 0x01)					//ѭ�����
		{			
			RFID_TASK_Flag = 0;
		}
	}
	
	Go(Go_sp-5,Go_mp);
	RFID_TASK_Flag = 1;
	while(RFID_TASK_Flag)
	{
		status = PcdRequest(PICC_REQALL,CT);	    //Ѱ��
		if(status == MI_OK)							//Ѱ���ɹ�
		{
			delay_ms(150);
			STOP();
			RFID_Card_Position = num2;					//RFID λ������Ϊ 3����D2
			Send_Debug_Info("(3)\n",5);
			delay_ms(1000);
			
			Read_Card(RFID_ADRSS);	    //��д��
			//Full_Read_Card();						   //��������
			delay_ms(1000);
			BEEP = 1;
			Full_Go(Go_sp-5,5);						//�������ǰ��
			RFID_TASK_Flag = 0;
		} 
		else if(Stop_Flag == 0x03)					//ǰ�����
		{
			RFID_TASK_Flag = 0;
		}
	}	
	RFID_Data_Handler(RXRFID);		//��RFID�����ݽ��д���
}


void Full_Find_cardsALL4(u8 num1,u8 num2,u16 timer_del)//����Ѱ��
{

	char status = 0;
	u8 RFID_TASK_Flag = 1;
	u8 CT[2];									    //������
			
	Tracking(XJ_sp-5);								//ѭ��
	delay_ms(timer_del);									//�ȴ�һ��ʱ���ٿ���Ѱ��
	RFID_TASK_Flag = 1;
	
	while(RFID_TASK_Flag)
	{
		status = PcdRequest(PICC_REQALL,CT);	    //Ѱ��
		if(status == MI_OK)						    //Ѱ���ɹ�
		{
			delay_ms(150);
			STOP();
			RFID_Card_Position = num1;					//RFID λ������Ϊ 2����C2
			
			Send_Debug_Info("(2)\n",5);
			
			delay_ms(1000);
			
			 Read_Card(RFID_ADRSS);	    //��д��
			//Full_Read_Card();						//��������
			delay_ms(1000);
			BEEP = 1;
			Full_Tracking(XJ_sp-5);					//����ѭ��
			RFID_TASK_Flag = 0;
		} 
		else if(Stop_Flag == 0x01)					//ѭ�����
		{			
			RFID_TASK_Flag = 0;
		}
	}
	
	Go(Go_sp-5,Go_mp);
	RFID_TASK_Flag = 1;
	while(RFID_TASK_Flag)
	{
		status = PcdRequest(PICC_REQALL,CT);	    //Ѱ��
		if(status == MI_OK)							//Ѱ���ɹ�
		{
			delay_ms(150);
			STOP();
			RFID_Card_Position = num2;					//RFID λ������Ϊ 3����D2
			Send_Debug_Info("(3)\n",5);
			delay_ms(1000);
			
			Read_Card(RFID_ADRSS);	    //��д��
			//Full_Read_Card();						   //��������
			delay_ms(1000);
			BEEP = 1;
			Full_Go(Go_sp-5,5);						//�������ǰ��
			RFID_TASK_Flag = 0;
		} 
		else if(Stop_Flag == 0x03)					//ǰ�����
		{
			RFID_TASK_Flag = 0;
		}
	}	
	RFID_Data_Handler(RXRFID);		//��RFID�����ݽ��д���
}

/****************************************************************
�������ܣ�ȫ�Զ��������
��	  ������
// D-------4
// E-------5
// F-------6
����  ֵ����
*****************************************************************/
void Full_Car_End_Motion()
{
	if(Car_Position_news[1] == 1)	//�����������A����	
	{
		if(End_AGV_Position == 1)   //���AGV���Ѿ�����A����
		{
			Full_Tracking(XJ_sp);	//��������B����		
			Full_Go(Go_sp,Go_mp);
			Full_Tracking(XJ_sp-10);
			Full_Left(L_sp);
			delay_ms(500);
			Full_Tracking(XJ_sp-10);
			Full_Back(Back_sp,Back_mp-60);	
		}
		else 
		{						//��������A����
			Full_Tracking(XJ_sp);		
			Full_Go(Go_sp,Go_mp);			
			Full_Left(L_sp);
			Full_Tracking(XJ_sp);		
			Full_Go(Go_sp,Go_mp);
			Full_Right(R_sp);				//��ת
			Full_Tracking(XJ_sp-10);
			Full_Left(L_sp);
			delay_ms(500);
			Full_Tracking(XJ_sp-10);
			Full_Back(Back_sp,Back_mp-60);	
		}
	} 
	else if(Car_Position_news[1] == 2)//�����������B����	
	{//�����������E����
		if(End_AGV_Position == 2)		 //���AGV����B����,��������C����
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
		{//  ��������	B����
			
			Full_Tracking(XJ_sp);	//��������B����		
			Full_Go(Go_sp,Go_mp);
			Full_Tracking(XJ_sp-10);
			Full_Left(L_sp);
			delay_ms(500);
			Full_Tracking(XJ_sp-10);
			Full_Back(Back_sp,Back_mp-60);
		}
	} 
	else if(Car_Position_news[1] == 3) //�����������C����
	{	
		if(End_AGV_Position == 3)			//���AGV����C���� //�������� D����
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
		else //�������� C����
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
	else if(Car_Position_news[1] == 4) //�����������D����
	{	
		if(End_AGV_Position == 4)			//���AGV����D���� //�������� E����
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
		else //�������� D����
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
	else if(Car_Position_news[1] == 5) //�����������E����
	{	
		if(End_AGV_Position == 5)			//���AGV����E���� //�������� F����
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
		else //�������� E����
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
	else if(Car_Position_news[1] == 6) //�����������F����
	{	
		if(End_AGV_Position == 6)			//���AGV����F���� //�������� A����
		{
			Full_Tracking(XJ_sp);		
			Full_Go(Go_sp,Go_mp);			
			Full_Left(L_sp);
			Full_Tracking(XJ_sp);		
			Full_Go(Go_sp,Go_mp);
			Full_Right(R_sp);				//��ת
			Full_Tracking(XJ_sp-10);
			Full_Left(L_sp);
			delay_ms(500);
			Full_Tracking(XJ_sp-10);
			Full_Back(Back_sp,Back_mp-60);				
		} 
		else //�������� F����
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
�������ܣ�������ʼλ���ж���Ӧ����
��	  ������
// B1-------1
// A2-------2
// A4-------3
����  ֵ����
*****************************************************************/
 void Full_Car_Strat_Position_HandlerMsg1()
{
	if(Car_Position_news[0] == 1)				//B1
	{
		Full_Tracking(XJ_sp);  //50
		//Full_Find_cards1();						//����Ѱ��ģʽ1  ǰ��Ѱ��
		Full_Left(L_sp);						//��ת
		//Full_Find_cards2_3();					//����Ѱ��ģʽ 2_3
		Full_Left(L_sp);						//��ת
		delay_ms(500);							//��ʱ
		Full_Left(L_sp);						//��ת
		Full_Tracking(XJ_sp);					//ѭ��
		Full_Go(Go_sp,Go_mp);					//ǰ��	
		
	} else if(Car_Position_news[0] == 2) 
	{		//A2
		
		
		Full_Tracking(XJ_sp);
		
		//Full_Find_cards1();						//����Ѱ��ģʽ1  ǰ��Ѱ��
		Full_RL_45(L_sp,1,33);					//��ת45��
	
	} else if(Car_Position_news[0] == 3) {		//A4
		
		Full_Tracking(XJ_sp);
		Full_Go(Go_sp,Go_mp);
		Full_Left(L_sp);						//��ת
		Full_Tracking(XJ_sp);
		//Full_Find_cards1();						//����Ѱ��ģʽ1  ǰ��Ѱ��
		Full_Right(R_sp);						//��ת
		//Full_Find_cards2_3();					//����Ѱ��ģʽ 2_3
		Full_Left(L_sp);						//��ת
		delay_ms(500);							//��ʱ
		Full_Left(L_sp);						//��ת
		Full_Tracking(XJ_sp);					//ѭ��
		Full_Go(Go_sp,Go_mp);					//ǰ��
	}
}

/****************************************************************
�������ܣ�������ʼλ���ж���Ӧ����
��	  ������
// B1-------1
// A2-------2
// A4-------3
����  ֵ����
*****************************************************************/
void Full_Car_Strat_Position_HandlerMsg2()
{
	if(Car_Position_news[0] == 1 || Car_Position_news[0] == 3)				//B1 �� A4
	{
		Full_Left(L_sp);						//��ת
		delay_ms(500);							//��ʱ
		Full_Left(L_sp);						//��ת
		
	} else if(Car_Position_news[0] == 2) {		//A2
		
		Full_Right(R_sp);						//��ת
		//Full_Find_cards2_3();					//����Ѱ��ģʽ 2_3
		Full_Left(L_sp);						//��ת
		delay_ms(500);							//��ʱ
		Full_Left(L_sp);						//��ת
		Full_Tracking(XJ_sp);					//ѭ��
		Full_Go(Go_sp,Go_mp);					//ǰ��
		Full_Left(L_sp);						//��ת
		delay_ms(500);							//��ʱ
		Full_Left(L_sp);						//��ת	
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
�������ܣ�ȫ�Զ���ʼ����
��	  ������
����  ֵ����
*****************************************************************/
void Full_Motion()
{
	switch(mark)
	{
		case 5://����ʱLED��ʼ��Ϣ
			Control_LED_Nixie_light(1);				//�򿪼�ʱ 55
			delay_ms(500);
			mark = 10;
			break;
		case 10://С������F4����ת45��
		
		    Full_Tracking(XJ_sp);
		    Full_Go(Go_sp,Go_mp);
			Full_Right(R_sp);
		    Full_Tracking(XJ_sp);
		    Full_Go(Go_sp,Go_mp);
			Full_RL_45(R_sp,2,23);					//��ת45��
			mark = 15;
			break;//
		case 15://С����ƽ�巢���ѵ�F4��
			Full_Return_Data(0xC1);					   //����TFT��־��λ��
			Full_Circulate_Flag = 0;				   //�ȴ� ��ɫ��״������ʶ����
			mark = 20;
			break;
		case 20://��ʼʶ���ƺź���״��ɫ
			Control_LED_show_data(2,Shape_Number);	//����� ��ʾ��״����
			delay_ms(1000);
			Control_LED_show_data(2,Shape_Number);	//����� ��ʾ��״����
			delay_ms(1000);
			mark =  25;
			break;
		case 25://С������F2����ͷ���
			Full_Left(L_sp);
		    Full_Tracking(XJ_sp);
			Full_Go(Go_sp,Go_mp);
			Full_Left(L_sp);
			mark = 30;
			break;
		case 30://��ʼʶ���ά��
			MP_Tracking(XJ_sp,60);					//С��ѭ��E2����
			delay_ms(500);
			Full_Return_Data(0xC2);					//���ص����ά���־�ﴦ
			delay_ms(500);
			Full_Circulate_Flag = 0;				//�ȴ���ά��������
			mark = 35;
			break;
		case 35://Ѱ���׶�
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
			    Full_Find_cardsALL4(4,3,1500);					//ѭ��	
				if(RFID_Card_Position==3||RFID_Card_Position==4)
				{
					Full_Left(L_sp);
					delay_ms(500);
					Full_Left(L_sp);
					delay_ms(500); 
					Full_Tracking(XJ_sp);					//ѭ��			
			        Full_Go(Go_sp,Go_mp);					//ǰ��	D2
				}
				else
				{				
				    Full_Right(R_sp);
					Full_Find_cardsALL4(2,1,1500);					//ѭ��	
					Full_Left(L_sp);
					delay_ms(500);
					Full_Left(L_sp);
					delay_ms(500); 
					Full_Tracking(XJ_sp);					//ѭ��			
			        Full_Go(Go_sp,Go_mp);					//ǰ��	D2
					Full_Left(L_sp);
					Full_Tracking(XJ_sp);					//ѭ��			
			        Full_Go(Go_sp,Go_mp);					//ǰ��	D2
				}
			}
//			Full_Tracking(XJ_sp);					//ѭ��			
//			Full_Go(Go_sp,Go_mp);					//ǰ��	D2
//		    Full_Left(L_sp);
//			delay_ms(500);
//		    Full_Left(L_sp);
//		    delay_ms(200);
//		    Full_Tracking(XJ_sp);					//ѭ��			
//			Full_Go(Go_sp,Go_mp);					//ǰ��  F2
//		    Full_Right(R_sp);
//			Full_Tracking(XJ_sp);					//ѭ��	


//            ////////�˴���Ҫ�Ż�		
//			Full_Go(Go_sp,Go_mp);					//ǰ��  F4
//		    Full_Left(L_sp);
//		    delay_ms(500);
//		    Full_Left(L_sp);						//���� F4 ��ͷ����
//		    Full_Find_cardsALL2(2,3);
//		    Full_Left(L_sp);					//��ת
//		    Full_Find_cardsALL2(4,5);
			mark = 40;

			break;
		case 40://����B2
	        Full_Tracking(XJ_sp);					//ѭ��	
			Full_Go(Go_sp,Go_mp);					//ǰ��			
			mark = 45;
			break;
		case 45://С����B2��ʼ����
			Full_Light_Goal(LIGHT_Grent);  		
			Send_Debug_Info("Stall:",10);
			if(LI_ADR[0]==1)      Send_Debug_Info("1\n",3); 
			else if(LI_ADR[0]==2) Send_Debug_Info("2\n",3); 
			else if(LI_ADR[0]==3) Send_Debug_Info("3\n",3); 
			else if(LI_ADR[0]==4) Send_Debug_Info("4\n",3);
			Full_Left(L_sp); 
			mark = 50;
			break;
		case 50://�򿪵�բ
			Control_Sluice_gate();					//�򿪵�բ
		    Control_Sluice_gate();					//�򿪵�բ
			delay_ms(200);
			mark = 55;
			break;
		case 55://С������B4
			Full_Tracking(XJ_sp);					//ѭ��
			Full_Go(Go_sp,Go_mp);					//ǰ�� F6
			mark = 60;
			break;
		case 60://С������C4
			Full_Left(L_sp); 
		    MP_Tracking(XJ_sp,70);
			mark = 65;
			break;
		case 65://����AGV
			AGV_Control_Infrared(Caution_Text);		//��AGV���ͺ��ⱨ������
			AGV_Control_Infrared(Caution_Text);		//��AGV���ͺ��ⱨ������
			AGV_Start();//��AVG���Ŀ�ʼλ�ú�ͣ��λ��							//����AGV��ȫ�Զ�
			AGV_Start();
			AGV_Start();
			Full_Circulate_Flag = 0;				//�ȴ�AGV���������
			mark = 70;
			break;
		case 70:     //
			Control_Sluice_gate();					//�򿪵�բ
		    Control_Sluice_gate();					//�򿪵�բ
			delay_ms(500);
			Send_AGV_Run_flag(0x81);				//��AGV�����������
			Send_AGV_Run_flag(0x81);				//��AGV�����������
			Send_AGV_Run_flag(0x81);				//��AGV�����������
		    Send_Debug_Info("Send OK\n",5);			//Send
			Full_Circulate_Flag = 0;	
			mark = 75;
			break;
		case 75://AVG����բ
			Control_Magnetic_suspension();			//������
			delay_ms(500);
			Control_Magnetic_suspension();			//������
			delay_ms(500);
			mark = 80;
			break;
		case 80://�ȴ���ETC
			Full_Tracking(XJ_sp);					  //ѭ��
			Full_Go(Go_sp,14);						  //ǰ��  D4
			ETC_Open_Flag = 1;
			delay_ms(1000);
			Full_Circulate_Flag = 0;		    	  //�ȴ�ETC��	
			mark = 85;
			break;
		case 85://��ETC
			Full_Tracking(XJ_sp);					//ѭ��
			Full_Go(Go_sp,Go_mp);					//ǰ�� F6
			mark = 90;
			break;
		case 90://С������D6�㣬��ת45��
			Full_Right(R_sp);
			Full_Tracking(XJ_sp);					  //ѭ��
			Full_Go(Go_sp,Go_mp);					//ǰ�� F6					  //ǰ��  D4
		    Full_Right(R_sp);
			Full_Tracking(XJ_sp);					  //ѭ��
			Full_Go(Go_sp,Go_mp);					//ǰ�� F6				  //ǰ��  D4
		    Full_Right(R_sp);
			delay_ms(1000);
			Full_RL_45(R_sp,2,33);					//��ת45��
			mark = 95;
			break;
		case 95://���ͺ������
			Stereo_Display(Car_Text);		   		//������ʾ
			delay_ms(1500);
			Stereo_Display(Car_Text);		   		//������ʾ
			delay_ms(1500);
			Stereo_Display(Car_Text);		   		//������ʾ
			delay_ms(1500);
		    Full_Left(L_sp);
			mark = 100;
			break;
		case 100://С������ʶ����ʻ��D4
		  	MP_Tracking(XJ_sp,50);					//С��ѭ����C2����
		
		    SYN_7318_One_test(1,0);					//����ʶ��
		
		    Full_Tracking(XJ_sp);					  //ѭ��
			Full_Go(Go_sp,Go_mp);					//ǰ�� F6				  //ǰ��  D4
			mark = 105;
			break;		
		case 105://�жϽ��복��
			Full_Car_End_Motion();					//����ͣ�����
			mark = 110;
			break;
		case 110:
			Control_LED_Nixie_light(2);				//�رռ�ʱ
			delay_ms(1000);
			Control_LED_Nixie_light(2);				//�رռ�ʱ
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


void DEbugNUM(u16 xr)//С��6λ��Ŷ
{
	u8 hu[6];
	sprintf((char*)hu,"%d",(char*)xr);	
	Send_Debug_Info((u8 * )hu,strlen(hu));
}

