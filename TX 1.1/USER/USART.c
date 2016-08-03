#include "stm32f10x.h"
#include "USART.h"
#include "SPI.h"


extern u8 usart_buffer[64];
extern u8 usart_index;
extern u8 usart_status;
extern u8 ars_max;
extern u8 time_offset;
extern u8 speed_offset;
extern u32 window_length;
extern u32 window_index;
extern double distance;
extern u8 tx_power;
extern u8 tx_power_flag;
 
/*
 USART1初始化,波特率115200，单次8比特，无奇偶校验，1停止位
 */
void USART1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	  
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	USART_ClearFlag(USART1,USART_FLAG_TC); 
	
	USART_Cmd(USART1, ENABLE);
	printf("*************系统配置*************\r\n");
	printf("USART初始化\t\t完成\r\n");
}
/*
fputc重定向
*/
int fputc(int ch, FILE *f)
{

	USART_SendData(USART1, (unsigned char) ch);
	while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);	
	return (ch);
}
/*
	命令		命令字			参数1															参数2
设置定位窗口 	0x01		定位窗口（两字节，低位在前，默认20）
设置发送功率  	0x02	发送功率寄存器写入值（单位；两字节，低位在前；33.5dB)		   		
设置时间偏移    0x03    时间差的偏移（从硬件获得数据中减去；4字节，低位在前；默认0）
设置光速偏移    0x04    光速的百分比偏移（单位：1%；从真空光速中以百分比形式减去；默认0）
读取DW1000寄存器 0x05     地址（一字节） 偏移地址（两字节，低位在前） 读取字节长度（两字节，低位在前,最多128）

*/
void usart_handle(void)
{
	u16 i;
	u32 tmp=0;
	u8 tmpp[128];
	if(usart_status==2)
	{
		tmp=0;
		//配置定位周期单位（ms）
		if((usart_buffer[0]==0x01)&&(usart_index==3))
		{
			tmp=usart_buffer[1]+(usart_buffer[2]<<8);
			window_length=tmp;
			window_index=0;
			distance=0;
			printf("\r\n*定位窗口设置成功*\r\n");
			printf("[定位周期设置为%u]\r\n",tmp);
    	}
		else if	(((usart_buffer[0]==0x02)&&(usart_index==2)))
		{
			tx_power=usart_buffer[1];
			tx_power_flag=1;
			
		}
		else if	(((usart_buffer[0]==0x03)&&(usart_index==5)))
		{
			time_offset= usart_buffer[1]+(usart_buffer[2]<<8)+(usart_buffer[3]<<16)+(usart_buffer[4]<<24);
			printf("\r\n*电磁波飞行时间偏移设置成功*\r\n");
			printf("[电磁波飞行时间偏移为0x%x%x%x%x]\r\n",usart_buffer[4],usart_buffer[3],usart_buffer[2],usart_buffer[1]);
		}
		else if	(((usart_buffer[0]==0x04)&&(usart_index==2)))
		{
			speed_offset= usart_buffer[1];
			printf("\r\n*电磁波速度偏移百分比设置成功*\r\n");
		   	printf("[电磁波速度偏移百分比为%u%%]\r\n",speed_offset);
		}
		else if	(((usart_buffer[0]==0x05)&&(usart_index==6)))
		{
			tmp=(usart_buffer[4]+(usart_buffer[5]<<8));
			Read_DW1000(usart_buffer[1],(usart_buffer[2]+(usart_buffer[3]<<8)),tmpp,tmp);
			printf("*访问地址 0x%02x:0x%02x 访问长度 %d *\r\n[返回数据 0x",usart_buffer[1],(usart_buffer[2]+(usart_buffer[3]<<8)),tmp);
			for(i=0;i<tmp;i++)
			{
				printf("%02x",*(tmpp+tmp-i-1));
			}
			printf("]\r\n");
		}
		else
		{
			printf("\r\n*输入错误*\r\n");
		}
		
		usart_status=0;
		usart_index=0;

	}
	
}

