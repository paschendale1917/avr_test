#ifndef PWM_H_
#define PWM_H_


#define F_CPU			 16000000UL
#define DIVIDER			 256						 //��������  16000000/256=62500 ����� � �������
#define F_PWM			 1000						 //������ ������� ���
#define FREQ_VALUE       F_CPU/DIVIDER/F_PWM         //������� ���. ���������� ���� ����� �������� ���������� ������!


#include <avr/io.h>
#include <avr/interrupt.h>
#include "macros.h"


#define RESET_COM_A			0xC0
#define RESET_COM_B         0x30  
#define RESET_DIVIDER       0x07


void tim1A_init(void);
void pwm1A_start(uint8_t duty);
void pwm1A_stop(void);

#endif /* PWM_H_ */