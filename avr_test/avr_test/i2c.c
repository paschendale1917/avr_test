#include "i2c.h"

uint8_t scanned_addr[10]={0,};

void i2c_init(void){
	WRITE_REG(TWBR, F_CPU/F_I2C/2 - 8);
}

uint8_t start_cond(void){
	uint8_t status=0;
	SET_BIT(TWCR,(1<<TWINT)|(1<<TWSTA)|(1<<TWEN));									//������� ������
	while(!READ_BIT(TWCR,(1<<TWINT))){												//������� ��������� ����� TWINT, ��� ����� �������� ������ ��������
		//	 while_delay=adc_conv;
		//	if(adc_conv-while_delay>30) return START_ERROR;                         //���� �� ����� �������� ��������� �����, �� ������� �� �������(����� �� �����)
	};
	status=TWSR&0xF8;
	if ((status == 0x08)|(status == 0x10))											//�������� ����� ������ ��� �������� �������� ������� TWSR
	return START_ERROR;
	else
	return SUCCESS;
	
}

uint8_t i2c_sendbyte(uint8_t byte){
	WRITE_REG(TWDR,byte);
	WRITE_REG(TWCR,(1<<TWINT) | (1<<TWEN));											//��������� ��������
	while(!READ_BIT(TWCR,(1<<TWINT))){												//������� ��������� ����� TWINT, ��� ����� �������� ������ ��������
		//	 while_delay=adc_conv;
		//	if(adc_conv-while_delay>30) return SEND_ERROR;
	};
	uint8_t status = TWSR & 0xF8;													// ���������� ���� �������
	if ((status == 0x18) | (status == 0x28) | (status == 0x40))						// �������� ���� ������� (�������� SLA+W, ������ ������, SLA+R)
	return ACK;
	else
	return NACK;
}


void stop_cond(void){
	SET_BIT(TWCR,(1<<TWINT)|(1<<TWSTO)|(1<<TWEN));
}

uint8_t i2c_readbyte(uint8_t nack_status){											//nack_status ����������, ��� ����� ���������� ������ ����� ������ �����-nack ��� ack
	if (nack_status == ACK)
	MODIFY_REG(TWCR,0xC4,(1<<TWINT|1<<TWEN|1<<TWEA));
	else
	MODIFY_REG(TWCR,0xC4,(1<<TWINT|1<<TWEN));										//��������� ���� � �������� NACK(���� ������� ��������� ���� � ����� �� ������� ��� NACK, �� �� ��������� ���������� ����� � ���������� ������ � ����� ������ ����������)
	while(!READ_BIT(TWCR,(1<<TWINT))){												//������� ��������� ����� TWINT, ��� ����� �������� ������ ��������
		//	while_delay=adc_conv;
		//	if(adc_conv-while_delay>30) return SEND_ERROR;                          //���� �� ����� �������� ��������� �����, �� ������� �� �������
	};
	return TWDR;
}

void i2c_scanner(uint8_t *scans){
	uint8_t cntr=0;
	for(uint8_t i=0;i<255;i++){
		start_cond();
		uint8_t answ=i2c_sendbyte(i);
		if(answ==ACK){
			i2c_readbyte(NACK);														//������ ���������� ��������� NACK, ����� �� �� ������ �����
			stop_cond();
			scans[cntr++]=i;
			print_hexnumber(i);
			print_string("\r\n");
			//_delay_ms(1000);
		}else
		 stop_cond();
	}
}