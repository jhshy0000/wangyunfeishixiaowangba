#ifndef __XJ_H
#define __XJ_H	 
#include "sys.h"
#include "djqd.h"
#include "test.h"

#define TRACK_EN  PAout(0)  // ����ѭ�������ϴ�ʹ�ܶ�

extern u8 gd;

void XJ_Init(void);				//��ʼ��	
void Track(void);			   //ѭ������
void Xj_Track_Test(void);
void Track_Usart2_Close( void);
void Track_Usart2_Open( void);
	 				    
#endif

