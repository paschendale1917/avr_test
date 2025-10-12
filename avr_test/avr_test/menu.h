
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
#define X_MENU_OFFSET													20               //������ (font[1]+2) �� ������! ����� �� ������ � ����� ��������� ������ ����
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


// ��������� ������ ����
typedef struct MenuItem {
	const char **menu_name;    // ��������� �� ��������� �� ������ ����� � ���������� ������� ����
	uint8_t num_menu_items;	   // ���������� ������� ����
	void (*action)(void);      // ��������� �� �������-����������
	struct MenuItem *prev;     // ��������� �� ��������� ����������� ������ ����
	struct MenuItem *next;     // ��������� �� ��������� ���������� ������ ����
	struct MenuItem *parent;   // ��������� �� ��������� �������������(����� �������� ������ �����������) ������ ����
	struct MenuItem *child;    // ��������� �� ��������� ���������(����� ������� ������ �����������) ������ ����
	uint16_t pointer_pos_x;    // ������� ��������� �
	uint16_t pointer_pos_y;    // ������� ��������� �
} MenuItem;





extern MenuItem* current_menu;
extern MenuItem main_menu_item_1,main_menu_item_2,main_menu_item_3,main_menu_item_4;


extern const char *main_menu[];
extern const char *item_1_menu[];
extern const char *item_2_menu[];
extern const char *item_3_menu[];
extern const char *pointer;



extern uint8_t pointer_clear_flag; //���� ����� ��� ����, ����� ���������� �������� ������ ��������� ������ ��� ���������� � ����, � �� �� ���������� ������

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