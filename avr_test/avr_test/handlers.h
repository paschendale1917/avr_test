#ifndef HANDLERS_H_
#define HANDLERS_H_

#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "encoder.h"
#include "st7735.h"
#include "uart.h"
#include "ADC.h"
#include "bme280.h"
#include "PWM.h"
#include "mpu6050.h"
#include "menu.h"

void test_handler(void);		//бан¤ по-черному, где испытываетс¤ вс¤ческое(оставить потом дл¤ примеров использовани¤)
void enc_process(void);			//обработка средств ввода(конкретно здесь:вращение ручки энкодера, длинное и короткое нажати¤ кнопки энкодера и отдельной кнопки)
void hardware_init(void);		//инициализаци¤ "железа", используемого в проекте


#endif /* HANDLERS_H_ */