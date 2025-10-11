#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>
#include "macros.h"

#define SPI_DDR															DDRB
#define SPI_PORT														PORTB

#define BKL																PB1
#define DC															    PB2
#define DATA														    PB3
#define RESET															PB4
#define CLOCK															PB5
#define CS															    PB0

void spi_master_init(void);
void spi_sendbyte(uint8_t byte);


#endif /* SPI_H_ */