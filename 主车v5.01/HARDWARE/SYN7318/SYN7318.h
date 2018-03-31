#ifndef __SYN7138_H
#define __SYN7138_H	 
#include "sys.h"



#if SYSTEM_TASK_TYPE==TASK_TYPE1//任务1定义
#define SYN7318_RST_H   (GPIOC->BSRR = GPIO_Pin_9 )
#define SYN7318_RST_L   (GPIOC->BRR  = GPIO_Pin_9 )

#elif SYSTEM_TASK_TYPE==TASK_TYPE0//任务0定义
#define SYN7318_RST_H   (GPIOD->BSRR = GPIO_Pin_4 )
#define SYN7318_RST_L   (GPIOD->BRR  = GPIO_Pin_4 )

#endif

extern void SYN7318_Test( void);  // 开启语音测试
extern u8 Ysn7813_flag;
extern u8 Ysn7813_flag_cs;
void SYN7318_Open(void);
void SYN7318_Close(void);

void SYN7318_Put_Char(u8 txd);
void SYN7318_Put_String(u8* Pst,u16 Length);
u8 SYN7318_Get_char( void) ;
void SYN7318_Get_String(u8 *Pst,u8 Length);
void SYN_TTS(u8 *Pst);
void SYN_MP3(u8 *Pst);
void Start_ASR(u8 Dict);
void Stop_ASR(void);
void Dict_TTS(u8 Dict);
void Start_WakeUp(u8 Wake_ID);
void Three_One(u8 Dict, u8 Wake_ID, u8 Sound, u8 *Pst);
void Status_Query(void);
u8  SYN7318_Rst(void) ;

void SYN_7318_One_test(u8 mode,u8 num);  // mode 模式测试 1 随机指令 0 指定指令
	
extern void SYN_TTS(u8 *Pst);
extern void SYN7318_Init(void);
extern void Yu_Yin_Asr( void);
extern void Three_One_Test(u8 mode) ;

#endif


