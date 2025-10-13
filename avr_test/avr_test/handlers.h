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

#define MENU_PROCESS			0xFF

extern uint8_t menustate;				//в этой переменной будем сохранять текущее состояние меню
extern uint8_t clear_display_flag;		//флаг очистки дисплея. при переходе в обработчик нужно очистить дисплей единожды. будем делать по флагу

void test_handler(void);				//бан¤ по-черному, где испытываетс¤ вс¤ческое(оставить потом дл¤ примеров использовани¤)
void menu_process(void);					//обработка средств ввода(конкретно здесь:вращение ручки энкодера, длинное и короткое нажати¤ кнопки энкодера и отдельной кнопки)
void hardware_init(void);				//инициализаци¤ "железа", используемого в проекте

void main_screen_handler(void);
void backlight_handler(void);
void ADC_handler(void);
void about_handler(void);


#endif /* HANDLERS_H_ */