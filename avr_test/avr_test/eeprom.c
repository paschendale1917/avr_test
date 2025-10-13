#include "eeprom.h"

//������ �� ��������
void EEPROM_write_byte(uint16_t addr, uint8_t byte){
	while(READ_BIT(EECR, (1<<EEPE)));								//�������� ��������� ���������� ������ ������
	cli();															//��������� ���������� ���������
	WRITE_REG(EEAR,addr);											//������ ����� ������ ������ � ������� EEAR - ������� ������ �����
	WRITE_REG(EEDR,byte);											//������ ���� ������ � ������� ������
	SET_BIT(EECR,(1<<EEMPE));										//��������� ������ � ������
	SET_BIT(EECR,(1<<EEPE));										//�������� ������ ������
	while(READ_BIT(EECR, (1<<EEPE)));
	sei();
}

uint8_t EEPROM_read_byte(uint16_t addr){
	uint8_t sreg_backup = SREG;
	cli();
	while(READ_BIT(EECR, (1<<EEPE)));								//�������� ��������� ���������� ������ ������
	WRITE_REG(EEAR,addr);										//������ ����� ������ ������ � ������� EEAR - ������� ������ �����;
	SET_BIT(EECR,(1<<EERE));										//��������� ������ ������ �� ������
	SREG=sreg_backup;
	sei();
	return EEDR;													//������ ����	
}

void EEPROM_write_data(uint16_t addr, uint8_t *data, uint16_t size_of_data){
	for(uint16_t i=0;i<size_of_data;i++){
		EEPROM_write_byte(addr+i,*(data+i));
	}
}

void EEPROM_read_data(uint16_t addr, uint8_t *data, uint16_t size_of_data){
	for(uint16_t i=0;i<size_of_data;i++){
		*(data+i)=EEPROM_read_byte(addr+i);
	}
}