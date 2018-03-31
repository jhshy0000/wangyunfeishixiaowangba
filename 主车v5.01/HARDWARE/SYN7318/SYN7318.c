
#include "syn7318.h"
#include "string.h"
#include "delay.h"
#include "led.h"
#include "usart.h"
#include "djqd.h"
#include "test.h"
#include "hw.h"
#include "syn7318.h"
#include "canp_hostcom.h"
#include "full_automation.h"

u8 Ysn7813_flag=0;
u8 Ysn7813_flag_cs=0;

//	                         ֡ͷ  ���ݳ��� ���� ���� 04/�Զ��� 31=0X1F
u8 Wake_Up[] = {0xfd,0x00,0x02,0x51,0x1F};
u8 Stop_Wake_Up[] = {0xFD,0x00,0x01,0x52};
//	u8 Play_MP3[] = {0xFD,0x00,0x18,0x61,0x01,0x44,0x3A,0x5C,0x4D,0x70,0x33,0x5C,0x57,0x69,0x66,0x69,0x5F,0xCE,0xD2,0xD4,0xDA,0xD5,0xE2,0x2E,0x6D,0x70,0x33};

  u8 Play_MP3[] ={ 0xFD,0x00,0x1E,0x01,0x01,0xC6,0xF4,0xB6,0xAF,0xD3, 0xEF ,0xD2, 0xF4,
                            	0xBF, 0xD8, 0xD6 ,0xC6 ,0xBC ,0xDD ,0xCA ,0xBB ,0xA3 ,0xAC, 0xC7, 0xEB,
                             	0xB7, 0xA2, 0xB3, 0xF6 ,0xD6, 0xB8, 0xC1, 0xEE };
//	0xFD 0x00 0x1E 0x01 0x01 0xC6 0xF4 0xB6 0xAF 0xD3 0xEF 0xD2 0xF4 0xBF 0xD8 0xD6 0xC6 0xBC 0xDD 0xCA 0xBB 0xA3 0xAC 0xC7 0xEB 0xB7 0xA2 0xB3 0xF6 0xD6 0xB8 0xC1 0xEE 
//   0xFD 0x00 0x23 0x01 0x01 0xFD 0x00 0x1E 0x01 0x01 0xBB 0xB6 0xD3 0xAD 0xB9 0xDB 0xBF 0xB4 0xD3 0xEF 0xD2 0xF4 0xBA 0xCF 0xB3 0xC9 0xCF 0xB5 0xCD 0xB3 0xB5 0xC4 0xD1 0xDD 0xCA 0xBE 0xA3 0xA1
	//                             ֡ͷ  ���ݳ��� ʶ�� �ʵ�	
u8 Start_ASR_Buf[] = {0xFD,0x00,0x02,0x10,0x03};
u8 Stop_ASR_Buf[] = {0xFD,0x00,0x01,0x11};
u8 Start_ASR_One_Buf[] = {0xFD,0x00,0x02,0x10,0x03};//  ���β��� ʹ���û��ʵ�1
//static u8 YY_HXC[8]={0x55 ,0x06 ,0x10 ,0x01 ,0x00 ,0x00 ,0x11 ,0xbb};  // ���Ѵ�
static u8 YY_ZZW[8]={0x55 ,0x06 ,0x10 ,0x04 ,0x00 ,0x00 ,0x14 ,0xbb};  // ��ת��
static u8 YY_SJ [8]={0x55 ,0x06 ,0x20 ,0x01 ,0x00 ,0x00 ,0x21 ,0xbb};  

u8 Stop_Three_One_ASR_Buf[] = {0xFD,0x00,0x01,0x16};  // ֹͣ����һʶ��

u8 D_MP3_1[] = {"D:\\Mp3\\Wifi_�״ο���.mp3"};
u8 D_MP3_2[] = {"D:\\Mp3\\Prompt_������Ը�.mp3"};
u8 D_MP3_3[] = {"D:\\Mp3\\Answer_��Ǹû����.mp3"};
u8 D_MP3_4[] = {0xFD,0x00,0x21,0x15,0x03,0x04,0x02,0x01,0x44,0x3A,0x5C,0x4D,0x70,0x33,0x5C,0x50,0x72,0x6F,0x6D,0x70,0x74,0x5F,0xD6,0xF7,0xC8,0xCB,0xC7,0xEB,0xB7,0xD4,0xB8,0xC0,0x2E,0x6D,0x70,0x33};
u8 Stop_Th2O[] = {0xFD,0x00,0x01,0x16};
u8 TF_MP3_1[] = {"E:\\1-����\\һ�����㲻����.mp3"};

u8 *Dict0[] = {"������","������","�������","��С����","����ʾ��","�ر���ʾ��","�뿪��","��ص�","����һ��","����һ��","��С����","�������"};//00�ʵ�
u8 *Dict1[] = {"���","���޴���","������Ϸ","����֮��","���ؼҿ���","��ʱ�հ���","������Ϸ","��Խ����","��ʪ����","��Ĭ�ǽ�","���˶Դ�","������¹�"};//01�ʵ�
u8 *Dict2[] = {"һ�����㲻����","��ѧ��","��ָ��","������","�ɰ���̫��","��СѼ","���ֵܵĹ���","�ᳪ�������","ũ�����","�����̫��","С͵�͹���","������"};//02�ʵ�
u8 *Dict3[] = {"��һ����ò������õĴ���"};//03�ʵ� ��Ҫ�Լ�������¼����������еĴ���  
u8 ** DICT[10] = {Dict0,Dict1,Dict2,Dict3};

u8 Back[4] = {0};   //��������ش�������
u8 ASR[6] = {0};    //����ʶ�����ش�������
u8 S[4] = {0};      //����ģ�鵱ǰ����״̬�ش�������

u8 BACK_SELF[7];

/*********************************************************************
���� �� ������Delay1ms
������˵�������޲�
����    ������Delay1ms();��ʱ1ms��
*********************************************************************/
void Delay1ms()		 
{
			delay_ms(1);

}

/*********************************************************************
���� �� ������Delay500ms
������˵�������޲�
����    ������Delay500ms();��ʱ500ms��
*********************************************************************/
void Delay500ms()	 
{
     delay_ms(500);
}



/*********************************************************************
���� �� ������SYN7318_Init
������˵�������޲�
����    ������SYN7318_Init();��ʼ������������������Ϊ115200bps
*********************************************************************/
void SYN7318_Init(void)
{
#if  SYSTEM_TASK_TYPE==TASK_TYPE1
	uart3_init( 115200);	
	RCC->APB2ENR |= 1<<6;	 //PE2 RST
	GPIOC->CRH&=0XFFFFFF0F;//PE2 �������
	GPIOC->CRH|=0X00000030;	   
	GPIOC->ODR|=1<<9;     //PE2 �����
#elif  SYSTEM_TASK_TYPE==TASK_TYPE0
	uart2_init(115200);
    RCC->APB2ENR |= 1<<5;	 //PD4
	GPIOD->CRL&=0XFFF0FFFF;	 //PD4 �������
	GPIOD->CRL|=0X00030000;	   
	GPIOD->ODR|=1<<4;        //PD4 �����
#endif
}

u8  SYN7318_Rst(void)  //����ģ�鸴λ
{
   SYN7318_RST_H;	
   delay_ms(10);
	 
   SYN7318_RST_L;	
   delay_ms(100);	

    SYN7318_RST_H;  
	
	while ( 0x55 == SYN7318_Get_char() );
	SYN7318_Get_String(Back ,3 );
	if(Back[2] ==0x4A )  return 1;
	else return 0;
	
}

void SYN7318_Open(void)
{

   Ysn7813_flag =1;
   Ysn7813_flag_cs =1;

}

void SYN7318_Close(void)
{

  Ysn7813_flag = 0;
  Ysn7813_flag_cs = 0;
//	SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ

}



/*********************************************************************
���� �� ������SYN7318_Put_Char----����һ���ֽں���
������˵������txd---�����͵��ֽڣ�8λ��
����    ������SYN7318_Put_Char('d');  ���͡�d��
*********************************************************************/
void SYN7318_Put_Char(u8 txd)
{
#if SYSTEM_TASK_TYPE==TASK_TYPE1
	  U3SendChar( txd );
#elif SYSTEM_TASK_TYPE==TASK_TYPE0
	  U2SendChar( txd );
#endif	
	
}

/*********************************************************************
���� �� ������SYN7318_Put_String----�����ַ�������
������˵������Pst������ַ�����������
              Length���ַ�������
����    ������u8 d[4] = {0x00,0x01,0x02,0x03};
              SYN7318_Put_String(d,4); ---��������d�е�Ԫ��
*********************************************************************/
void SYN7318_Put_String(u8* Pst,u16 Length)
{ 
   	u16 i;
	for(i = 0; i < Length; i++)
	{
		  SYN7318_Put_Char( Pst[i]);
	}
}

/*********************************************************************
���� �� ������SYN7318_Get_char----����һ���ֽں���
������˵�������޲�
���� �� ֵ�������յ����ֽ�
����    ������u8 d;
              d = SYN7318_Get_char();
              SYN7318_Get_char(d); -----������յ����ֽ�
*********************************************************************/

#define WaitTime 200

u8 SYN7318_Get_char( void) 
{

  	  unsigned int return_data=0;
	
#if   SYSTEM_TASK_TYPE==TASK_TYPE1
	   while(!(USART3->SR & USART_FLAG_RXNE));

	   return_data = (u16)USART_ReceiveData(USART3);
#elif SYSTEM_TASK_TYPE==TASK_TYPE0	
	   while(!(USART2->SR & USART_FLAG_RXNE));
	   return_data = (u16)USART_ReceiveData(USART2);
#endif
	
	
	return return_data;
	
}

/*********************************************************************
���� �� ������SYN7318_Get_String----�����ַ�������
������˵������Pst����Ž��յ����ַ�����������
              Length���ַ�������
����    ������u8 d[4] = {0};
              SYN7318_Get_String(d,4);
              SYN7318_Get_String(d,4);----������յ����ַ��� 
*********************************************************************/
void SYN7318_Get_String(u8 *Pst,u8 Length)
{
  u8 i;
  for(i=0;i<Length;i++)
  {
     Pst[i] = SYN7318_Get_char();
  }
	
}

/*********************************************************************
���� �� ������SYN_TTS----�����ϳɲ��ź���
������˵������Pst�����Ҫ�ϳɲ��ŵ��ı���������
����    ������u8 Data[] = {"��������"};
              SYN_TTS(Data); -----�ϳɲ��ű�������
*********************************************************************/
void SYN_TTS(u8 *Pst)
{
	u16 Length;
	u8 Frame[5];   //���淢�����������
	
	
	Length = strlen((char*)Pst); 
	Frame[0] = 0xFD;      //֡ͷ
	Frame[1] = 0x00;
	Frame[2] = Length+2;
	Frame[3] = 0x01;      //�����ϳɲ�������
	Frame[4] = 0x00;      //���ű����ʽΪ��GB2312��
	
    SYN7318_Put_String(Frame, 5);	
	SYN7318_Put_String(Pst, Length);	
	SYN7318_Get_String(Back,4);
	
	//���ճɹ�
	while(!(Back[3] == 0x41))
	{
		SYN7318_Get_String(Back,4);
	}	
	//���м��
	SYN7318_Get_String(Back,4);
	while(!(Back[3] == 0x4f))
	{
		SYN7318_Get_String(Back,4);
	}	
}

/*********************************************************************
���� �� ������SYN_MP3----MP3���ź���
������˵������Pst�����Ҫ���ŵ�MP3��������
����    ������u8 D_MP3_1[] = {"D:\\Mp3\\Wifi_�״ο���.mp3"};
              SYN_MP3(D_MP3_1); ----����MP3
*********************************************************************/
void SYN_MP3(u8 *Pst)
{
	u16 Length;
	u8 Frame[5];   //���淢�����������
	
	Length = strlen((char*)Pst); 
	Frame[0] = 0xFD;      //֡ͷ
	Frame[1] = 0x00;
	Frame[2] = Length+2;
	Frame[3] = 0x61;      //MP3��������
	Frame[4] = 0x04;      //���ű����ʽΪ��GB2312��
	
  SYN7318_Put_String(Frame, 5);	
	SYN7318_Put_String(Pst, Length);	
	SYN7318_Get_String(Back,0);
//	if(Back[3] == 0x41)
//	   LED2 = 0;
}

/*********************************************************************
���� �� ������Start_ASR----��ʼ����ʶ����
������˵������Dict���ʵ���
����    ������Start_ASR(0x00); ----ʶ��0x00�ʵ��еĴ���
*********************************************************************/
void Start_ASR(u8 Dict)
{

	u8 Frame[5];   //���淢�����������
	
	Frame[0] = 0xFD;      //֡ͷ
	Frame[1] = 0x00;
	Frame[2] = 0x02;   
	Frame[3] = 0x10;      //��ʼ����ʶ������
	Frame[4] = Dict;      //�ʵ���  �������޸���Ҫʶ��Ĵʵ���
	
	ASR[3]=0;
	
  SYN7318_Put_String(Frame, 5);	
	SYN7318_Get_String(Back,4); 
	if(Back[3] == 0x41)
	{
		 SYN7318_Get_String(Back,3);  //����ʶ������ش����
		if(Back[0] == 0xfc)
		{
			 SYN7318_Get_String(ASR,Back[2]);
		}
	}
}

/*********************************************************************
���� �� ������Stop_ASR----ֹͣ����ʶ����
������˵�������޲�
*********************************************************************/
void Stop_ASR(void)
{
	u8 Frame[4];   //���淢�����������
	
	Frame[0] = 0xFD;      //֡ͷ
	Frame[1] = 0x00;
	Frame[2] = 0x01;   
	Frame[3] = 0x11;      //ֹͣ����ʶ������
	
  SYN7318_Put_String(Frame, 4);	
	SYN7318_Get_String(Back,4); 
//	if(Back[3] == 0x41)
//	{
//	   LED8 = 0; //D7��
//	}
}

/***************************************************************************
���� �� ������Dict_TTS----ʶ��ʵ�����������
������˵������Dict���ʵ���
����    ������Start_ASR(0x00); ----ʶ��0x00�ʵ��еĴ���
              Dict_TTS(0x00);------���ʶ��ɹ�,��ʶ�𵽵Ĵ����������� 
***************************************************************************/
void Dict_TTS(u8 Dict)
{
	switch(ASR[0])
	{
		case 0x01:
		case 0x02:
		{
			SYN_TTS(DICT[Dict][ASR[3]]);  //ASR[3]�д�Ŵ���ID
		}break;
		case 0x03:
		{
			SYN_TTS("�û�������ʱ");
		}break;
		case 0x04:
		{
			SYN_TTS("�û�������ʱ");
		}break;
		case 0x05:
		case 0x07:
		{
			SYN_TTS("ʶ���ʶ");
		}break;
		case 0x06:
		{
			SYN_TTS("ʶ���ڲ�����");
		}break;
	}
}

/*********************************************************************************************************************************************************
���� �� ������Start_WakeUp----��ʼ�������Ѻ���
������˵������Wake_ID��������ID��----0x00���Ʊ�  0x02��С��   0x04������  0x06�������ܼ�  0x07������ܼ�  0x08��С���ܼ�  0x09����ܼ�  0x1F���Զ���
����    ������Start_WakeUp(0x09);----������Ϊ��ܼ�,���ѳɹ�֮�󲥱��������⡱
**********************************************************************************************************************************************************/
void Start_WakeUp(u8 Wake_ID)
{
	u8 Frame[5];   //���淢�����������
	
	Frame[0] = 0xFD;      //֡ͷ
	Frame[1] = 0x00;
	Frame[2] = 0x02;   
	Frame[3] = 0x51;      //��ʼ������������
	Frame[4] = Wake_ID;   
	
  SYN7318_Put_String(Frame,5);	
	SYN7318_Get_String(Back,4); 
	if(Back[3] == 0x41)  
	//while(Back[3] == 0x41)
	{
		 SYN7318_Get_String(Back,4);  //������������ش����
		 if(Back[3] == 0x21)
		 //while(Back[3] == 0x21)
		 {
			 SYN_MP3(D_MP3_2);
		 }
	}
}

/*********************************************************************************************************************************************************
���� �� ������Three_One----����һ�����������������ѣ����Ѻ󲥷���ʾ���������������ʶ��
������˵������Dict���ʵ���
              Wake_ID��������ID��----0x00���Ʊ�  0x02��С��   0x04������  0x06�������ܼ�  0x07������ܼ�  0x08��С���ܼ�  0x09����ܼ�  0x1F���Զ���
              Sound����ʾ������  0x00������ʾ��  0x01���ı�����  0x02������MP3����
              Pst����ʾ��������
����    ������Three_One(0x00, 0x06, 0x02, D_MP3_2); ----������Ϊ�����ܼ�,���ѳɹ��󲥷š������⡱,֮�����ʶ��0x00�ʵ��еĴ��� 
              Dict_TTS(0x00); ----���Dict_TTS������ʶ������������
**********************************************************************************************************************************************************/
void Three_One(u8 Dict, u8 Wake_ID, u8 Sound, u8 *Pst)
{
	u8 Frame[100];   //���淢�����������
	u16 Length,i;
	
	Frame[0] = 0xFD;      //֡ͷ
//	Frame[1] = 0x00;
//	Frame[2] = 0x07;    //���ڲ�ȷ���������ĳ��ȣ�����ѡ����ʱ����
	Frame[3] = 0x15;      //����һʶ������
	Frame[4] = Dict;      //�ʵ���
	Frame[5] = Wake_ID;   //������ID��
	Frame[6] = Sound;     //��ʾ������
	if(Sound == 0x00)     //����ʾ��
	{
		Frame[1] = 0x00;
		Frame[2] = 0x04;
	  SYN7318_Put_String(Frame, 7);	
	}
	else                 //����ʾ��
	{
		Frame[7] = 0x01;      //�ı�������MP3��ʾ�������ʽ��0x00��GB2312����  0x01��GBK����  0x02��BIG5����  0x03��UnicodeСͷ  0x04�Unicode��ͷ
		Length = strlen((char*)Pst); //�ı�������MP3��ʾ���ĳ���
		Frame[1] = 0x00;
		Frame[2] = Length + 5;
		for(i = 0; i < Length; i++)
		{
			Frame[8 + i] = Pst[i];
		}
	  SYN7318_Put_String(Frame, Length + 8);	
	}
	 SYN7318_Get_String(Back,4); 
	if(Back[3] == 0x41)
	{
		SYN7318_Get_String(Back,3);  //����ʶ������ش����
		if(Back[0] == 0xfc)
		{
			 SYN7318_Get_String(ASR,Back[2]);
			
			 Yu_Yin_Asr();
		}
	}
	
}

/***************************************************************************
���� �� ������Status_Query----ģ��״̬��ѯ����
������˵�������޲�
����    ������Status_Query(); 
***************************************************************************/
void Status_Query(void)
{
	u8 Frame[4];   //���淢�����������
	
	Frame[0] = 0xFD;    //֡ͷ
	Frame[1] = 0x00;
	Frame[2] = 0x01;   
	Frame[3] = 0x21;   //״̬��ѯ����
	
    SYN7318_Put_String(Frame, 4);	
	SYN7318_Get_String(Back,4); 
	if(Back[3] == 0x41)  
	{
		 SYN7318_Get_String(S,4);  //ģ�鵱ǰ����״̬�Ļش����
	}
}
/*
<------   ���ճɹ�             0xFC 0x0 0x1 0x41 
<------   ʶ���ʶ             0xFC 0x0 0x1 0x7 

����һ

<------   ʶ��ɹ�,ƥ���=9,����ID=16,����ID=117    0xFC 0x0 0x6 0x1 0x9 0x0 0x10 0x0 0x75 
<------   0xFD,0x00,0x21,0x15,0x03,0x04,0x02,0x01,0x44,0x3A,0x5C,0x4D,0x70,0x33,0x5C,0x50,0x72,0x6F,0x6D,0x70,0x74,0x5F,0xD6,0xF7,0xC8,0xCB,0xC7,0xEB,0xB7,0xD4,0xB8,0xC0,0x2E,0x6D,0x70,0x33
          ֡ͷ   ����  ������ �ʵ� ���� ���� GBK  ����--->

����

���������ѡ�: len= 5   FD 00 02 51 04 
<------   ���ճɹ�     0xFC 0x0 0x1 0x41 

�������ϳɲ��š�: len= 22   FD 00 13 01 01 5B 6D 35 35 5D A1 B0 B0 D9 C1 E9 A1 B1 5B 6D 33 5D 
<------   ���ѳɹ�          0xFC 0x0 0x1 0x21 
<------   ���ճɹ�          0xFC 0x0 0x1 0x41 
<------   ģ�����          0xFC 0x0 0x1 0x4F 
*/

#define SYN_RST_MODE   1  // 0 ����λ   1 ������λ

void SYN7318_Test( void)  // ������������
{ 
	//unsigned int i = 0;
   
	while(Ysn7813_flag_cs)
	{
		
	 SYN7318_Init();		
	 SYN_TTS("����ʶ�����,�뷢�������Ѵʣ�������ʻ");
	 LED2 = !LED2;
	 Status_Query();  //��ѯģ�鵱ǰ�Ĺ���״̬		
	 if(S[3] == 0x4F)  //ģ����м���������
	 {

		Delay1ms();
		SYN7318_Put_String(Wake_Up,5);//���ͻ���ָ��
		SYN7318_Get_String(Back,4);   //���շ�����Ϣ
		if(Back[3] == 0x41)           //���ճɹ�
		{  
		while(Ysn7813_flag_cs)
		{			
			delay_ms(1000);
			delay_ms(1000);
		  full_Speech("������ʻ",8);
			
			//BEEP = 0;
			delay_ms(1000);delay_ms(1000);
			//BEEP = 1;
		  SYN7318_Get_String(Back,3); //����ǰ��λ�ش�����
			BEEP = 0;
			
		  if(Back[0] == 0xfc)       //֡ͷ�ж�
		  {
			
			SYN7318_Get_String(ASR,Back[2]);//���ջش�����
			if(ASR[0] == 0x21)              //���ѳɹ�
			{
				SYN7318_Put_String(Play_MP3,33);//�����Ѿ�����������ʻ���뷢��ָ��

				SYN7318_Get_String(Back,4);
				SYN7318_Get_String(Back,4);
				
				while(!(Back[3] == 0x4f));    //�ȴ�����
				{
					LED2 = ~LED2;
					Delay500ms();
				}			
					//��ʼ����ʶ��
					while(Ysn7813_flag)
					{									
						SYN7318_Put_String(Start_ASR_Buf,5);//������ʶ������					
						SYN7318_Get_String(Back,4);        //���շ�����Ϣ
						if(Back[3] == 0x41)               //���ճɹ�
						{
							LED1 = ~LED1;             //LED1��ת
							full_Speech("С����ת",8);
							SYN7318_Get_String(Back,3);  //����ʶ������ش����
							if(Back[0] == 0xfc)        //֡ͷ�ж�
							{
								LED2 = ~LED2;
								SYN7318_Get_String(ASR,Back[2]);//���ջش�����
								Yu_Yin_Asr();
							}
						}
								
					}
					SYN7318_Put_String(Stop_Wake_Up,4);//����ֹͣ����ָ��
			}
			else                          //�����ڲ�����
			{
				LED3 =~LED3;
			}
			}
		}
			}	
			else LED3 =~LED3;
		 } else {
		 
		 }
	}
}


void Yu_Yin_Asr( void)  // ����ʶ������
{
			switch(ASR[0])
			{
				case 0x01:
				{
					switch(ASR[3])
					{
	                    case 0x1A:  // С��ǰ��
						{
							Go_Test( 80,10);
							
							SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
							break;
						}
						case 0x1B:   //  С������
						{
							SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ�� 
							Back_Test(80,10);
								
							break;
						}
						case 0x1C:   //  С����ת
						{							  							  
							Full_Left(90);
							Ysn7813_flag = 0;
							Ysn7813_flag_cs = 0;
							SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��;
							break;
						}
						case 0x1D:  // С����ת
						{														  
							Full_Right(90);
							Ysn7813_flag = 0;
							Ysn7813_flag_cs = 0;
							SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
							break;
						}
						case 0x1E:  // С��ֹͣ
						{														
							Stop_Test();
							
							SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
							break;
						}
						case 0x1F:   //  С��ѭ��
						{														
							Track_Test( 50);
							
							SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
							break;
						}	                      
						default:
						{					  
							SYN_TTS("�Բ�����û����");
							SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
							break;
						}
					}
						SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x02: //ʶ��ɹ���������ID�ţ�
				{
					SYN_TTS("�Բ�����û����");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x03://�û�������ʱ
				{
					SYN_TTS("�������ߣ���ʱΪ������");
//					Ysn7813_flag = 0;
//					Ysn7813_flag_cs = 0;
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x04:
				{
					SYN_TTS("�밲�����鷳����˵һ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x05:
				{
					SYN_TTS("�Բ�������˵һ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x06:
				{
					SYN_TTS("ʶ���ڲ�����");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x07:
				{
					SYN_TTS("�Բ�������˵һ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				default:
				{
					SYN_TTS("����");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
		}

}


/****************************************************************
�������ܣ�ȫ�Զ�����ʶ��
��	  ��: mode =  1 ���ָ��  mode != 1 ָ������  num = ָ������ָ��
����  ֵ����
*****************************************************************/
void SYN_7318_One_test(u8 mode,u8 num)  // mode ģʽ���� 1 ���ָ�� 0 ָ��ָ��
{
  u8 error_num=0;
    SYN_TTS("��ʼʶ��");
	Ysn7813_flag =1;
	while(Ysn7813_flag)
	{								
		SYN7318_Init();
		Status_Query();   //��ѯģ�鵱ǰ�Ĺ���״̬	
		if(S[3] != 0x4F)  //ģ����м���������  
		{ 
			 SYN_TTS("ģ��æµ����ʱ�޷�Ϊ���������ڸ�λ��");
			 while( !SYN7318_Rst());
			 SYN7318_Init();
			 SYN_TTS("ģ�鸴λ�ɹ���");
		}
		delay_ms(500);
		SYN7318_Put_String(Start_ASR_One_Buf,5);//������ʶ������  // �û��ʵ� 2  Start_ASR_One_Buf
								
		SYN7318_Get_String(Back,4);     //���շ�����Ϣ
		if(Back[3] == 0x41)           //���ճɹ�
		{
			LED1 = ~LED1;             //LED1��ת
			delay_ms(1000);
			if(mode) 
			{
				YY_ZZW[3] = num;
				Send_ZigbeeData_To_Fifo( YY_SJ,8); // �������
			}
			else  
			{
				YY_ZZW[3] = num;
				Send_ZigbeeData_To_Fifo( YY_ZZW,8); // ָ������
			}
				
			SYN7318_Get_String(Back,3);  //����ʶ������ش����
			if(Back[0] == 0xfc)        //֡ͷ�ж�
			{
				LED2 = ~LED2;
				SYN7318_Get_String(ASR,Back[2]);//���ջش�����
                // Yu_Yin_Asr();

				if(ASR[0] ==0x02&&ASR[1] !=0x00&&ASR[2]==0) 
				{
					switch(ASR[3]) // ����ID
					{
						case 0x00:   //���߹㳡վ  
					  {
							  SYN_TTS("���߹㳡վ��ͣ�ڳ���A");
							  SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
							  Car_Position_news[1] = 0x01;
							  Ysn7813_flag =0;
							  
							 break;
						}														
						case 0x01:   //  ֣�ݶ�վ
						{								 
							 SYN_TTS("֣�ݶ�վ��ͣ�ڳ���B");						  
							 SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
							 Car_Position_news[1] = 0x02;
							 Ysn7813_flag =0;
							 break;
						}														
                        case 0x02:   //  ����԰վ
						{														  															
							SYN_TTS("����԰վ��ͣ�ڳ���C");
							
                            SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
							Car_Position_news[1] = 0x03;
							Ysn7813_flag =0;
							break;
						}
                        case 0x03:   //��������վ
						{
							SYN_TTS("��������վ��ͣ�ڳ���D");
                            SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
							Car_Position_news[1] = 0x04;
							Ysn7813_flag =0;
							break;
						}														
                        case 0x04:   //�����깬վ
						{														  
							
							SYN_TTS("�����깬վ��ͣ�ڳ���E");
                            SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
							Car_Position_news[1] = 0x05;
							Ysn7813_flag =0;
							break;													
						}
						case 0x05:  //����֮��
						{														  
							
							SYN_TTS("����֮��վ��ͣ�ڳ���F");
                            SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
							Car_Position_news[1] = 0x06;
							Ysn7813_flag =0;
							break;													
						}
						default:
						{														  
							SYN_TTS("û�и����");
							SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
							//Ysn7813_flag =0;
							break;
						}
					}
				}
				else if(ASR[0] ==0x03)
				{
				   SYN_TTS("����ʶ��ʱ����ʱ");
				   SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��				
				}
				else if(ASR[0] ==0x07)
				{
					SYN_TTS("����ʶ�𱻾ܣ�����");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
				}
				else
				{
				   SYN_TTS("����ʶ�����������");
				   SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
				}
					
                 
			}
		}			
		/*************************************************/
		else
		{
				SYN_TTS("����������ʶ��ʧ�ܣ�");
			    SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
		}			
		delay_ms(1000);
        error_num++;	
        if( error_num >3)  	Ysn7813_flag =0;	
	}		
}




void Three_One_Test(u8 mode) // ����һ����  // 0����ʶ��   1���ʶ��
{
 
	SYN7318_Init();
	SYN_TTS("������������һ[=yi1]���ԣ��뷢���Ѵ� ������ʻ");  
	if(!mode)  // ����ʶ��
 {   
  	
	Three_One(0x03, 0x1F, 0x01, "ģ�黽�ѳɹ����뷢����ָ��");
	SYN7318_Put_String(Stop_Three_One_ASR_Buf,4); //ֹͣ����һʶ��
	
	SYN7318_Get_String(Back,4);
	
	//���ճɹ�
	while(!(Back[3] == 0x41))
	{
		SYN7318_Get_String(Back,4);
	}	
			  
	 SYN_TTS("��������һ[=yi1]�������");  
 }
 
 else 
 {
     while(Ysn7813_flag )
		 {
		    Three_One(0x03, 0x1F, 0x01, "ģ�黽�ѳɹ����뷢����ָ��");
	      SYN7318_Put_String(Stop_Three_One_ASR_Buf,4); //ֹͣ����һʶ��
		     
			  SYN7318_Get_String(Back,4);
	
	    //���ճɹ�
	    while(!(Back[3] == 0x41))
	    {
		    SYN7318_Get_String(Back,4);
	    }	
			 
      if(  (ASR[0]==0x01)&& (ASR[3]==0x19))		// ʶ���˳�����
			{
			     SYN_TTS("�ɰ��°���S3�ٴν������"); 
			
			}		
      else 			
				SYN_TTS("���ٴλ���ģ�� ������ʻ");  
		 }
// 
 
 
 }

}
