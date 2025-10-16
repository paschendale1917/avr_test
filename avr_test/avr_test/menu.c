﻿#include "menu.h"

volatile char string[100];
uint8_t pwm_value=50;
uint8_t enc=0,prev_enc=0;

uint8_t pointer_clear_flag=0;

const char PROGMEM sys_set[]="SYSTEM_SETTINGS";
const char PROGMEM menu_it2[]="MENU_ITEM_2";
const char PROGMEM menu_it3[]="MENU_ITEM_3";
const char PROGMEM about[]="ABOUT";

const char PROGMEM adc_set[]="ADC_SETTINGS";
const char PROGMEM backlght[]="BACKLIGHT";
const char PROGMEM menu_it1_3[]="MENU_ITEM_1.3";

const char PROGMEM menu_it2_1[]="MENU_ITEM_2.1";
const char PROGMEM menu_it2_2[]="MENU_ITEM_2.2";

const char PROGMEM menu_it3_1[]="MENU_ITEM_3.1";
const char PROGMEM menu_it3_2[]="MENU_ITEM_3.2";
const char PROGMEM menu_it3_3[]="MENU_ITEM_3.3";
const char PROGMEM menu_it3_4[]="MENU_ITEM_3.4";
const char PROGMEM menu_it3_5[]="MENU_ITEM_3.5";


const char *main_menu[]={sys_set,menu_it2,menu_it3,about};
const char *item_1_menu[]={adc_set,backlght,menu_it1_3};
const char *item_2_menu[]={menu_it2_1,menu_it2_2};
const char *item_3_menu[]={menu_it3_1,menu_it3_2,menu_it3_3,menu_it3_4,menu_it3_5};

/*
const char *main_menu[]={"SYSTEM_SETTINGS","MENU_ITEM_2","MENU_ITEM_3","ABOUT"};
const char *item_1_menu[]={"ADC_SETTINGS","BACKLIGHT","MENU_ITEM_1.3"};
const char *item_2_menu[]={"MENU_ITEM_2.1","MENU_ITEM_2.2"};
const char *item_3_menu[]={"MENU_ITEM_3.1","MENU_ITEM_3.2","MENU_ITEM_3.3","MENU_ITEM_3.4","MENU_ITEM_3.5"};*/
const char *pointer=">";

//указатель на структуру, описывающую текущий пункт меню
//в него будем грузить адреса требуемых пунктов(например, при нажатии кнопок или вращении ручки энкодера
MenuItem *current_menu = NULL;

//создаем пункты меню
MenuItem main_menu_item_1,main_menu_item_2,main_menu_item_3,main_menu_item_4;
MenuItem menu_item_1_1,menu_item_1_2,menu_item_1_3;
MenuItem menu_item_2_1,menu_item_2_2;
MenuItem menu_item_3_1,menu_item_3_2,menu_item_3_3,menu_item_3_4,menu_item_3_5;

//в этих структурах описываются все связи пунктов меню, а также инфа об указателе > на них
//при перемещении указателя вся инфа берется отсюда
//описание всех элементов структуры в заголовочном файле
void menu_init(MenuItem* start_menu) {
	const uint8_t line_height = (pgm_read_byte(MENU_FONT + 1) + POINTER_TAB);
	//главное меню
	main_menu_item_1 =  (MenuItem){main_menu,  4, NULL,					&main_menu_item_4,  &main_menu_item_2,  NULL,		       &menu_item_1_1,  X_POINTER_OFFSET, Y_POINTER_OFFSET};
	main_menu_item_2 =  (MenuItem){main_menu,  4, NULL,					&main_menu_item_1,  &main_menu_item_3,  NULL,		       &menu_item_2_1,  X_POINTER_OFFSET, Y_POINTER_OFFSET + 1 * line_height};
	main_menu_item_3 =  (MenuItem){main_menu,  4, NULL,					&main_menu_item_2,  &main_menu_item_4,  NULL,			   &menu_item_3_1,  X_POINTER_OFFSET, Y_POINTER_OFFSET + 2 * line_height};
	main_menu_item_4 =  (MenuItem){main_menu,  4, about_handler,		&main_menu_item_3,  &main_menu_item_1,  NULL,			   NULL,		    X_POINTER_OFFSET, Y_POINTER_OFFSET + 3 * line_height};
	
	//меню второго уровня вложенности пункта 1 главного меню
	menu_item_1_1 =		(MenuItem){item_1_menu, 3, NULL,				&menu_item_1_3,		&menu_item_1_2,		&main_menu_item_1, NULL,			X_POINTER_OFFSET, Y_POINTER_OFFSET};
	menu_item_1_2 =		(MenuItem){item_1_menu, 3, backlight_handler,   &menu_item_1_1,		&menu_item_1_3,		&main_menu_item_1, NULL,			X_POINTER_OFFSET, Y_POINTER_OFFSET + 1 * line_height};
	menu_item_1_3 =		(MenuItem){item_1_menu, 3, NULL,				&menu_item_1_2,		&menu_item_1_1,		&main_menu_item_1, NULL,			X_POINTER_OFFSET, Y_POINTER_OFFSET + 2 * line_height};
	
	//меню второго уровня вложенности пункта 2 главного меню
	menu_item_2_1 =		(MenuItem){item_2_menu, 2, NULL,				&menu_item_2_2,		&menu_item_2_2,		&main_menu_item_1, NULL,			X_POINTER_OFFSET, Y_POINTER_OFFSET};
	menu_item_2_2 =		(MenuItem){item_2_menu, 2, NULL,				&menu_item_2_1,		&menu_item_2_1,		&main_menu_item_1, NULL,			X_POINTER_OFFSET, Y_POINTER_OFFSET + 1 * line_height};

	//меню второго уровня вложенности пункта 3 главного меню
	menu_item_3_1 =		(MenuItem){item_3_menu, 5, NULL,				&menu_item_3_5,		&menu_item_3_2,		&main_menu_item_1, NULL,			X_POINTER_OFFSET, Y_POINTER_OFFSET};
	menu_item_3_2 =		(MenuItem){item_3_menu, 5, NULL,				&menu_item_3_1,		&menu_item_3_3,		&main_menu_item_1, NULL,			X_POINTER_OFFSET, Y_POINTER_OFFSET + 1 * line_height};
	menu_item_3_3 =		(MenuItem){item_3_menu, 5, NULL,				&menu_item_3_2,		&menu_item_3_4,		&main_menu_item_1, NULL,			X_POINTER_OFFSET, Y_POINTER_OFFSET + 2 * line_height};
	menu_item_3_4 =		(MenuItem){item_3_menu, 5, NULL,				&menu_item_3_3,		&menu_item_3_5,		&main_menu_item_1, NULL,			X_POINTER_OFFSET, Y_POINTER_OFFSET + 3 * line_height};
	menu_item_3_5 =		(MenuItem){item_3_menu, 5, NULL,				&menu_item_3_4,		&menu_item_3_1,		&main_menu_item_1, NULL,			X_POINTER_OFFSET, Y_POINTER_OFFSET + 4 * line_height};
		
	current_menu = start_menu;
	pointer_clear_flag=1;
}

void draw_pointer(MenuItem *ptr){
	draw_string(ptr->pointer_pos_x, ptr->pointer_pos_y, pointer,MENUFONT_SPACE, BACKGROUND_COLOR, POINTER_COLOR, MENU_FONT);
}

void clear_pointer(MenuItem *ptr){
	draw_string(ptr->pointer_pos_x, ptr->pointer_pos_y, pointer,MENUFONT_SPACE, BACKGROUND_COLOR, BACKGROUND_COLOR, MENU_FONT);
}

void clear_pointer_coord(MenuItem *ptr){
	ptr->pointer_pos_x=X_POINTER_OFFSET;
	ptr->pointer_pos_y=Y_POINTER_OFFSET;
}

void display_pointer(const char *pointer) {
	static MenuItem *prev_menu = NULL;
	if(pointer_clear_flag) {                                  //если произошел поворот ручки энкодера и вследсвие этого выставлен флаг
		if(prev_menu != NULL && prev_menu != current_menu) {  //  если меню изменилось очищаем старый указатель
			clear_pointer(prev_menu);
		}
		draw_pointer(current_menu);							//отрисовываем указатель нового меню
		prev_menu = current_menu;							//запоминаем, что у нас произошел поворот ручки 
		pointer_clear_flag = 0;
	}
}

//функция отображения всех пунктов текущего меню
void display_current_menu(uint8_t xstart, uint8_t ystart) {
	uint8_t space=pgm_read_byte(MENU_FONT+1)+2;
	clear_pointer(current_menu);
	for(uint8_t i=0;i<current_menu->num_menu_items;i++){
		draw_string_progmem(xstart,ystart+space*i,*(current_menu->menu_name+i), MENUFONT_SPACE, BACKGROUND_COLOR, MENU_COLOR,MENU_FONT);
	}
}

//очищаем только те области, что были закрашены. полное очищение дольше и визуально подмаргивает дисплей
void clear_current_menu(uint8_t xstart, uint8_t ystart) { 
	uint8_t space=pgm_read_byte(MENU_FONT+1)+2;
	for(uint8_t i=0;i<current_menu->num_menu_items;i++){
		draw_string_progmem(xstart,ystart+space*i,*(current_menu->menu_name+i), MENUFONT_SPACE, BACKGROUND_COLOR, BACKGROUND_COLOR,MENU_FONT);
	}
	clear_pointer(current_menu);	
}

//выход в меню более высокого уровня из дочернего меню
void enter_upmenu(void) {
	if(current_menu->parent != NULL||current_menu->action!=NULL) {					//если есть родительский пункт меню или обрабочик пункта меню
		clear_current_menu(X_MENU_OFFSET,Y_MENU_OFFSET);							//стираем меню
		current_menu = current_menu->parent;										//загружаем указатель на родительский пункт меню
		pointer_clear_flag = 1;														//выставляет флаг необходимости отрисовать указатель
		display_current_menu(X_MENU_OFFSET, Y_MENU_OFFSET);							//отрисовываем меню и указатель
	}else if(current_menu->parent==NULL)menustate=0;                                //если активно меню самого высокого порядка(у которого нет родительского меню), то выходим на главный экран
}

//вход в дочернее меню
void enter_submenu(void) {
	if(current_menu->child != NULL) {
		clear_current_menu(X_MENU_OFFSET,Y_MENU_OFFSET);
		current_menu = current_menu->child;
		pointer_clear_flag = 1;
		display_current_menu(X_MENU_OFFSET, Y_MENU_OFFSET);
	}
}

// функция запуска обработчика пункта меню
void execute_menu_action(void) {                               
	if (current_menu->action)current_menu->action();
}

void menu_navigate_next(void){
	current_menu = current_menu->next;	
}

void menu_navigate_prev(void){
	current_menu = current_menu->prev;
}

