
#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "sys.h"
#include "delay.h"		
#include "csb.h"	
#include "test.h"
// ���� 
u32 status=0;						// ����ֵ
u8 tx=0,txr=0;
float  real_time;	 		 // ����ֵ

u16	dis;						   // �������ֵ

/***************************************************************
** ���ܣ�     ���㳬��������þ���
** ������	  �޲���
** ����ֵ��    ��
****************************************************************/ 	
void jscsb()
{
   real_time = status ;
	
   real_time=(float)real_time*1.72-20;     // �������	��ʱ10us��S=Vt/2����20��������
   dis=(u16)real_time;

}

/***************************************************************
** ���ܣ�     �ⲿ�ж�4������� ʹ�����ж�9
** ������	  �޲���
** ����ֵ��    ��
****************************************************************/


void EXTI9_5_IRQHandler(void)
{
	
	if(INT0==0)	
	{
		
	  TIM3->CR1&=~(0x01);                   // ��ʹ�ܶ�ʱ��3
	  jscsb();
     
	}								 
	EXTI->PR=1<<7;                       // ���LINE4�ϵ��жϱ�־λ
}







#if SPPED_INT_MODE
/***************************************************************
** ���ܣ�     �ⲿ�ж�15-10������� ʹ�����ж�10
** ������	  �޲���
** ����ֵ��    ��
****************************************************************/
void EXTI15_10_IRQHandler(void)
{ 
	  if(SPEED==0)
	  {
	    if(tx>2)
		{
		  tx=0;
		  CodedDisk++;
	      MP++;
		}
	  	else tx++; 
	  }
	   
	   EXTI->PR=1<<10;  //���LINE10�ϵ��жϱ�־λ 
}

#else 

/***************************************************************
** ���ܣ�     �ⲿ�ж�0������� ʹ�����ж�0
** ������	  �޲���
** ����ֵ��    ��
****************************************************************/
void EXTI0_IRQHandler(void)
{ 
	  EXTI->PR=1<<0;  //���LINE0�ϵ��жϱ�־λ

	  if(SPEED_R==0)
	  {
	    if(txr>2)
			{
				txr=0;
				CodedDisk++;
					MP++;
			}
	  	else txr++; 
	  }
	 
}

#endif


/***************************************************************
** ���ܣ�     �ⲿ�жϳ�ʼ��
** ������	  �޲���
** ����ֵ��    ��
****************************************************************/
void EXTIX_Init(void)
{

	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|INC_APB|INT0_APB,ENABLE);//ʹ��PORTA,PORTEʱ��

	GPIOB->CRL&=0XFFFFFFF0;  // PB0 ���ó�����	  
	GPIOB->CRL|=0X00000008;
	GPIOB->ODR|=1<<0; 
	
	GPIOB->CRH&=0XFFFFF0FF;  // PB10���ó�����	  
	GPIOB->CRH|=0X00000800;
	GPIOB->ODR|=1<<10;   

	

	GPIO_InitStructure.GPIO_Pin  = INT0_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(INT0_GPIO, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);


    GPIO_EXTILineConfig(INT0_PORTSOURCE,INT0_PINSOURCE);
 	EXTI_InitStructure.EXTI_Line= INT0_LINE;	//KEY2
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource10); 
	EXTI_InitStructure.EXTI_Line=EXTI_Line10;
    EXTI_Init(&EXTI_InitStructure);
	
    GPIO_InitStructure.GPIO_Pin  =  INC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //���ó���������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(INC_GPIO, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4

	NVIC_InitStructure.NVIC_IRQChannel = INT0_IRQ;			//ʹ�ܰ���WK_UP���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//�����ȼ�3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 




#if SPPED_INT_MODE

 //GPIOB10	  �ж����Լ��жϳ�ʼ������ �½��ش��� 
    
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource10);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line10;
  	EXTI_Init(&EXTI_InitStructure);	  	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
		
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ��� 2
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);
		
#else

//GPIOB0	  �ж����Լ��жϳ�ʼ������ �½��ش��� 
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  	EXTI_Init(&EXTI_InitStructure);	  	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//�����ȼ�1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);
		
#endif


	
}

/***************************************************************
** ���ܣ�     ��ʱ��3�жϷ�����
** ������	  �޲���
** ����ֵ��    ��
****************************************************************/
void TIM3_IRQHandler(void)
{ 	    		  			    
	if(TIM3->SR&0X0001)         // ����ж�
	{	 
	 status++; 	

  }
		 					   
	TIM3->SR&=~(1<<0);          // ����жϱ�־λ 	    
}

/***************************************************************
** ���ܣ�     ��ʱ��3�жϳ�ʼ��
** ������	  arr���Զ���װֵ
      		  psc��ʱ��Ԥ��Ƶ��
** ����ֵ��    ��
****************************************************************/
void Timer3_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�
  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ��� 2
	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
	
	TIM_Cmd(TIM3, DISABLE);  //ʹ��TIMx		ENABLE
}
/***************************************************************
** ���ܣ�     ��������������
** ������	  �޲���
** ����ֵ��    ��
****************************************************************/ 
void tran(void)
{
  
    CSB_TX = 1;            
	delay_us(3) ;
    CSB_TX = 0;

	TIM3->CR1|=0x01;         // ʹ�ܶ�ʱ��3
    TIM3->SR&=~(1<<0);       // ����жϱ�־λ    
    status  = 0;			 // ��ʱ������

	delay_ms(30) ;			 //�ȴ�һ��ʱ�䣬�ȴ����ͳ����������ź�
	CSB_TX = 1;
    delay_ms(5);

}
