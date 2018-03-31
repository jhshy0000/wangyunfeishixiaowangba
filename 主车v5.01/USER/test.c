/************************************************************
**  ����: ����7�����ײ����
	�汾: V5.21-15:45
	�޸�: ��
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


#define  ZCKZ_ADDR    0xAA  // ���������־���ַ���
#define  YSBZW_ADDR   0x02  // ���������־���ַ���
#define  DZ_ADDR      0x03  // �����բ��־���ַ���
#define  LEDXS_ADDR   0x04  // ����LED��ʾ��־���ַ���
#define  JXB_ADDR     0x05  // �����е�۱�־���ַ���

void Key_Onclink(void);
void IO_Init(void); //IO��ʼ��	
void DIY_BEEP(u16 T_Hz,u8 timer);


u8 G_Tab[6];	   //������ⷢ������
u8 S_Tab[NUM]; 	   //������������������
u8 C_Tab[NUM]; 	   //�����˶���־�ﷵ����������

u8 Stop_Flag=0;    //״̬��־λ
u8 Track_Flag=0;     //ѭ����־λ
u8 G_Flag=0;	   //ǰ����־λ
u8 B_Flag=0;	   //���˱�־λ
u8 L_Flag=0;	   //��ת��־λ
u8 R_Flag=0;	   //��ת��־λ
u8 SD_Flag=1;      //�˶���־�����ݷ��������־λ
u8 Rx_Flag =0;

u16 CodedDisk=0;   //����ֵͳ��
u16 tempMP=0;	   //��������ֵ
u16 MP;			   //��������ֵ
int Car_Spend = 50;//С���ٶ�Ĭ��ֵ
u32 count = 0;	   //��ѭ��ʱ  ��������ѭ����ȫ���Ĵ���  ���ڴ˲����ɷ�ֹѭ��������
int LSpeed;		   //ѭ�������ٶ�
int RSpeed;		   //ѭ�������ٶ�
u8 Line_Flag=0;	   // 
u8 send_Flag=0;	   // ���ͱ�־λ

unsigned Light=0; //���ն�

u16 error_Flag = 0;

// ������

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
	
	NVIC_Configuration(); 	 //����NVIC�жϷ���0:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
		
	
	SYN7318_Init();//������ʼ��
	

	IO_Init();                  //IO��ʼ��

	S_Tab[0]=0x55;
	S_Tab[1]=0xaa;

	C_Tab[0]=0x55;
	C_Tab[1]=0x02;
	
	CanP_Init();
	
	Power_Check();  //�������  �ϵ��ȼ��һ�ε���
	Send_Electric( Electric_Buf[0],Electric_Buf[1]);

	STOP();
	Host_Close_UpTrack();
	
 	Set_Track_Init();			         //����ѭ����ʼ������
	//   Buart1_init(115200);
	Readcard_daivce_Init();
	
    Send_Debug_Info("A1B2C3\n",8); // �ϴ�������Ϣ
    //LED_RL(120);
	while(1)
	{	 
		Key_Onclink();
		
		while(Full_Circulate_Flag)
		{
			LED1 = 1;
			LED0 = 0;
			Full_Motion();
			error_Flag = 0;				//��ʱ״̬��־λ����
			Wifi_Rx_flag = 0;			//WIFI����״̬��־λ����
			Zigbee_Rx_flag = 0;			//ZigBee����״̬��־λ����
		}
		if(error_Flag == 900)			//90 �� 
		{
			error_Flag = 0;
			Full_Circulate_Flag = 1;   //�Զ�����ȫ�Զ�
			DIY_BEEP(4,25);
		}
		error_Flag++;
		if(ETC_Open_Flag == 1)	//�����Ӧ������С����ǰ��һ��֪����Ӧ��Ϊֹ				
		{
			if(error_Flag == 25 || error_Flag == 50 || 
			   error_Flag == 75 || error_Flag == 100 || error_Flag == 125)		// 5s 10s 15s
			{
				Full_Go(40,3);
			}
			
		}
		
		LED0 = !LED0;					//����״̬	
		delay_ms(100);
		LED1 = 0;
		
		////////////////////////
		if(Wifi_Rx_flag ==1)  			// wifi ���ձ� ��
		{
			  Wifi_Rx_flag =0;
			  if(Wifi_Rx_Buf[0]==0x55)  // ���յ�55��ͷ����
			  {			  	 
			  	     Normal_data(); 	//�������ݴ���
			  }
		}

		if(Rx_Flag ==1)						   //���յ�����ָ��
		{
			if(Wifi_Rx_Buf[1]==ZCKZ_ADDR) 	   //��������
			{
				switch(Wifi_Rx_Buf[2])
				{
					case 0xB1:				   //��ʼָ��   1
						Car_Position_news[0] = Wifi_Rx_Buf[3];		//�õ�������ʼλ�� 02
						Car_Position_news[1] = Wifi_Rx_Buf[4];		//�õ��������λ�� 05
						Car_Position_news[2] = Wifi_Rx_Buf[5];		//δʹ��          00
						mark = 5;
						Full_Circulate_Flag = 1;
						break;
					case 0xB2:				                         //��״����ʶ�����
						for(i=0;i<6;i++)
						{
							Car_Text[i] = Hw_Data[i];		//���泵��
						}		
						break;
					case 0xB3:	//								//������״����
						Shape_Number[1] = Hw_Data[0];			//����
						Shape_Number[3] = Hw_Data[1];			//Բ
						Shape_Number[5] = Hw_Data[2];   		//������
						Shape_Number[7] = Hw_Data[3];			//����
						Shape_Number[9] = Hw_Data[4];			//�����
						Full_Circulate_Flag = 1;
						break;
					case 0x10:  			//��������ǰ��λ
						Hw_Data[0]=Wifi_Rx_Buf[3];
						Hw_Data[1]=Wifi_Rx_Buf[4];
						Hw_Data[2]=Wifi_Rx_Buf[5];
					break;
					case 0x11: 				//�������ݺ���λ
						Hw_Data[3]=Wifi_Rx_Buf[3];
						Hw_Data[4]=Wifi_Rx_Buf[4];
						Hw_Data[5]=Wifi_Rx_Buf[5];
					break;
					case 0x12: 				//�������ݺ���λ
						LI_ADR[0]=Wifi_Rx_Buf[3];//���⵲λ
						LI_ADR[1]=Wifi_Rx_Buf[4];//RFID��ַ
//					    LI_ADR[0]=0x02;//���⵲λ
//					    LI_ADR[1]=0x14;//RFID��ַ
						LI_ADR[2]=0;
					break;
					case 0x20://����TFT��ҳ
						if(Wifi_Rx_Buf[3] == 0x01)
						{
							//TFT_turn_over(1);//TFT��һҳ
							Transmition(H_S,4);
						}
						else if(Wifi_Rx_Buf[3] == 0x02)
						{
							//TFT_turn_over(2);//TFT��һҳ
							Transmition(H_X,4);
						}
						break;
					case 0xB4:
						for(i=0;i<6;i++)
						{
							//Caution_Text[i] = Hw_Data[i];		//���汨������
							Caution_Text[i]=HW_K[i];
							Send_Debug_Info("�к�������",8); // �ϴ�������Ϣ
							
						}
						Send_Debug_Info(Caution_Text,6); // �ϴ�������Ϣ
						Full_Circulate_Flag = 1;
						break;
					case 0xB5:
							
						break;
				}
	    	Rx_Flag =0;
			}
		} 
		///////////////////////////////////
		if(Zigbee_Rx_flag ==1)	 //�ȵ�ETC��Ӧ���
		{
		    Send_Debug_Info("one\n",6); // �ϴ�������Ϣ
			Zigbee_Rx_flag =0;
			if((Zigb_Rx_Buf[1]==0x0c) && (ETC_Open_Flag == 1)) 			//ETC
			{
			   if(Zigb_Rx_Buf[2]==0x01)
			   {
					if(Zigb_Rx_Buf[3]==0x01)							//ETC��
					{
						Full_Circulate_Flag = 1;//������һ����						
					    Send_Debug_Info("ETC\n",6); // �ϴ�������Ϣ
					}							
			   }
			}
			else if(Zigb_Rx_Buf[1]==0x21)	 	//AGV��������
			{
			    Send_Debug_Info("two\n",6); // �ϴ�������Ϣ
				switch(Zigb_Rx_Buf[2])
				{
					case 0x01:							  //�����ɹ�
						Full_Circulate_Flag = 1;
						break;
					case 0x02:							  //�������
						Full_Circulate_Flag = 1;
						break;
					case 0x03:							  //������
						Full_Circulate_Flag = 1;
						break;
				}		
			}	
		}
	}
	
}

void Key_Onclink(void)
{

		if(!KEY0)			  //����K1����
		{
			 delay_ms(30);
			 if(!KEY0)
			 {
				 //SYN_7318_One_test(1,0);					//����ʶ��
				   //Start_AGV_Position = 1;
				   Send_Debug_Info("AGV=1,B4\n",10); // �ϴ�������Ϣ
			 }
		}
	   if(!KEY1)			//����K2����
		 {  
			 delay_ms(30);
			 if(!KEY1)
			 {
			    Start_AGV_Position = 2;
				  Send_Debug_Info("AGV=2,D2\n",10); // �ϴ�������Ϣ
			 }
		}
	  if(!KEY2)			  //����K3����
		{
			delay_ms(30);
			if(!KEY2)
			{ 
				 Start_AGV_Position = 4;
				 Send_Debug_Info("AGV=4,F4\n",10); // �ϴ�������Ϣ	
			}
		}
		if(!KEY3)			  //����K4����
		{
			delay_ms(30);
			if(!KEY3)
			{
				  Start_AGV_Position = 3;
				  Send_Debug_Info("AGV=3,D6\n",10); // �ϴ�������Ϣ
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
** ���ܣ�     ��ʼ�����İ���ʹ�ö˿�
** ������	  �޲���
** ����ֵ��    ��
****************************************************************/
void IO_Init(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
    YJ_INIT();			         //��ʼ��Ӳ��
	
    RCC_APB2PeriphClockCmd(PSS_APB|LED_L_APB| BEEP_APB|LED_R_APB|RCC_APB2Periph_GPIOD|LED_SET_APB,ENABLE);							  
   
    GPIO_InitStructure.GPIO_Pin  = PSS_PIN;			//����״̬
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   //������������
 	GPIO_Init(PSS_GPIO, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin  = BEEP_PIN;		 //����������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BEEP_GPIO, &GPIO_InitStructure);

   	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;		 //���İ������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	#if SYSTEM_TASK_TYPE==TASK_TYPE1
	
	GPIO_InitStructure.GPIO_Pin  = LED_SET_PIN;		 //��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_SET_GPIO, &GPIO_InitStructure);	
	LED_SET=1;


	#elif SYSTEM_TASK_TYPE==TASK_TYPE0
	GPIO_InitStructure.GPIO_Pin  = LED_L_PIN;		 //��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_L_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin  = LED_R_PIN;		 //������ҵ�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_R_GPIO, &GPIO_InitStructure);
	
	LED_L=1;	
	LED_R=1;
	#endif
	
	
	BEEP=1;
	beep=0;//�ر�����������
		
}

