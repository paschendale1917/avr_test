#include "spi.h"

void spi_master_init(void){	SET_BIT(SPI_DDR,(1<<DATA|1<<CLOCK));	CLEAR_BIT(SPCR,1<<SPR0|1<<SPR1);	SET_BIT(SPSR,1<<SPI2X);      // fck/2  16000000/2=8ÌÃö	SET_BIT(SPCR,(1<<SPE|1<<MSTR)); 	}void spi_sendbyte(uint8_t byte){	SPDR = byte;	while(!READ_BIT(SPSR,(1<<SPIF)));}uint8_t spi_readbyte(void){	spi_sendbyte(0x00);	while(!READ_BIT(SPSR,(1<<SPIF)));	return SPDR;}