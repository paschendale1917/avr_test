#include "eeprom.h"

//пример из даташита
void EEPROM_write_byte(uint16_t addr, uint8_t byte){
	while(READ_BIT(EECR, (1<<EEPE)));								//ожидание окончания предыдущей сессии записи
	cli();															//запрещаем прерывания глобально
	WRITE_REG(EEAR,addr);											//кладем адрес ячейки данных в регистр EEAR - регистр адреса ячеки
	WRITE_REG(EEDR,byte);											//кладем байт данных в регистр данных
	SET_BIT(EECR,(1<<EEMPE));										//разрешаем запись в ячейку
	SET_BIT(EECR,(1<<EEPE));										//стартуем начало записи
	while(READ_BIT(EECR, (1<<EEPE)));
	sei();
}

uint8_t EEPROM_read_byte(uint16_t addr){
	uint8_t sreg_backup = SREG;
	cli();
	while(READ_BIT(EECR, (1<<EEPE)));								//ожидание окончания предыдущей сессии записи
	WRITE_REG(EEAR,addr);										//кладем адрес ячейки данных в регистр EEAR - регистр адреса ячеки;
	SET_BIT(EECR,(1<<EERE));										//разрешаем чтение данных из еепром
	SREG=sreg_backup;
	sei();
	return EEDR;													//читаем байт	
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