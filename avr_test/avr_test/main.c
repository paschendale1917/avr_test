//текущее
//работа над меню по принципу библиотеки micromenu
//выводится список пунктов, слева указатель, перемещающийся относительно них
//работает перемещение по пунктам меню с помощью энкодера, вход и выход из меню второго уровня вложенности
//to do: запуск обработчиков из меню, допилить mpu6050

#define F_CPU 16000000UL
#include "main.h"

int main(void){
	hardware_init();
	sei();
	//print_string("HELLO WORLD\r\n");
	//print_bme280_coef();
  	i2c_scanner(scanned_addr);
	menu_init(&main_menu_item_1);                                                       //инициализируем меню и устанавливаем указатель на первый пункт
	display_current_menu(X_MENU_OFFSET,Y_MENU_OFFSET);                                  //отрисовываем начальное меню
    while (1) {
		enc_process();	
			
			
  }
}

