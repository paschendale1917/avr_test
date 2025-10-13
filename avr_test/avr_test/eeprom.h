#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/io.h>
#include "macros.h"
#include "avr/interrupt.h"
#include <avr/eeprom.h>

void EEPROM_write_byte(uint16_t addr, uint8_t byte);
uint8_t EEPROM_read_byte(uint16_t addr);
void EEPROM_write_data(uint16_t addr, uint8_t *data, uint16_t size_of_data);
void EEPROM_read_data(uint16_t addr, uint8_t *data, uint16_t size_of_data);

#endif /* EEPROM_H_ */