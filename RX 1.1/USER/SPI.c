#include "stm32f10x.h"
#define SPI1_CS_High GPIO_SetBits(GPIOA, GPIO_Pin_3)
#define SPI1_CS_Low GPIO_ResetBits(GPIOA, GPIO_Pin_3) 
/*����SPI1Ϊ˫��ȫ˫��������ģʽ��8bit��ʱ��4��Ƶ*/
void SPI1_init(void)
{
	SPI_InitTypeDef SPI1_conf;
	GPIO_InitTypeDef GPIO_conf;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);  // GPIOA,SPI1,IO����ʱ�Ӵ� 

	GPIO_conf.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7; //MISO,MOSI,SCKΪ�����������
	GPIO_conf.GPIO_Speed=GPIO_Speed_50MHz; 
	GPIO_conf.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_conf);

	GPIO_conf.GPIO_Pin=GPIO_Pin_3; //NSSΪIO�������
	GPIO_conf.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_conf);

	
	///////////////////////����///////////////////////////////////
	SPI1_conf.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
	SPI1_conf.SPI_Mode=SPI_Mode_Master;
	SPI1_conf.SPI_DataSize=SPI_DataSize_8b;
	SPI1_conf.SPI_CPOL=SPI_CPOL_Low;
	SPI1_conf.SPI_CPHA=SPI_CPHA_1Edge;
	SPI1_conf.SPI_NSS=SPI_NSS_Soft;
	SPI1_conf.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_32;
	SPI1_conf.SPI_FirstBit=SPI_FirstBit_MSB;
	SPI1_conf.SPI_CRCPolynomial=7;
	SPI_Init(SPI1,&SPI1_conf); 
	
	SPI_Cmd(SPI1, ENABLE);

	SPI1_CS_High;   //Ƭѡ�ø�
	printf("SPI��ʼ��\t\t���\r\n");
	////////////////////////////////////////////////////////////////
}
/*
ͨ��SPI���ͣ����ҽ��ܵ�һ���ֽڵ�����
data���������ݵĴ��ָ��
����ֵ����ȡ������
*/
u8 SPI_send_and_receive_byte(u8 *data)
{
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1,*data);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(SPI1);
}
/*
��DW1000оƬд������
addr:�Ĵ�����ַ
offset_index:ƫ����
data:Ҫд�����ݵ��׵�ַ
length��д�����ݵĳ���
*/
void Write_DW1000(u8 addr,u16 offset_index,u8 *data,u16 length)
{
	u8 SPI_send_and_receive_byte(u8 *data) ;
	u16 i;
	u8 tmp;
   	SPI1_CS_Low;
	if(offset_index==0x00)
	{
		tmp=addr|0x80;
		SPI_send_and_receive_byte(&tmp);		
	}
	else if(offset_index<=0x007f)
	{
		tmp=addr|0xC0;
		SPI_send_and_receive_byte(&tmp);
		tmp=offset_index;	
		SPI_send_and_receive_byte(&tmp);	
	}
	else 
	{
		tmp=addr|0xC0;
		SPI_send_and_receive_byte(&tmp);
		tmp=offset_index;
		tmp|=0x80;
		SPI_send_and_receive_byte(&tmp);
		tmp=offset_index>>7;	
		SPI_send_and_receive_byte(&tmp);		
	}
	for(i=0;i<length;i++)
	{
		SPI_send_and_receive_byte(data++);	
	}
	SPI1_CS_High;
}
/*
��DW1000оƬ��������
addr:�Ĵ�����ַ
offset_index:ƫ����
data:��Ŷ������ݵ��׵�ַ
length���������ݵĳ���
*/
void Read_DW1000(u8 addr,u16 offset_index,u8 *data,u16 length)
{
u8 SPI_send_and_receive_byte(u8 *data) ;
	u16 i;
	u8 tmp;
   	SPI1_CS_Low;
	if(offset_index==0x00)
	{
		SPI_send_and_receive_byte(&addr);		
	}
	else if(offset_index<=0x007f)
	{
		tmp=addr|0x40;
		SPI_send_and_receive_byte(&tmp);
		tmp=offset_index;	
		SPI_send_and_receive_byte(&tmp);	
	}
	else 
	{
		tmp=addr|0x40;
		SPI_send_and_receive_byte(&tmp);
		tmp=offset_index;
		tmp|=0x80;
		SPI_send_and_receive_byte(&tmp);
		tmp=offset_index>>7;	
		SPI_send_and_receive_byte(&tmp);		
	}
	tmp=0xff;
	for(i=0;i<length;i++)
	{
		*data=SPI_send_and_receive_byte(&tmp);
		data++;	
	}
	SPI1_CS_High;
}
