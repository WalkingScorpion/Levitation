#define _PAN_ID 0x8974		//���˾�����id
#define _TX_sADDR 0x2014	//���ͷ��̵�ַ
#define _RX_sADDR 0x2015	//���շ��̵�ַ
#define _POLLING_FLAG 0x38  //��λ������֤�ֽ�

void DW1000_init(void);
void data_response(void);
void RX_mode_enable(void);
void to_IDLE(void);
void ACK_send(void);
