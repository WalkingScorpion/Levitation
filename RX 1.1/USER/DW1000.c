#include "stm32f10x.h"
#include "SPI.h"
#include "DW1000.h"
void TIMx_bezero(TIM_TypeDef* TIMx);
void TIMx_stop(TIM_TypeDef* TIMx);
void TIMx_work(TIM_TypeDef* TIMx);

u8 Location_Number=0x00;
extern u8 distance_flag;
u8 Tx_Buff[13];
u32 Tx_stp_L;
u8 Tx_stp_H;
u32 Rx_stp_L;
u8 Rx_stp_H;
u32 diff;
u8 tmp;
extern u8 Receive_buffer[14];
/*
DW1000��ʼ��
*/
void DW1000_init(void)
{
	u32 tmp,status,i;	
	SPI_InitTypeDef SPI1_conf;
	

	


	tmp=0x00000055;
	Write_DW1000(0x23,0x12,(u8 *)(&tmp),2);

	////////////////////����ģʽ����////////////////////////
	//AGC_TUNE1	������Ϊ16 MHz PRF
	tmp=0x00008870;
	Write_DW1000(0x23,0x04,(u8 *)(&tmp),2);
	//AGC_TUNE2	 ����֪����ɶ�ã������ֲ���ȷ�涨Ҫд0x2502A907
	tmp=0x2502A907;
	Write_DW1000(0x23,0x0C,(u8 *)(&tmp),4);	
	//DRX_TUNE2������ΪPAC size 8��16 MHz PRF
	tmp=0x311A002D;
	Write_DW1000(0x27,0x08,(u8 *)(&tmp),4);
	//NSTDEV  ��LDE�ྶ���������㷨���������
	tmp=0x0000006D;
	Write_DW1000(0x2E,0x0806,(u8 *)(&tmp),1);
	//LDE_CFG2	����LDE�㷨����Ϊ��Ӧ16MHz PRF����
	tmp=0x00001607;
	Write_DW1000(0x2E,0x1806,(u8 *)(&tmp),2);
	//TX_POWER	 �������͹�������Ϊ16 MHz,���ʵ���ģʽ 
	tmp=0x1f1f1f1f;
	Write_DW1000(0x1E,0x00,(u8 *)(&tmp),4);
	//RF_TXCTRL	 ��ѡ����ͨ��5
	tmp=0x001E3FE0;
	Write_DW1000(0x28,0x0C,(u8 *)(&tmp),4);
	//TC_PGDELAY�����������ʱ����Ϊ��ӦƵ��5
	tmp=0x000000C0;
	Write_DW1000(0x2A,0x0B,(u8 *)(&tmp),1);
	//FS_PLLTUNE   ��PPL����Ϊ��ӦƵ��5
	tmp=0x000000A6;
	Write_DW1000(0x2B,0x0B,(u8 *)(&tmp),1);
	/////////////////////ʹ�ù�������/////////////////////////
	//local address	��д�뱾����ַ��PAN_ID �ͱ����̵�ַ��
	tmp=_PAN_ID;
	tmp=(tmp<<16)+_RX_sADDR; 
	Write_DW1000(0x03,0x00,(u8 *)(&tmp),4); 
	
	//re-enable	  auto ack	 Frame Filter �����������Զ��������ܡ��Զ�Ӧ���ܡ�֡���˹���
	tmp=0x200410FD;
	Write_DW1000(0x04,0x00,(u8 *)(&tmp),4);
	
	// interrupt   ���жϹ���ѡ��
	tmp=0x00806082;
	Write_DW1000(0x0E,0x00,(u8 *)(&tmp),4);
	
	// ack�ȴ�
	tmp=3;
	Write_DW1000(0x1A,0x03,(u8 *)(&tmp),1);

	

	while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)==0)
	{
		Read_DW1000(0x0F,0x00,(u8 *)(&status),4);
		printf("0x%08x\r\n",status);			  
		if((status&0x00000002)==0x00000002)
		{
			tmp=0x02;
			Write_DW1000(0x0F,0x00,(u8 *)&tmp,1);
			SPI1_conf.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
			SPI1_conf.SPI_Mode=SPI_Mode_Master;
			SPI1_conf.SPI_DataSize=SPI_DataSize_8b;
			SPI1_conf.SPI_CPOL=SPI_CPOL_Low;
			SPI1_conf.SPI_CPHA=SPI_CPHA_1Edge;
			SPI1_conf.SPI_NSS=SPI_NSS_Soft;
			SPI1_conf.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_4;
			SPI1_conf.SPI_FirstBit=SPI_FirstBit_MSB;
			SPI1_conf.SPI_CRCPolynomial=7;
			SPI_Init(SPI1,&SPI1_conf); 
		} 	

		Read_DW1000(0x0F,0x00,(u8 *)(&status),4);			  
		if((status&0x00800000)==0x00800000)
		{
			tmp=0x80;
			Write_DW1000(0x0F,0x02,(u8 *)&tmp,1);	
		} 

		Read_DW1000(0x0F,0x00,(u8 *)(&status),4);			  
		if((status&0x02000000)==0x02000000)
		{
			tmp=0x02;
			Write_DW1000(0x0F,0x03,(u8 *)&tmp,1);	
		} 
	}	

	printf("��λоƬ����\t\t���\r\n");
   	for (i=0;i<20;i++)
   	{
		//LDE LOAD
		tmp=0x8000;
		Write_DW1000(0x2D,0x06,(u8 *)(&tmp),2);
	}
	
}
/*
���ݻط�
*/
void data_response(void)
{
	

	if(distance_flag==2)
	{
		Read_DW1000(0x17,0x00,(u8 *)(&Tx_stp_L),4);
		Read_DW1000(0x15,0x00,(u8 *)(&Rx_stp_L),4);
		Read_DW1000(0x17,0x04,&Tx_stp_H,1);
		Read_DW1000(0x15,0x04,&Rx_stp_H,1);
	

		if(Tx_stp_H==Rx_stp_H)
		{
			diff=(Tx_stp_L-Rx_stp_L);
		}
		else if(Rx_stp_H<Tx_stp_H)
		{
				diff=((Tx_stp_H-Rx_stp_H)*0xFFFFFFFF+Tx_stp_L-Rx_stp_L);
		}
		else
		{
			diff=((0xFF-Rx_stp_H+Tx_stp_H+1)*0xFFFFFFFF+Tx_stp_L-Rx_stp_L);
		}
	}
	Tx_Buff[0]=0x41;
	Tx_Buff[1]=0x88;
	Tx_Buff[2]=Location_Number;
	Tx_Buff[4]=(u8)(_PAN_ID>>8);
   	Tx_Buff[3]=(u8)_PAN_ID;
	Tx_Buff[6]=(u8)(_TX_sADDR>>8);
	Tx_Buff[5]=(u8)_TX_sADDR;
	Tx_Buff[8]=(u8)(_RX_sADDR>>8);
	Tx_Buff[7]=(u8)_RX_sADDR;
   	Tx_Buff[9]=(u8)diff;
	diff>>=8;
	Tx_Buff[10]=(u8)diff;
	diff>>=8;
	Tx_Buff[11]=(u8)diff;
	diff>>=8;
	Tx_Buff[12]=(u8)diff;

	to_IDLE();
	tmp=15;
	Write_DW1000(0x08,0x00,&tmp,1);
	Write_DW1000(0x09,0x00,Tx_Buff,13);
	tmp=0x82;
	Write_DW1000(0x0D,0x00,&tmp,1);

	TIMx_bezero(TIM3);


}
/*
�򿪽���ģʽ
*/
void RX_mode_enable(void)
{
	u8 tmp;
	tmp=0x01;
	Write_DW1000(0x0D,0x01,&tmp,1);	

}
/*
����IDLE״̬
*/
void to_IDLE(void)
{
	u8 tmp;
	tmp=0x40;
	Write_DW1000(0x0D,0x00,&tmp,1);	
}
void ACK_send(void)
{
	Tx_Buff[0]=0x44;
	Tx_Buff[1]=0x00;
	Tx_Buff[2]=Receive_buffer[2];

	to_IDLE();
	tmp=5;
	Write_DW1000(0x08,0x00,&tmp,1);
	Write_DW1000(0x09,0x00,Tx_Buff,3);
	tmp=0x82;
	Write_DW1000(0x0D,0x00,&tmp,1);
	
	TIMx_work(TIM3);

}
