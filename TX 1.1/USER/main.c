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
int TIM2_flag=0;
/*
TIM2ʱ�ӳ�ʼ��:50ms�������λ���ڣ�
*/
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

    TIM_TimeBaseStructure.TIM_Period=100;		 							
    TIM_TimeBaseStructure.TIM_Prescaler= 36000;				   
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 	
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);							    		
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);
	
	printf("��λ��������\t\t���\r\n");
																
}
/*
TIM3ʱ�ӳ�ʼ��:50ms������Զ��ط���ʱ��ʱ��
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

    TIM_TimeBaseStructure.TIM_Period=50000;		 							
    TIM_TimeBaseStructure.TIM_Prescaler=72;				   
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 	
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	printf("�Զ��ط�����\t\t���\r\n");
	printf("���ͼ������\t\t���\r\n");															
}
/*
TIM4ʱ�ӳ�ʼ��:1ms��� �����ڼ�����
*/
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
�ⲿ�жϳ�ʼ����ʹ��PA1�������ش����ж�
*/
void EXTI_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                          
    NVIC_InitStructure.NVIC_IRQChannel=EXTI1_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        
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

/*
 * ��������main
 * ����  ��������
 * ����  ����
 * ���  ����
 */
int main(void)
{

	SystemInit();
	USART1_init();// USART1��ʼ��,������115200������8���أ�����żУ�飬1ֹͣλ��������λ���·�����
	SPI1_init()	;//SPI��ʼ����������DW1000ͨ��
	DW1000_init();//DW1000��ʼ�����Ĵ���������
	TIM2_init(); //TIM2ʱ�ӳ�ʼ��:20ms�������λ���ڣ�
	TIM3_init(); //TIM3ʱ�ӳ�ʼ��:1ms������Զ��ط���ʱ��ʱ��
	EXTI_init();  //�ⲿ�жϳ�ʼ����ʹ��PA1�������ش����ж�
	TIM4_init(); //TIM4ʱ�ӳ�ʼ��:1ms��� �����ڼ�����
	//RX_mode_enable();
	printf("**********************************\r\n");
	
   
	while (1)
	{
		;      
	}
}



