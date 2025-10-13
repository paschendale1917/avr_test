#ifndef PWM_H_
#define PWM_H_


#define F_CPU			 16000000UL
#define DIVIDER			 256						 //делитель  16000000/256=62500 тиков в секунду
#define F_PWM			 1000						 //задаем частоту ШИМ
#define FREQ_VALUE       F_CPU/DIVIDER/F_PWM         //частота ШИМ. скважность выше этого значения установить нельзя!


#include <avr/io.h>
#include <avr/interrupt.h>
#include "macros.h"


#define RESET_COM_A			0xC0
#define RESET_COM_B         0x30  
#define RESET_DIVIDER       0x07
#define BACKLIGHT_CELL      30


void tim1A_init(void);
void pwm1A_start(uint8_t duty);
void pwm1A_stop(void);

#endif /* PWM_H_ */