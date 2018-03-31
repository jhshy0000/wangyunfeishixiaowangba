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
#define  AGVBZW_ADDR   0x02  // ���������־���ַ���
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

	while(1)
	{	 
		LED0=!LED0;
		LED1 = 0;
		delay_ms(100);		
		Key_Onclink();
		if(!KEY0)			  //����K1����
		 {
		   delay_ms(10);
			if(!KEY0)
			{
				 AGV_Tracking(80,6000);
//					Full_AGV_45RL(90,2,500);		//��ת
//			AGV_mark = 5;
//			AGV_Run_Flag = 1;
			}
		}
		while(Full_Circulate_Flag)
		{
			LED1 = 1;
			LED0 = 0;
			Full_Motion();
			Zigbee_Rx_flag = 0;			//ZigBee����״̬��־λ����
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
		///////////////////////////////////
		if(Zigbee_Rx_flag ==1)	 //�ȵ�ETC��Ӧ���
		{
		    
		 if(Zigb_Rx_Buf[1]==AGVBZW_ADDR)	 	//AGV��������
			{
			    Send_Debug_Info("Receiv\n",8); // �ϴ�������Ϣ
				switch(Zigb_Rx_Buf[2])
				{
					   case 0x80:
							Start_AGV_Position = Zigb_Rx_Buf[3];//AGV�ӳ�����
							End_AGV_Position   = Zigb_Rx_Buf[4];//AGVͣ���ĳ���
						   // DEbugNUM(Zigb_Rx_Buf[4]);
							mark = 5;
							Full_Circulate_Flag = 1;
						break;
						case 0x81:
							Full_Circulate_Flag = 1;
							Send_Debug_Info("Ruku\n",8); // �ϴ�������Ϣ
							break;
						case 0x10:  //��������ǰ��λ
							G_Tab[0]=Zigb_Rx_Buf[3];
							G_Tab[1]=Zigb_Rx_Buf[4];
							G_Tab[2]=Zigb_Rx_Buf[5];
							break;
						case 0x11: 	//�������ݺ���λ
							G_Tab[3]=Zigb_Rx_Buf[3];//���ݵ���λ
							G_Tab[4]=Zigb_Rx_Buf[4];//��λУ����
							G_Tab[5]=Zigb_Rx_Buf[5];//��λУ����
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

				   Start_AGV_Position = 1;
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
	
	#if SYSTEM_TASK_TYPE==TASK_TYPE0
	
	GPIO_InitStructure.GPIO_Pin  = LED_SET_PIN;		 //��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_SET_GPIO, &GPIO_InitStructure);	
	LED_SET=0;	
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

