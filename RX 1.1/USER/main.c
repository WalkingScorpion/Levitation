/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * �ļ���  ��main.c
 * ����    ��LED��ˮ�ƣ�Ƶ�ʿɵ�����         
 * ʵ��ƽ̨��Ұ��STM32������
 * ��汾  ��ST3.5.0
 *
 * ����    ��wildfire team 
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html
 * �Ա�    ��http://firestm32.taobao.com
**********************************************************************************/
#include "stm32f10x.h"
#include "SPI.h"
#include "USART.h"
#include "DW1000.h"

/*
�ⲿ�жϳ�ʼ����ʹ��PA1�������س����ж�
*/
void EXTI_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                          
    NVIC_InitStructure.NVIC_IRQChannel=EXTI1_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           
    NVIC_Init(&NVIC_InitStructure);

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;                                           
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;          
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;                 
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_SetBits(GPIOA,GPIO_Pin_1);  

	EXTI_ClearITPendingBit(EXTI_Line1);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;                                         
    EXTI_Init(&EXTI_InitStructure);

	printf("�ⲿ�ж�����\t\t���\r\n");
}
void TIM2_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    
	TIM_DeInit(TIM2);

    TIM_TimeBaseStructure.TIM_Period=10000;		 							
    TIM_TimeBaseStructure.TIM_Prescaler=72;				   
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 	
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	printf("��ʱ����\t\t���\r\n");
																
}
/*
TIM3ʱ�ӳ�ʼ��:20ms���
*/
void TIM3_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
    
	TIM_DeInit(TIM3);

    TIM_TimeBaseStructure.TIM_Period=20000;		 							
    TIM_TimeBaseStructure.TIM_Prescaler=72;				   
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 	
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	printf("�Զ��ط�����\t\t���\r\n");
	printf("���ͼ������\t\t���\r\n");															
}
void TIM4_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE);
    
	TIM_DeInit(TIM4);

    TIM_TimeBaseStructure.TIM_Period=10;		 							
    TIM_TimeBaseStructure.TIM_Prescaler=7200;				   
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 	
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	printf("����ģʽ����\t\t���\r\n");
																
}

/*
 * ��������main
 * ����  ��������
 * ����  ����
 * ���  ����
 */
int main(void)
{
	
	SystemInit();
	USART1_init();//USART��ʼ��
	SPI1_init()	;//SPI��ʼ��
	DW1000_init();//DW1000��ʼ��
	TIM3_init(); //��ʱ��3��ʼ��
	EXTI_init();  //�ⲿ�жϳ�ʼ��
	TIM4_init();
	RX_mode_enable();
	printf("RX!\r\n");

	
	
	printf("**********************************\r\n");

	

	while (1)
	{
		;
	}      
	
}



