/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * 文件名  ：main.c
 * 描述    ：LED流水灯，频率可调……         
 * 实验平台：野火STM32开发板
 * 库版本  ：ST3.5.0
 *
 * 作者    ：wildfire team 
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html
 * 淘宝    ：http://firestm32.taobao.com
**********************************************************************************/
#include "stm32f10x.h"
#include "SPI.h"
#include "USART.h"
#include "DW1000.h"

/*
外部中断初始化，使用PA1，上升沿出发中断
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

	printf("外部中断配置\t\t完成\r\n");
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
	
	printf("延时配置\t\t完成\r\n");
																
}
/*
TIM3时钟初始化:20ms溢出
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
	
	printf("自动重发配置\t\t完成\r\n");
	printf("发送监控配置\t\t完成\r\n");															
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
	
	printf("命令模式启动\t\t完成\r\n");
																
}

/*
 * 函数名：main
 * 描述  ：主函数
 * 输入  ：无
 * 输出  ：无
 */
int main(void)
{
	
	SystemInit();
	USART1_init();//USART初始化
	SPI1_init()	;//SPI初始化
	DW1000_init();//DW1000初始化
	TIM3_init(); //定时器3初始化
	EXTI_init();  //外部中断初始化
	TIM4_init();
	RX_mode_enable();
	printf("RX!\r\n");

	
	
	printf("**********************************\r\n");

	

	while (1)
	{
		;
	}      
	
}



