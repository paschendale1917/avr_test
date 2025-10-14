
#ifndef ENCODER_H_
#define ENCODER_H_



#include <avr/io.h>
#include <avr/interrupt.h>
#include "ADC.h"
#include "macros.h"


#define ENCODER_DDR         DDRD
#define ENCODER_PORT        PORTD
#define ENCODER_PIN         PIND

#define BUTTON_DDR          DDRD
#define BUTTON_PORT         PORTD
#define BUTTON_PIN		    PIND

#define BUTTON             PD7
#define ENC_DT             PD2
#define ENC_CLK            PD3
#define ENC_BUTTON         PD4

#define LONG_TIMEOUT       150


typedef  enum
{
	ENC_LEFT    		=  0 ,
	BUTTON_UP    		=  1 ,
	BUTTON_DOWN			=  2 ,
	ENC_RIGHT			=  3 ,
	BUTTON_SELECT		=  4 ,
	BUTTON_MENUITEMBACK =  5 ,
	BUTTON_NOTHING		=  255
} Button_TypeDef;

extern volatile uint8_t shortpress_enc_but,
						shortpress_but,
						longpress_enc_but,
						longpress_but;

extern volatile uint8_t	newState,
						oldState,
						upState,
						downState,
						encoder_value,
						buttonState;

extern volatile uint8_t flag;
extern volatile uint8_t adc_conv_flag;
extern volatile uint8_t adc_conv;
extern volatile uint8_t update_bme280_flag;
 

void enc_ports_init(void);
void timer0_init();
uint8_t readButtonState(void);
void resetButton(void);

#endif /* ENCODER_H_ */