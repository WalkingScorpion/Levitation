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
#include "DW1000.h"
#include "USART.h"
#include "SPI.h"
void TIMx_bezero(TIM_TypeDef* TIMx);
void TIMx_stop(TIM_TypeDef* TIMx);
void TIMx_work(TIM_TypeDef* TIMx);

u8 distance_flag;
u8 ars_counter;
u8 ars_max=3;

u8 usart_buffer[64];
u8 usart_index;
u8 usart_status;

extern u8 Location_Number;
extern u32 Tx_stp_L;
extern u8 Tx_stp_H;
extern u32 Rx_stp_L;
extern u8 Rx_stp_H;
extern u32 data;
extern u32 tmp1;
extern s32 tmp2;
extern double diff;
extern double distance;
u8 Receive_buffer[14];

extern u16 std_noise;
extern	u16 fp_ampl1;
extern	u16 fp_ampl2;
extern	u16 fp_ampl3;
extern	u16 cir_mxg;
extern	u16 rxpacc;
extern	double fppl;
extern	double rxl;
extern int TIM2_flag;
u8 tx_power=0x1f;
u8 tx_power_flag=0;
u16 vol_and_temp;
extern u32 window_index;
extern u32 window_length;

/*
0:����ɴ���
1�����ڽ���
2������ɽ��գ�û����ɴ���


*/
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
  
/*
״̬ת��˵��
distance_flag=0  ��ʼ״̬
distance_flag=1  ��λ�����ѷ���
distance_flag=2  ACK�Ѿ�����
distance_flag=3  �ڶ��������ѽ���
*/
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
				Write_DW1000(0x1E,0x00,&tx_power,1);
				Write_DW1000(0x1E,0x01,&tx_power,1);
				Write_DW1000(0x1E,0x02,&tx_power,1);
				Write_DW1000(0x1E,0x03,&tx_power,1);
				printf("\r\n*���书�����óɹ�*\r\n");
				printf("[���书������Ϊ%3.1lfdB]\r\n",1.0*(18-(tx_power>>5)*3+(tx_power&0x1f)*0.5));
			 }
		} 

		Read_DW1000(0x0F,0x00,(u8 *)(&status),4);
		if((status&0x00000080)==0x00000080)
		{
			tmp=0x80;
			Write_DW1000(0x0F,0x00,&tmp,1);
			if(distance_flag==0)
			{
   				Read_DW1000(0x17,0x00,(u8 *)(&Tx_stp_L),4);
				Read_DW1000(0x17,0x04,&Tx_stp_H,1);
				distance_flag=1;

				TIMx_bezero(TIM3);
			}
		}
		Read_DW1000(0x0F,0x00,(u8 *)(&status),4);
		if((status&0x00004000)==0x00004000)
		{
			tmp=0x60;
			Write_DW1000(0x0F,0x01,&tmp,1);
			Read_DW1000(0x11,0x00,Receive_buffer,14);
	
			if(((Receive_buffer[0]&0x07)==0x04)) //�����ACK
			{
				
				if(distance_flag==1)
				{
					if(Receive_buffer[2]==Location_Number-1)
					{	 
						Read_DW1000(0x15,0x00,(u8 *)(&Rx_stp_L),4);
						Read_DW1000(0x15,0x04,&Rx_stp_H,1);
						Read_DW1000(0x12,0x00,(u8 *)(&std_noise),2);
						Read_DW1000(0x12,0x02,(u8 *)(&fp_ampl2),2);
						Read_DW1000(0x12,0x04,(u8 *)(&fp_ampl3),2);	
						Read_DW1000(0x12,0x06,(u8 *)(&cir_mxg),2);
						Read_DW1000(0x15,0x07,(u8 *)(&fp_ampl1),2);	
						Read_DW1000(0x10,0x02,(u8 *)(&rxpacc),2);
						distance_flag=2;
					}
					TIMx_bezero(TIM3);
					to_IDLE();	
					RX_mode_enable();
				}
			}
			else if(((Receive_buffer[0]&0x07)==0x01))//����
			{
				if(distance_flag==2)
				{
					 if(Receive_buffer[2]==Location_Number-1)
					 {
						TIMx_stop(TIM3);
						distance_flag=3;
						ars_counter=0;

						Read_DW1000(0x11,0x09,(u8 *)(&data),4);
						Read_DW1000(0x13,0x00,(u8 *)(&tmp1),4);
						Read_DW1000(0x14,0x00,(u8 *)(&tmp2),3);
				
						distance_measurement();
						quality_measurement();
					}
					
				}
				
			}
		}
		
	}

	
}

void TIM2_IRQHandler(void)
{
	
	if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
	{	
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update); 
  		Location_polling();//���붨λ
	}		 
}
void TIM3_IRQHandler(void)
{
	
	if ( TIM_GetITStatus(TIM3 , TIM_IT_Update) != RESET ) 
	{
		
		TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);
		
		if(distance_flag==0) //����ʧ��
		{
			
			printf("��λ����%d\t\t�����쳣\r\n",ars_counter+1);
		}
		else if(distance_flag==1)  //���ͳɹ�.��ACKʧ��  
		{
			printf("��λӦ��%d\t\t�����쳣\r\n",ars_counter+1);	
		}
		else if(distance_flag==2) //ACK���ճɹ��������ݽ���ʧ��  
		{	
			printf("��λ����%d\t\t�����쳣\r\n",ars_counter+1);		
		}
  	
		ars_counter++;
		if(ars_counter<ars_max)
		{
			Location_polling();
		}
		else
		{
			TIMx_stop(TIM3)	;

			ars_counter=0;
			
		}
	}		 
}
void TIM4_IRQHandler(void)
{
	if ( TIM_GetITStatus(TIM4 , TIM_IT_Update) != RESET ) 
	{	
		TIM_ClearITPendingBit(TIM4 , TIM_FLAG_Update);
		usart_status=2;
		TIMx_stop(TIM4)	;

		usart_handle();
		  		
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
			TIMx_bezero(TIM3);

			usart_buffer[usart_index++]=USART1->DR;
			if(usart_index==64)
			{
				usart_index=0;
			}
		}			
	} 
	 
}
//������TIMx����  
void TIMx_bezero(TIM_TypeDef* TIMx)
{
	TIM_ITConfig(TIMx,TIM_IT_Update,DISABLE);
	TIM_SetCounter(TIMx,0x0000);
	TIM_ClearFlag(TIMx, TIM_FLAG_Update);
	TIM_ITConfig(TIMx,TIM_IT_Update,ENABLE);	
}
//������TIMx����,ֹͣ����
void TIMx_stop(TIM_TypeDef* TIMx)
{
	TIM_ITConfig(TIMx,TIM_IT_Update,DISABLE);
	TIM_SetCounter(TIMx,0x0000);
	TIM_Cmd(TIMx, DISABLE);
	TIM_ClearFlag(TIMx, TIM_FLAG_Update);
	TIM_ITConfig(TIMx,TIM_IT_Update,ENABLE);
}
//����������TIMx
void TIMx_work(TIM_TypeDef* TIMx)
{
	TIM_ITConfig(TIMx,TIM_IT_Update,DISABLE);				    		
	TIM_Cmd(TIMx, ENABLE);
	TIM_ClearFlag(TIMx, TIM_FLAG_Update);
	TIM_ITConfig(TIMx,TIM_IT_Update,ENABLE);
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
