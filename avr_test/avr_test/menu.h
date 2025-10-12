
#ifndef MENU_H_
#define MENU_H_

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "fonts.h"
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include "st7735.h"
#include "uart.h"
#include "ADC.h"
#include "PWM.h"
#include "mpu6050.h"
#include "bme280.h"


#define MENU_COLOR														GREEN
#define POINTER_COLOR                                                   ORANGE
#define MENU_FONT                                                       TinyFont
#define BIGFONT_SPACE													-3
#define TINYFONT_SPACE													0
#define MENUFONT_SPACE													TINYFONT_SPACE
#define X_MENU_OFFSET													20               //меньше (font[1]+2) не делать! иначе не влезет в экран указатель пункта меню
#define Y_MENU_OFFSET													0
#define X_POINTER_OFFSET												(X_MENU_OFFSET-10)
#define Y_POINTER_OFFSET												Y_MENU_OFFSET

#define TINYFONT

#ifdef BIGFONT
#define SYSTEM_FONT														BigFont
#define SYSTEMFONT_SPACE												BIGFONT_SPACE
#else
#define SYSTEM_FONT														TinyFont
#define SYSTEMFONT_SPACE												TINYFONT_SPACE
#endif


// Структура пункта меню
typedef struct MenuItem {
	const char **menu_name;    // указатель на указатель на массив строк с названиями пунктов меню
	uint8_t num_menu_items;	   // количество пунктов меню
	void (*action)(void);      // указатель на функцию-обработчик
	struct MenuItem *prev;     // указатель на структуру предыдущего пункта меню
	struct MenuItem *next;     // указатель на структуру следующего пункта меню
	struct MenuItem *parent;   // указатель на структуру родительского(более высокого уровня вложенности) пункта меню
	struct MenuItem *child;    // указатель на структуру дочернего(более низкого уровня вложенности) пункта меню
	uint16_t pointer_pos_x;    // позиция указателя х
	uint16_t pointer_pos_y;    // позиция указателя у
} MenuItem;





extern MenuItem* current_menu;
extern MenuItem main_menu_item_1,main_menu_item_2,main_menu_item_3,main_menu_item_4;


extern const char *main_menu[];
extern const char *item_1_menu[];
extern const char *item_2_menu[];
extern const char *item_3_menu[];
extern const char *pointer;



extern uint8_t pointer_clear_flag; //флаг нужен для того, чтобы контроллер обновлял значок указателя только при пермещении в меню, а не на постоянной основе

extern uint8_t pwm_value,enc,prev_enc;

void menu_init(MenuItem* start_menu);
void display_current_menu(uint8_t xstart, uint8_t ystart);
void clear_current_menu(uint8_t xstart, uint8_t ystart);
void execute_menu_action(void);
void enter_submenu(void);
void enter_upmenu(void);
void display_pointer(const char *pointer);
void draw_pointer(MenuItem *ptr);
void clear_pointer(MenuItem *ptr);
void clear_pointer_coord(MenuItem *ptr);
void main_menu_it4_handler(void);

#endif /* MENU_H_ */