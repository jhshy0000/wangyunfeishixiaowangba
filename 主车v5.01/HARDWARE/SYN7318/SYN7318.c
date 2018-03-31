
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

//	                         帧头  数据长度 唤醒 百灵 04/自定义 31=0X1F
u8 Wake_Up[] = {0xfd,0x00,0x02,0x51,0x1F};
u8 Stop_Wake_Up[] = {0xFD,0x00,0x01,0x52};
//	u8 Play_MP3[] = {0xFD,0x00,0x18,0x61,0x01,0x44,0x3A,0x5C,0x4D,0x70,0x33,0x5C,0x57,0x69,0x66,0x69,0x5F,0xCE,0xD2,0xD4,0xDA,0xD5,0xE2,0x2E,0x6D,0x70,0x33};

  u8 Play_MP3[] ={ 0xFD,0x00,0x1E,0x01,0x01,0xC6,0xF4,0xB6,0xAF,0xD3, 0xEF ,0xD2, 0xF4,
                            	0xBF, 0xD8, 0xD6 ,0xC6 ,0xBC ,0xDD ,0xCA ,0xBB ,0xA3 ,0xAC, 0xC7, 0xEB,
                             	0xB7, 0xA2, 0xB3, 0xF6 ,0xD6, 0xB8, 0xC1, 0xEE };
//	0xFD 0x00 0x1E 0x01 0x01 0xC6 0xF4 0xB6 0xAF 0xD3 0xEF 0xD2 0xF4 0xBF 0xD8 0xD6 0xC6 0xBC 0xDD 0xCA 0xBB 0xA3 0xAC 0xC7 0xEB 0xB7 0xA2 0xB3 0xF6 0xD6 0xB8 0xC1 0xEE 
//   0xFD 0x00 0x23 0x01 0x01 0xFD 0x00 0x1E 0x01 0x01 0xBB 0xB6 0xD3 0xAD 0xB9 0xDB 0xBF 0xB4 0xD3 0xEF 0xD2 0xF4 0xBA 0xCF 0xB3 0xC9 0xCF 0xB5 0xCD 0xB3 0xB5 0xC4 0xD1 0xDD 0xCA 0xBE 0xA3 0xA1
	//                             帧头  数据长度 识别 词典	
u8 Start_ASR_Buf[] = {0xFD,0x00,0x02,0x10,0x03};
u8 Stop_ASR_Buf[] = {0xFD,0x00,0x01,0x11};
u8 Start_ASR_One_Buf[] = {0xFD,0x00,0x02,0x10,0x03};//  单次测试 使用用户词典1
//static u8 YY_HXC[8]={0x55 ,0x06 ,0x10 ,0x01 ,0x00 ,0x00 ,0x11 ,0xbb};  // 唤醒词
static u8 YY_ZZW[8]={0x55 ,0x06 ,0x10 ,0x04 ,0x00 ,0x00 ,0x14 ,0xbb};  // 左转弯
static u8 YY_SJ [8]={0x55 ,0x06 ,0x20 ,0x01 ,0x00 ,0x00 ,0x21 ,0xbb};  

u8 Stop_Three_One_ASR_Buf[] = {0xFD,0x00,0x01,0x16};  // 停止三合一识别

u8 D_MP3_1[] = {"D:\\Mp3\\Wifi_首次开机.mp3"};
u8 D_MP3_2[] = {"D:\\Mp3\\Prompt_主人请吩咐.mp3"};
u8 D_MP3_3[] = {"D:\\Mp3\\Answer_抱歉没听清.mp3"};
u8 D_MP3_4[] = {0xFD,0x00,0x21,0x15,0x03,0x04,0x02,0x01,0x44,0x3A,0x5C,0x4D,0x70,0x33,0x5C,0x50,0x72,0x6F,0x6D,0x70,0x74,0x5F,0xD6,0xF7,0xC8,0xCB,0xC7,0xEB,0xB7,0xD4,0xB8,0xC0,0x2E,0x6D,0x70,0x33};
u8 Stop_Th2O[] = {0xFD,0x00,0x01,0x16};
u8 TF_MP3_1[] = {"E:\\1-歌曲\\一条大鱼不见了.mp3"};

u8 *Dict0[] = {"音量加","音量减","最大音量","最小音量","打开提示音","关闭提示音","请开灯","请关灯","调暗一点","调亮一点","最小亮度","最大亮度"};//00词典
u8 *Dict1[] = {"你好","查无此人","缠绵游戏","长江之歌","常回家看看","超时空爱情","超速游戏","超越梦想","潮湿的心","沉默是金","成人对待","城里的月光"};//01词典
u8 *Dict2[] = {"一条大鱼不见了","上学歌","五指歌","升国旗","可爱的太阳","丑小鸭","两兄弟的故事","会唱歌的秘密","农夫和蛇","北风和太阳","小偷和公鸡","公鸡叫"};//02词典
u8 *Dict3[] = {"第一个最好不放有用的词条"};//03词典 需要自己利用烧录成这个数组中的词条  
u8 ** DICT[10] = {Dict0,Dict1,Dict2,Dict3};

u8 Back[4] = {0};   //接收命令回传的数组
u8 ASR[6] = {0};    //接收识别结果回传的数组
u8 S[4] = {0};      //接收模块当前工作状态回传的数组

u8 BACK_SELF[7];

/*********************************************************************
【函 数 名】：Delay1ms
【参数说明】：无参
【简    例】：Delay1ms();延时1ms。
*********************************************************************/
void Delay1ms()		 
{
			delay_ms(1);

}

/*********************************************************************
【函 数 名】：Delay500ms
【参数说明】：无参
【简    例】：Delay500ms();延时500ms。
*********************************************************************/
void Delay500ms()	 
{
     delay_ms(500);
}



/*********************************************************************
【函 数 名】：SYN7318_Init
【参数说明】：无参
【简    例】：SYN7318_Init();初始化函数，波特率设置为115200bps
*********************************************************************/
void SYN7318_Init(void)
{
#if  SYSTEM_TASK_TYPE==TASK_TYPE1
	uart3_init( 115200);	
	RCC->APB2ENR |= 1<<6;	 //PE2 RST
	GPIOC->CRH&=0XFFFFFF0F;//PE2 推挽输出
	GPIOC->CRH|=0X00000030;	   
	GPIOC->ODR|=1<<9;     //PE2 输出高
#elif  SYSTEM_TASK_TYPE==TASK_TYPE0
	uart2_init(115200);
    RCC->APB2ENR |= 1<<5;	 //PD4
	GPIOD->CRL&=0XFFF0FFFF;	 //PD4 推挽输出
	GPIOD->CRL|=0X00030000;	   
	GPIOD->ODR|=1<<4;        //PD4 输出高
#endif
}

u8  SYN7318_Rst(void)  //语音模块复位
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
//	SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识

}



/*********************************************************************
【函 数 名】：SYN7318_Put_Char----发送一个字节函数
【参数说明】：txd---待发送的字节（8位）
【简    例】：SYN7318_Put_Char('d');  发送‘d’
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
【函 数 名】：SYN7318_Put_String----发送字符串函数
【参数说明】：Pst：存放字符串的数组名
              Length：字符串长度
【简    例】：u8 d[4] = {0x00,0x01,0x02,0x03};
              SYN7318_Put_String(d,4); ---发送数组d中的元素
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
【函 数 名】：SYN7318_Get_char----接收一个字节函数
【参数说明】：无参
【返 回 值】：接收到的字节
【简    例】：u8 d;
              d = SYN7318_Get_char();
              SYN7318_Get_char(d); -----输出接收到的字节
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
【函 数 名】：SYN7318_Get_String----接收字符串函数
【参数说明】：Pst：存放接收到的字符串的数组名
              Length：字符串长度
【简    例】：u8 d[4] = {0};
              SYN7318_Get_String(d,4);
              SYN7318_Get_String(d,4);----输出接收到的字符串 
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
【函 数 名】：SYN_TTS----语音合成播放函数
【参数说明】：Pst：存放要合成播放的文本的数组名
【简    例】：u8 Data[] = {"北京龙邱"};
              SYN_TTS(Data); -----合成播放北京龙邱
*********************************************************************/
void SYN_TTS(u8 *Pst)
{
	u16 Length;
	u8 Frame[5];   //保存发送命令的数组
	
	
	Length = strlen((char*)Pst); 
	Frame[0] = 0xFD;      //帧头
	Frame[1] = 0x00;
	Frame[2] = Length+2;
	Frame[3] = 0x01;      //语音合成播放命令
	Frame[4] = 0x00;      //播放编码格式为“GB2312”
	
    SYN7318_Put_String(Frame, 5);	
	SYN7318_Put_String(Pst, Length);	
	SYN7318_Get_String(Back,4);
	
	//接收成功
	while(!(Back[3] == 0x41))
	{
		SYN7318_Get_String(Back,4);
	}	
	//空闲监测
	SYN7318_Get_String(Back,4);
	while(!(Back[3] == 0x4f))
	{
		SYN7318_Get_String(Back,4);
	}	
}

/*********************************************************************
【函 数 名】：SYN_MP3----MP3播放函数
【参数说明】：Pst：存放要播放的MP3的数组名
【简    例】：u8 D_MP3_1[] = {"D:\\Mp3\\Wifi_首次开机.mp3"};
              SYN_MP3(D_MP3_1); ----播放MP3
*********************************************************************/
void SYN_MP3(u8 *Pst)
{
	u16 Length;
	u8 Frame[5];   //保存发送命令的数组
	
	Length = strlen((char*)Pst); 
	Frame[0] = 0xFD;      //帧头
	Frame[1] = 0x00;
	Frame[2] = Length+2;
	Frame[3] = 0x61;      //MP3播放命令
	Frame[4] = 0x04;      //播放编码格式为“GB2312”
	
  SYN7318_Put_String(Frame, 5);	
	SYN7318_Put_String(Pst, Length);	
	SYN7318_Get_String(Back,0);
//	if(Back[3] == 0x41)
//	   LED2 = 0;
}

/*********************************************************************
【函 数 名】：Start_ASR----开始语音识别函数
【参数说明】：Dict：词典编号
【简    例】：Start_ASR(0x00); ----识别0x00词典中的词条
*********************************************************************/
void Start_ASR(u8 Dict)
{

	u8 Frame[5];   //保存发送命令的数组
	
	Frame[0] = 0xFD;      //帧头
	Frame[1] = 0x00;
	Frame[2] = 0x02;   
	Frame[3] = 0x10;      //开始语音识别命令
	Frame[4] = Dict;      //词典编号  在这里修改想要识别的词典编号
	
	ASR[3]=0;
	
  SYN7318_Put_String(Frame, 5);	
	SYN7318_Get_String(Back,4); 
	if(Back[3] == 0x41)
	{
		 SYN7318_Get_String(Back,3);  //语音识别命令回传结果
		if(Back[0] == 0xfc)
		{
			 SYN7318_Get_String(ASR,Back[2]);
		}
	}
}

/*********************************************************************
【函 数 名】：Stop_ASR----停止语音识别函数
【参数说明】：无参
*********************************************************************/
void Stop_ASR(void)
{
	u8 Frame[4];   //保存发送命令的数组
	
	Frame[0] = 0xFD;      //帧头
	Frame[1] = 0x00;
	Frame[2] = 0x01;   
	Frame[3] = 0x11;      //停止语音识别命令
	
  SYN7318_Put_String(Frame, 4);	
	SYN7318_Get_String(Back,4); 
//	if(Back[3] == 0x41)
//	{
//	   LED8 = 0; //D7亮
//	}
}

/***************************************************************************
【函 数 名】：Dict_TTS----识别词典结果播报函数
【参数说明】：Dict：词典编号
【简    例】：Start_ASR(0x00); ----识别0x00词典中的词条
              Dict_TTS(0x00);------如果识别成功,把识别到的词条播报出来 
***************************************************************************/
void Dict_TTS(u8 Dict)
{
	switch(ASR[0])
	{
		case 0x01:
		case 0x02:
		{
			SYN_TTS(DICT[Dict][ASR[3]]);  //ASR[3]中存放词条ID
		}break;
		case 0x03:
		{
			SYN_TTS("用户静音超时");
		}break;
		case 0x04:
		{
			SYN_TTS("用户语音超时");
		}break;
		case 0x05:
		case 0x07:
		{
			SYN_TTS("识别拒识");
		}break;
		case 0x06:
		{
			SYN_TTS("识别内部错误");
		}break;
	}
}

/*********************************************************************************************************************************************************
【函 数 名】：Start_WakeUp----开始语音唤醒函数
【参数说明】：Wake_ID：唤醒名ID号----0x00：云宝  0x02：小播   0x04：百灵  0x06：叮当管家  0x07：百灵管家  0x08：小播管家  0x09：大管家  0x1F：自定义
【简    例】：Start_WakeUp(0x09);----唤醒名为大管家,唤醒成功之后播报“我在这”
**********************************************************************************************************************************************************/
void Start_WakeUp(u8 Wake_ID)
{
	u8 Frame[5];   //保存发送命令的数组
	
	Frame[0] = 0xFD;      //帧头
	Frame[1] = 0x00;
	Frame[2] = 0x02;   
	Frame[3] = 0x51;      //开始语音唤醒命令
	Frame[4] = Wake_ID;   
	
  SYN7318_Put_String(Frame,5);	
	SYN7318_Get_String(Back,4); 
	if(Back[3] == 0x41)  
	//while(Back[3] == 0x41)
	{
		 SYN7318_Get_String(Back,4);  //语音唤醒命令回传结果
		 if(Back[3] == 0x21)
		 //while(Back[3] == 0x21)
		 {
			 SYN_MP3(D_MP3_2);
		 }
	}
}

/*********************************************************************************************************************************************************
【函 数 名】：Three_One----三合一函数：开启语音唤醒，唤醒后播放提示音，播完后开启语音识别
【参数说明】：Dict：词典编号
              Wake_ID：唤醒名ID号----0x00：云宝  0x02：小播   0x04：百灵  0x06：叮当管家  0x07：百灵管家  0x08：小播管家  0x09：大管家  0x1F：自定义
              Sound：提示音类型  0x00：无提示音  0x01：文本类型  0x02：内置MP3类型
              Pst：提示音的内容
【简    例】：Three_One(0x00, 0x06, 0x02, D_MP3_2); ----唤醒名为叮当管家,唤醒成功后播放“我在这”,之后可以识别0x00词典中的词条 
              Dict_TTS(0x00); ----结合Dict_TTS函数把识别结果播报出来
**********************************************************************************************************************************************************/
void Three_One(u8 Dict, u8 Wake_ID, u8 Sound, u8 *Pst)
{
	u8 Frame[100];   //保存发送命令的数组
	u16 Length,i;
	
	Frame[0] = 0xFD;      //帧头
//	Frame[1] = 0x00;
//	Frame[2] = 0x07;    //由于不确定数据区的长度，所以选择暂时屏蔽
	Frame[3] = 0x15;      //三合一识别命令
	Frame[4] = Dict;      //词典编号
	Frame[5] = Wake_ID;   //唤醒名ID号
	Frame[6] = Sound;     //提示音类型
	if(Sound == 0x00)     //无提示音
	{
		Frame[1] = 0x00;
		Frame[2] = 0x04;
	  SYN7318_Put_String(Frame, 7);	
	}
	else                 //有提示音
	{
		Frame[7] = 0x01;      //文本或内置MP3提示音编码格式：0x00：GB2312编码  0x01：GBK编码  0x02：BIG5编码  0x03：Unicode小头  0x04nicode大头
		Length = strlen((char*)Pst); //文本或内置MP3提示音的长度
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
		SYN7318_Get_String(Back,3);  //语音识别命令回传结果
		if(Back[0] == 0xfc)
		{
			 SYN7318_Get_String(ASR,Back[2]);
			
			 Yu_Yin_Asr();
		}
	}
	
}

/***************************************************************************
【函 数 名】：Status_Query----模块状态查询函数
【参数说明】：无参
【简    例】：Status_Query(); 
***************************************************************************/
void Status_Query(void)
{
	u8 Frame[4];   //保存发送命令的数组
	
	Frame[0] = 0xFD;    //帧头
	Frame[1] = 0x00;
	Frame[2] = 0x01;   
	Frame[3] = 0x21;   //状态查询命令
	
    SYN7318_Put_String(Frame, 4);	
	SYN7318_Get_String(Back,4); 
	if(Back[3] == 0x41)  
	{
		 SYN7318_Get_String(S,4);  //模块当前工作状态的回传结果
	}
}
/*
<------   接收成功             0xFC 0x0 0x1 0x41 
<------   识别拒识             0xFC 0x0 0x1 0x7 

三合一

<------   识别成功,匹配度=9,词条ID=16,命令ID=117    0xFC 0x0 0x6 0x1 0x9 0x0 0x10 0x0 0x75 
<------   0xFD,0x00,0x21,0x15,0x03,0x04,0x02,0x01,0x44,0x3A,0x5C,0x4D,0x70,0x33,0x5C,0x50,0x72,0x6F,0x6D,0x70,0x74,0x5F,0xD6,0xF7,0xC8,0xCB,0xC7,0xEB,0xB7,0xD4,0xB8,0xC0,0x2E,0x6D,0x70,0x33
          帧头   长度  命令字 词典 唤醒 内置 GBK  内容--->

唤醒

【开启唤醒】: len= 5   FD 00 02 51 04 
<------   接收成功     0xFC 0x0 0x1 0x41 

【语音合成播放】: len= 22   FD 00 13 01 01 5B 6D 35 35 5D A1 B0 B0 D9 C1 E9 A1 B1 5B 6D 33 5D 
<------   唤醒成功          0xFC 0x0 0x1 0x21 
<------   接收成功          0xFC 0x0 0x1 0x41 
<------   模块空闲          0xFC 0x0 0x1 0x4F 
*/

#define SYN_RST_MODE   1  // 0 不复位   1 开启复位

void SYN7318_Test( void)  // 开启语音测试
{ 
	//unsigned int i = 0;
   
	while(Ysn7813_flag_cs)
	{
		
	 SYN7318_Init();		
	 SYN_TTS("语音识别测试,请发语音唤醒词，语音驾驶");
	 LED2 = !LED2;
	 Status_Query();  //查询模块当前的工作状态		
	 if(S[3] == 0x4F)  //模块空闲即开启唤醒
	 {

		Delay1ms();
		SYN7318_Put_String(Wake_Up,5);//发送唤醒指令
		SYN7318_Get_String(Back,4);   //接收反馈信息
		if(Back[3] == 0x41)           //接收成功
		{  
		while(Ysn7813_flag_cs)
		{			
			delay_ms(1000);
			delay_ms(1000);
		  full_Speech("语音驾驶",8);
			
			//BEEP = 0;
			delay_ms(1000);delay_ms(1000);
			//BEEP = 1;
		  SYN7318_Get_String(Back,3); //接收前三位回传数据
			BEEP = 0;
			
		  if(Back[0] == 0xfc)       //帧头判断
		  {
			
			SYN7318_Get_String(ASR,Back[2]);//接收回传数据
			if(ASR[0] == 0x21)              //唤醒成功
			{
				SYN7318_Put_String(Play_MP3,33);//播报已经启动语音驾驶，请发出指令

				SYN7318_Get_String(Back,4);
				SYN7318_Get_String(Back,4);
				
				while(!(Back[3] == 0x4f));    //等待空闲
				{
					LED2 = ~LED2;
					Delay500ms();
				}			
					//开始语音识别
					while(Ysn7813_flag)
					{									
						SYN7318_Put_String(Start_ASR_Buf,5);//发语音识别命令					
						SYN7318_Get_String(Back,4);        //接收反馈信息
						if(Back[3] == 0x41)               //接收成功
						{
							LED1 = ~LED1;             //LED1反转
							full_Speech("小车左转",8);
							SYN7318_Get_String(Back,3);  //语音识别命令回传结果
							if(Back[0] == 0xfc)        //帧头判断
							{
								LED2 = ~LED2;
								SYN7318_Get_String(ASR,Back[2]);//接收回传数据
								Yu_Yin_Asr();
							}
						}
								
					}
					SYN7318_Put_String(Stop_Wake_Up,4);//发送停止唤醒指令
			}
			else                          //唤醒内部错误
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


void Yu_Yin_Asr( void)  // 语音识别处理函数
{
			switch(ASR[0])
			{
				case 0x01:
				{
					switch(ASR[3])
					{
	                    case 0x1A:  // 小车前进
						{
							Go_Test( 80,10);
							
							SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
							break;
						}
						case 0x1B:   //  小车后退
						{
							SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别 
							Back_Test(80,10);
								
							break;
						}
						case 0x1C:   //  小车左转
						{							  							  
							Full_Left(90);
							Ysn7813_flag = 0;
							Ysn7813_flag_cs = 0;
							SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别;
							break;
						}
						case 0x1D:  // 小车右转
						{														  
							Full_Right(90);
							Ysn7813_flag = 0;
							Ysn7813_flag_cs = 0;
							SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
							break;
						}
						case 0x1E:  // 小车停止
						{														
							Stop_Test();
							
							SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
							break;
						}
						case 0x1F:   //  小车循迹
						{														
							Track_Test( 50);
							
							SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
							break;
						}	                      
						default:
						{					  
							SYN_TTS("对不起，我没听清");
							SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
							break;
						}
					}
						SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
					break;
				}
				case 0x02: //识别成功（无命令ID号）
				{
					SYN_TTS("对不起，我没听清");
					SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
					break;
				}
				case 0x03://用户静音超时
				{
					SYN_TTS("进入休眠，随时为您服务");
//					Ysn7813_flag = 0;
//					Ysn7813_flag_cs = 0;
					SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
					break;
				}
				case 0x04:
				{
					SYN_TTS("请安静，麻烦您再说一遍");
					SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
					break;
				}
				case 0x05:
				{
					SYN_TTS("对不起，请再说一遍");
					SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
					break;
				}
				case 0x06:
				{
					SYN_TTS("识别内部错误");
					SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
					break;
				}
				case 0x07:
				{
					SYN_TTS("对不起，请再说一遍");
					SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
					break;
				}
				default:
				{
					SYN_TTS("错误");
					SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
					break;
				}
		}

}


/****************************************************************
函数功能：全自动语音识别
参	  数: mode =  1 随机指令  mode != 1 指定命令  num = 指定播报指令
返回  值：无
*****************************************************************/
void SYN_7318_One_test(u8 mode,u8 num)  // mode 模式测试 1 随机指令 0 指定指令
{
  u8 error_num=0;
    SYN_TTS("开始识别！");
	Ysn7813_flag =1;
	while(Ysn7813_flag)
	{								
		SYN7318_Init();
		Status_Query();   //查询模块当前的工作状态	
		if(S[3] != 0x4F)  //模块空闲即开启唤醒  
		{ 
			 SYN_TTS("模块忙碌，暂时无法为您服务，正在复位！");
			 while( !SYN7318_Rst());
			 SYN7318_Init();
			 SYN_TTS("模块复位成功！");
		}
		delay_ms(500);
		SYN7318_Put_String(Start_ASR_One_Buf,5);//发语音识别命令  // 用户词典 2  Start_ASR_One_Buf
								
		SYN7318_Get_String(Back,4);     //接收反馈信息
		if(Back[3] == 0x41)           //接收成功
		{
			LED1 = ~LED1;             //LED1反转
			delay_ms(1000);
			if(mode) 
			{
				YY_ZZW[3] = num;
				Send_ZigbeeData_To_Fifo( YY_SJ,8); // 随机语音
			}
			else  
			{
				YY_ZZW[3] = num;
				Send_ZigbeeData_To_Fifo( YY_ZZW,8); // 指定语音
			}
				
			SYN7318_Get_String(Back,3);  //语音识别命令回传结果
			if(Back[0] == 0xfc)        //帧头判断
			{
				LED2 = ~LED2;
				SYN7318_Get_String(ASR,Back[2]);//接收回传数据
                // Yu_Yin_Asr();

				if(ASR[0] ==0x02&&ASR[1] !=0x00&&ASR[2]==0) 
				{
					switch(ASR[3]) // 命令ID
					{
						case 0x00:   //二七广场站  
					  {
							  SYN_TTS("二七广场站，停在车库A");
							  SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
							  Car_Position_news[1] = 0x01;
							  Ysn7813_flag =0;
							  
							 break;
						}														
						case 0x01:   //  郑州东站
						{								 
							 SYN_TTS("郑州东站，停在车库B");						  
							 SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
							 Car_Position_news[1] = 0x02;
							 Ysn7813_flag =0;
							 break;
						}														
                        case 0x02:   //  人民公园站
						{														  															
							SYN_TTS("人民公园站，停在车库C");
							
                            SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
							Car_Position_news[1] = 0x03;
							Ysn7813_flag =0;
							break;
						}
                        case 0x03:   //体育中心站
						{
							SYN_TTS("体育中心站，停在车库D");
                            SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
							Car_Position_news[1] = 0x04;
							Ysn7813_flag =0;
							break;
						}														
                        case 0x04:   //青少年宫站
						{														  
							
							SYN_TTS("青少年宫站，停在车库E");
                            SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
							Car_Position_news[1] = 0x05;
							Ysn7813_flag =0;
							break;													
						}
						case 0x05:  //市民之家
						{														  
							
							SYN_TTS("市民之家站，停在车库F");
                            SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
							Car_Position_news[1] = 0x06;
							Ysn7813_flag =0;
							break;													
						}
						default:
						{														  
							SYN_TTS("没有该命令！");
							SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
							//Ysn7813_flag =0;
							break;
						}
					}
				}
				else if(ASR[0] ==0x03)
				{
				   SYN_TTS("语音识别超时！超时");
				   SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别				
				}
				else if(ASR[0] ==0x07)
				{
					SYN_TTS("语音识别被拒！被拒");
					SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
				}
				else
				{
				   SYN_TTS("语音识别错误，请重试");
				   SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
				}
					
                 
			}
		}			
		/*************************************************/
		else
		{
				SYN_TTS("语音，开启识别失败！");
			    SYN7318_Put_String(Stop_ASR_Buf,4); //停止语音识别
		}			
		delay_ms(1000);
        error_num++;	
        if( error_num >3)  	Ysn7813_flag =0;	
	}		
}




void Three_One_Test(u8 mode) // 三合一测试  // 0单次识别   1多次识别
{
 
	SYN7318_Init();
	SYN_TTS("智能语音三合一[=yi1]测试，请发唤醒词 语音驾驶");  
	if(!mode)  // 单次识别
 {   
  	
	Three_One(0x03, 0x1F, 0x01, "模块唤醒成功，请发语音指令");
	SYN7318_Put_String(Stop_Three_One_ASR_Buf,4); //停止三合一识别
	
	SYN7318_Get_String(Back,4);
	
	//接收成功
	while(!(Back[3] == 0x41))
	{
		SYN7318_Get_String(Back,4);
	}	
			  
	 SYN_TTS("本次三合一[=yi1]测试完成");  
 }
 
 else 
 {
     while(Ysn7813_flag )
		 {
		    Three_One(0x03, 0x1F, 0x01, "模块唤醒成功，请发语音指令");
	      SYN7318_Put_String(Stop_Three_One_ASR_Buf,4); //停止三合一识别
		     
			  SYN7318_Get_String(Back,4);
	
	    //接收成功
	    while(!(Back[3] == 0x41))
	    {
		    SYN7318_Get_String(Back,4);
	    }	
			 
      if(  (ASR[0]==0x01)&& (ASR[3]==0x19))		// 识别退出命令
			{
			     SYN_TTS("可按下按键S3再次进入测试"); 
			
			}		
      else 			
				SYN_TTS("请再次唤醒模块 语音驾驶");  
		 }
// 
 
 
 }

}
