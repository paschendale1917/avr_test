#include "uart.h"
#include <avr/io.h>

void uart_init(void){
	WRITE_REG(UCSR0B ,(1<<TXEN0|1<<RXEN0));							//включаем приемник и передатчик					
	WRITE_REG(UCSR0C,(1<<UCSZ01|1<<UCSZ00));						//асинхронный режим, 1 стоп бит, 8бит, без бита четности
	WRITE_REG(UBRR0L,(uint8_t)(SPEED & 0xFF));
	WRITE_REG(UBRR0H,(uint8_t)(SPEED >> 8));
}

void uart_out(uint8_t data){										// передача байта через UART
	while(!READ_BIT(UCSR0A,(1<<UDRE0)));							// ожидание готовности UART к передаче
	WRITE_REG(UDR0,data);											// запись в регистр UDR байта данных начинает процесс передачи
}

uint8_t uart_in(void){												//прием байта из UART
	while(!READ_BIT(UCSR0A,(1<<RXC0)));								//ожидание прихода байта
	return UDR0;										
}

void print_string(char *string){
	while(*string) uart_out(*string++);
}

void print_number(uint16_t number){
	char num_m[5]={0,};
	sprintf(num_m,"%d", number);
	print_string(num_m);
}

void print_hexnumber(uint16_t number){
	char num_m[5]={0,};
	sprintf(num_m,"%s%x","0x", number);
	print_string(num_m);
}