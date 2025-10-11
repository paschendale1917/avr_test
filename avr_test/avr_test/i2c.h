#ifndef I2C_H_
#define I2C_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "macros.h"
#include "uart.h"

#define F_CPU 16000000UL
#define F_I2C 400000UL

#define START_ERROR                           0x02
#define SEND_ERROR                            0x03
#define ACK                                   0x04
#define NACK                                  0x05

#define REQUEST_WRITE                         0x00
#define REQUEST_READ                          0x01

extern uint8_t scanned_addr[];

void i2c_init(void);
uint8_t start_cond(void);
uint8_t i2c_sendbyte(uint8_t byte);
uint8_t i2c_readbyte(uint8_t nack_status);
void stop_cond(void);


#endif /* I2C_H_ */