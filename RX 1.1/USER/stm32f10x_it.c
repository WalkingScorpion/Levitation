/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "SPI.h"
#include "USART.h"
#include "DW1000.h"
#include <stdio.h>
void TIMx_bezero(TIM_TypeDef* TIMx);
void TIMx_stop(TIM_TypeDef* TIMx);
void TIMx_work(TIM_TypeDef* TIMx);
u8 distance_flag=0;
extern u8 usart_buffer[64];
extern u8 usart_index;
extern u8 usart_status;
extern u8 Location_Number;
u8 ars_counter;
u8 Receive_buffer[14];
u8 tx_power=0x1f;
u8 tx_power_flag=0;
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 
  void TIM4_IRQHandler(void)
{
	if ( TIM_GetITStatus(TIM4 , TIM_IT_Update) != RESET ) 
	{	
		TIM_ClearITPendingBit(TIM4 , TIM_FLAG_Update);
		usart_status=2;
		TIMx_stop(TIM4);

		usart_handle();
		  		
	}		 
}
 void EXTI1_IRQHandler(void)
{
	u32 status;
	u8 tmp;	
	EXTI_ClearITPendingBit(EXTI_Line1);
	while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)==0)
	{	 
		Read_DW1000(0x0F,0x00,(u8 *)(&status),4);
		if((status&0x00006000)==0x00002000)
		{
			tmp=0x20;
			Write_DW1000(0x0F,0x01,&tmp,1);
			to_IDLE();
			if(tx_power_flag==1)
			{
				tx_power_flag=0;
				Write_DW1000(0x1E,0x01,&tx_power,1);
				Write_DW1000(0x1E,0x02,&tx_power,1);
				printf("\r\n*发射功率设置成功*\r\n");
				printf("[发射功率设置为%3.1lfdB]\r\n",1.0*(18-(tx_power>>5)*3+(tx_power&0x1f)*0.5));
			 }
		}

		Read_DW1000(0x0F,0x00,(u8 *)(&status),4);
		if((status&0x00000080)==0x00000080)
		{
			tmp=0xf0;
			Write_DW1000(0x0F,0x00,&tmp,1);
			if(distance_flag==1)
			{
				distance_flag=2;	//定位应答发送成功

				TIMx_bezero(TIM3);
				data_response();
			}
			else if(distance_flag==2)
			{ 
				distance_flag=3;	 //定位数据发送成功
				TIMx_bezero(TIM3);
			}
		}
		
		Read_DW1000(0x0F,0x00,(u8 *)(&status),4);			  
		if((status&0x00004000)==0x00004000)
		{
			tmp=0x6f;
			Write_DW1000(0x0F,0x01,&tmp,1);
			Read_DW1000(0x11,0x00,Receive_buffer,14);

			if(((Receive_buffer[0]&0x07)==0x04)) //如果是ACK
			{
				if (distance_flag==3)
				{
					if(Receive_buffer[2]==Location_Number)
					{
						TIMx_stop(TIM3);
						distance_flag=0; //对端已接收
						ars_counter=0;
						printf("定位数据\t\t对端已接收\r\n");
						to_IDLE();
						RX_mode_enable();
					}
				}
			}
			else if(((Receive_buffer[0]&0x07)==0x01))//数据
			{				
				if(Receive_buffer[9]==0x38)
				{
					if(distance_flag==0)
					{
						ACK_send();
						//开启计数器TIM3
						Location_Number=Receive_buffer[2];
						distance_flag=1;//收到定位申请
					}
				}
			}
			else
			{
				to_IDLE();
				tmp=0x01;
				Write_DW1000(0x0D,0x01,&tmp,1);	
			}
		
		}
		 
	}
}
//distance_flag=0;对端已接收
//distance_flag=1;收到定位申请
//distance_flag=2;定位应答发送成功
//distance_flag=3;定位数据发送成功
void TIM3_IRQHandler(void)
{
	if ( TIM_GetITStatus(TIM3 , TIM_IT_Update) != RESET ) 
	{	
		TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);
		if(distance_flag==1) 
		{
			printf("定位应答\t\t发送超时\r\n");
			TIMx_stop(TIM3);
			distance_flag=0;
			to_IDLE();
			RX_mode_enable();
		}
		else if(distance_flag==2)  
		{
			TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE);
			TIM_SetCounter(TIM3,0x0000);
			TIM_Cmd(TIM3, DISABLE);
			TIM_ClearFlag(TIM3, TIM_FLAG_Update);
			TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);

			printf("定位数据%d\t\t发送超时\r\n",ars_counter+1);
			distance_flag=0;
			ars_counter=0;
					
		} 
		else if(distance_flag==3) 
		{	
			ars_counter++;	
			if(ars_counter<1)
			{
				data_response()	;
				distance_flag=2;
			}
			else
			{
				TIMx_stop(TIM3);
				distance_flag=0;
				Location_Number++;
				ars_counter=0;	
			}	
		}
  	

		
	}		 
}
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{ 
		if(usart_status==0)	
		{
			usart_status=1;

			TIMx_work(TIM4);

			usart_buffer[usart_index++]=USART1->DR;
			if(usart_index==64)
			{
				usart_index=0;
			}
	    }
		else if(usart_status==1)
		{
			TIMx_bezero(TIM4);

			usart_buffer[usart_index++]=USART1->DR;
			if(usart_index==64)
			{
				usart_index=0;
			}
		}			
	} 
	 
} 
//计数器TIMx清零  
void TIMx_bezero(TIM_TypeDef* TIMx)
{
	TIM_ITConfig(TIMx,TIM_IT_Update,DISABLE);
	TIM_SetCounter(TIMx,0x0000);
	TIM_ClearFlag(TIMx, TIM_FLAG_Update);
	TIM_ITConfig(TIMx,TIM_IT_Update,ENABLE);	
}
//计数器TIMx清零,停止工作
void TIMx_stop(TIM_TypeDef* TIMx)
{
	TIM_ITConfig(TIMx,TIM_IT_Update,DISABLE);
	TIM_SetCounter(TIMx,0x0000);
	TIM_Cmd(TIMx, DISABLE);
	TIM_ClearFlag(TIMx, TIM_FLAG_Update);
	TIM_ITConfig(TIMx,TIM_IT_Update,ENABLE);
}
//开启计数器TIMx
void TIMx_work(TIM_TypeDef* TIMx)
{
	TIM_ITConfig(TIMx,TIM_IT_Update,DISABLE);				    		
	TIM_Cmd(TIMx, ENABLE);
	TIM_ClearFlag(TIMx, TIM_FLAG_Update);
	TIM_ITConfig(TIMx,TIM_IT_Update,ENABLE);
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
