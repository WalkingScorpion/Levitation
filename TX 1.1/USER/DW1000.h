#define _PAN_ID 0x8974		//���˾�����id
#define _TX_sADDR 0x2014	//���ͷ��̵�ַ
#define _RX_sADDR 0x2015	//���շ��̵�ַ
#define _POLLING_FLAG 0x38  //��λ������֤�ֽ�
#define _WAVE_SPEED 299792458 //��Ų������ٶ�


void DW1000_init(void);
void Location_polling(void);
void RX_mode_enable(void);
void distance_measurement(void);
void quality_measurement(void);
void to_IDLE(void);
void ACK_send(void);
u16 Read_Voltage_and_Temperature(void);
