//текущее
//работа над меню по принципу библиотеки micromenu(связные списки)
//выводится список пунктов, слева указатель, перемещающийся относительно них
//работает перемещение по пунктам меню с помощью энкодера, вход и выход из меню второго уровня вложенности


#define F_CPU 16000000UL
#include "main.h"



int main(void){
	hardware_init();
	sei();
	//print_string("HELLO WORLD\r\n");
	//print_bme280_coef();
  	//i2c_scanner(scanned_addr);
	menu_init(&main_menu_item_1);                                                       //инициализируем меню и устанавливаем указатель на первый пункт
	display_current_menu(X_MENU_OFFSET,Y_MENU_OFFSET);                                  //отрисовываем начальное меню
    while (1) {
		
		switch(menustate){
			case 0:
				//main_screen_handler();	
				break;
			case  1:
			   // ADC_handler();
				break;
			case 2:
				backlight_handler();
				break;
			case 3:
				about_handler();
				break;
			default:
				menu_process();
				break;
						
		}
			
			
	}
}

