#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stdio.h>
#include "macros.h"

#define F_CPU 16000000UL
#define BAUD  9600UL
#define SPEED ((F_CPU+BAUD*8)/(BAUD*16)-1)

void uart_init(void);
void print_string(char *string);
uint8_t uart_in(void);
void uart_out(uint8_t data);
void print_number(uint16_t number);
void print_hexnumber(uint16_t number);
void i2c_scanner(uint8_t *scans);

#endif /* UART_H_ */