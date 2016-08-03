#include "stm32f10x.h"
#include "SPI.h"
#include "USART.h"
#include "DW1000.h"
#include "math.h"
void TIMx_bezero(TIM_TypeDef* TIMx);
void TIMx_stop(TIM_TypeDef* TIMx);
void TIMx_work(TIM_TypeDef* TIMx);
u8 Location_Number=0x00;
extern u8 distance_flag;
u32 time_offset=0; //电磁波传播时间调整
u8 speed_offset=0; //电磁波传播速度调整
u32 Tx_stp_L;
u8 Tx_stp_H;
u32 Rx_stp_L;
u8 Rx_stp_H;
u32 data;
u32 tmp1;
s32 tmp2;
double diff;
double distance;
extern u8 ars_counter;
extern u8 Receive_buffer[14];
u8 Tx_Buff[12];

u16 std_noise;
u16 fp_ampl1;
u16 fp_ampl2;
u16 fp_ampl3;
u16 cir_mxg;
u16 rxpacc;
double fppl;
double rxl;
u32 window_length=20;
u32 window_index=0;

extern u16 vol_and_temp	;

void Tx_init(void)
{
	distance_flag=0;
	//地址：反正！！ （低字节在前 单字节正常写入）
	Tx_Buff[0]=0x41;
	Tx_Buff[1]=0x88;
	Tx_Buff[2]=Location_Number++;
	Tx_Buff[3]=(0x74);//MAC_maker((u8)_PAN_ID);
	Tx_Buff[4]=(0x89);//MAC_maker((u8)(_PAN_ID>>8));
	Tx_Buff[5]=(0x15);//MAC_maker((u8)_RX_sADDR);
	Tx_Buff[6]=(0x20);//MAC_maker((u8)(_RX_sADDR>>8));
	Tx_Buff[7]=0x14;//MAC_maker((u8)_TX_sADDR);
	Tx_Buff[8]=0x20;//MAC_maker((u8)(_TX_sADDR>>8));
	Tx_Buff[9]=_POLLING_FLAG;
	Tx_Buff[10]=0x12;
	Tx_Buff[11]=0x34;
}

/*
DW1000初始化
*/
void DW1000_init(void)
{
	u32 tmp,status,i;
	SPI_InitTypeDef SPI1_conf;

	tmp=0x0f;
	Write_DW1000(0x36,0x01,(u8 *)(&tmp),1);

	
	tmp=0x00000055;
	Write_DW1000(0x23,0x12,(u8 *)(&tmp),2);
	////////////////////工作模式配置////////////////////////
	//AGC_TUNE1	：设置为16 MHz PRF
	tmp=0x00008870;
	Write_DW1000(0x23,0x04,(u8 *)(&tmp),2);
	//AGC_TUNE2	 ：不知道干啥用，技术手册明确规定要写0x2502A907
	tmp=0x2502A907;
	Write_DW1000(0x23,0x0C,(u8 *)(&tmp),4);	
	//DRX_TUNE2：配置为PAC size 8，16 MHz PRF
	tmp=0x311A002D;
	Write_DW1000(0x27,0x08,(u8 *)(&tmp),4);
	//NSTDEV  ：LDE多径干扰消除算法的相关配置
	tmp=0x0000006D;
	Write_DW1000(0x2E,0x0806,(u8 *)(&tmp),1);
	//LDE_CFG2	：将LDE算法配置为适应16MHz PRF环境
	tmp=0x00001607;
	Write_DW1000(0x2E,0x1806,(u8 *)(&tmp),2);
	//TX_POWER	 ：将发送功率配置为16 MHz, 
	tmp=0x1f1f1f1f;
	Write_DW1000(0x1E,0x00,(u8 *)(&tmp),4);
	//RF_TXCTRL	 ：选择发送通道5
	tmp=0x001E3FE0;
	Write_DW1000(0x28,0x0C,(u8 *)(&tmp),4);
	//TC_PGDELAY：脉冲产生延时设置为适应频道5
	tmp=0x000000C0;
	Write_DW1000(0x2A,0x0B,(u8 *)(&tmp),1);
	//FS_PLLTUNE   ：PPL设置为适应频道5
	tmp=0x000000A6;
	Write_DW1000(0x2B,0x0B,(u8 *)(&tmp),1);
	/////////////////////使用功能配置/////////////////////////
	//local address	：写入本机地址（PAN_ID 和本机短地址）
	tmp=_PAN_ID;
	tmp=(tmp<<16)+_TX_sADDR; 
	Write_DW1000(0x03,0x00,(u8 *)(&tmp),4);  	  
	//re-enable	   Frame Filter ：开启接收自动重启功能、自动应答功能、帧过滤功能
	tmp=0x200410FD;
	Write_DW1000(0x04,0x00,(u8 *)(&tmp),4);
	// interrupt   ：中断功能选择
	tmp=0x00806082;
	Write_DW1000(0x0E,0x00,(u8 *)(&tmp),2);
	// ack等待
	tmp=3;
	Write_DW1000(0x1A,0x03,(u8 *)(&tmp),1);
	

	while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)==0)
	{
		Read_DW1000(0x0F,0x00,(u8 *)(&status),4);			  
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
			SPI1_conf.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_8;
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
		//LDE LOAD
		for(i=0;i<20;i++)
		{
			tmp=0x8000;
			Write_DW1000(0x2D,0x06,(u8 *)(&tmp),2);
		}
	}
	printf("定位芯片配置\t\t完成\r\n");	
}
/*
申请定位
*/
void Location_polling(void)
{
	u8 tmp;
									
	Tx_init();
	
	to_IDLE();
	Write_DW1000(0x09,0x00,Tx_Buff,12);
	tmp=14;
	Write_DW1000(0x08,0x00,&tmp,1);
	tmp=0x82;
	Write_DW1000(0x0D,0x00,&tmp,1);

	TIMx_work(TIM3);
	
}
/*
打开接收模式
*/
void RX_mode_enable(void)
{
	u8 tmp;
	tmp=0x01;
	Write_DW1000(0x0D,0x01,&tmp,1);	
}
/*
返回IDLE状态
*/
void to_IDLE(void)
{
	u8 tmp;
	tmp=0x40;
	Write_DW1000(0x0D,0x00,&tmp,1);	
}
/*
计算距离信息(单位：cm)并串口输出
*/
void distance_measurement(void)
{
	
	if(distance_flag!=3)
	{
		printf("测定距离\t\t暂无数据\r\n");		
	}
	else
	{
		if(Tx_stp_H==Rx_stp_H)
		{
			diff=1.0*(Rx_stp_L-Tx_stp_L);
		}
		else if(Tx_stp_H<Rx_stp_H)
		{
			diff=1.0*((Rx_stp_H-Tx_stp_H)*0xFFFFFFFF+Rx_stp_L-Tx_stp_L);
		}
		else
		{
			diff=1.0*((0xFF-Tx_stp_H+Rx_stp_H+1)*0xFFFFFFFF+Rx_stp_L-Tx_stp_L);
		}
		tmp2&=0x0007FFFF;
		tmp2<<=13;
		diff=diff-1.0*data ;
		
		diff-=((double)time_offset);
		if(window_index<window_length)
		{
			if((15.65*diff/1000000000000/2*_WAVE_SPEED*(1.0-0.01*speed_offset)<500)&&(15.65*diff/1000000000000/2*_WAVE_SPEED*(1.0-0.01*speed_offset)>0))
			{
				distance+=15.65*diff/1000000000000/2*_WAVE_SPEED*(1.0-0.01*speed_offset);
				window_index++;
			}
			else
			{
				printf("%d  %d\r\n",Tx_stp_H,Rx_stp_H);	
			}
		}
		if(window_index==window_length)
		{
			//vol_and_temp=Read_Voltage_and_Temperature();
			printf("%.3lf\t0x%02x\t0x%02x\r\n",distance/window_length,(u8)(vol_and_temp>>8),(u8)vol_and_temp);
			distance=0;
			window_index=0;
				
		}
				
	}
}

/*
无线质量数据
*/
void quality_measurement(void)
{
	rxpacc>>=4;

	//抗噪声品质判定
	if((fp_ampl2/std_noise)>=2)
	{
		//printf("抗噪声品质\t\t良好\r\n");
	}
	else
	{
		//printf("抗噪声品质\t\t异常\r\n");
	}
	//LOS判定
	fppl=10.0*log((fp_ampl1^2+fp_ampl2^2+fp_ampl3^2)/(rxpacc^2))-115.72;
	rxl=10.0*log(cir_mxg*(2^17)/(rxpacc^2))-115.72;
	if((fppl-rxl)>=10.0*log(0.25))
	{
		//printf("LOS判定\t\t\tLOS\r\n");
	}
	else
	{
		//printf("LOS判定\t\t\tNLOS\r\n");
	}
}

//读电压与温度值
//返回值：高8位电压值，低8位温度值
u16 Read_Voltage_and_Temperature(void)
{
	u32 tmp;
	u16 result;

	tmp=0x00031780;
	Write_DW1000(0x28,0x00,(u8 *)(&tmp),4);
	tmp=0xffff;
	while(tmp--);

	tmp=0x01;
	Write_DW1000(0x36,0x00,(u8 *)(&tmp),1);
	tmp=0xffff;
	while(tmp--);

	tmp=0x00;
	Write_DW1000(0x36,0x04,(u8 *)(&tmp),1);
	tmp=0xffff;
	while(tmp--);

	tmp=0x0A8063;
	Write_DW1000(0x28,0x10,(u8 *)(&tmp),3);
	tmp=0xffff;
	while(tmp--);

	tmp=0x0F;
	Write_DW1000(0x28,0x12,(u8 *)(&tmp),1);
	tmp=0xffff;
	while(tmp--);

	tmp=0x00;
	Write_DW1000(0x2A,0x01,(u8 *)(&tmp),1);
	tmp=0xffff;
	while(tmp--);

	tmp=0x0301;
	Write_DW1000(0x2A,0x00,(u8 *)(&tmp),2);
	tmp=0xffff;
	while(tmp--);

	tmp=0x00;
	Write_DW1000(0x2A,0x00,(u8 *)(&tmp),1);
	tmp=0xffff;
	while(tmp--);


	Read_DW1000(0x2A,0x03,(u8 *)(&result),1);
	result<<=8;
	Read_DW1000(0x2A,0x04,(u8 *)(&result),1);

	tmp=0x38;
	Write_DW1000(0x36,0x04,(u8 *)(&tmp),1);
	tmp=0xffff;
	while(tmp--);

	tmp=0x00;
	Write_DW1000(0x36,0x00,(u8 *)(&tmp),1);
	tmp=0xffff;
	while(tmp--);


	return result;
}
