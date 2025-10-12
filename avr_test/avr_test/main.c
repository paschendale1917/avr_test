//текущее
//работа над меню по принципу библиотеки micromenu
//выводится список пунктов, слева указатель, перемещающийся относительно них
//работает перемещение по пунктам меню с помощью энкодера, вход и выход из меню второго уровня вложенности
//to do: запуск обработчиков из меню, допилить mpu6050

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

int main(void){
	enc_ports_init();
	timer0_init();
	st7735_ports_init();
	spi_master_init();
    st7735_init();
	background(BACKGROUND_COLOR);
	sei();
	uart_init();
	ADC_init();
	i2c_init();
	bme280_init(over_1,filter_off,standby_1000,over_1,over_1,normal_mode);
	tim1A_init();
	pwm1A_start(50);
	mpu6050_init();
	//print_string("HELLO WORLD\r\n");
	//print_bme280_coef();
  	//i2c_scanner(scanned_addr);
	menu_init(&main_menu_item_1);                                                       //инициализируем меню и устанавливаем указатель на первый пункт
	display_current_menu(X_MENU_OFFSET,Y_MENU_OFFSET);                                  //отрисовываем начальное меню
    while (1) {
		
			switch(readButtonState()){
				case ENC_LEFT:
					pwm_value--;
					enc--;
					current_menu = current_menu->prev;
					pointer_clear_flag=1;
					display_pointer(pointer);
					resetButton();
					break ;
				
				case ENC_RIGHT:
					pwm_value++;
					enc++;
					current_menu = current_menu->next;
					pointer_clear_flag=1;
					display_pointer(pointer);
					resetButton();
					break ;
				
				case BUTTON_UP:
					resetButton();
					break ;
				
				case BUTTON_DOWN:
					resetButton();
					break ;
					
				case BUTTON_MENUITEMBACK:
					enter_upmenu();	
					display_pointer(pointer);	
					resetButton();
					break ;
				
				case BUTTON_SELECT:
					execute_menu_action();											//запуск обработчика функции 
					enter_submenu();												//в current_menu адрес дочернего пункта меню
					display_current_menu(X_MENU_OFFSET,Y_MENU_OFFSET);
					display_pointer(pointer);
					resetButton();
					break ;
				
				default:
					display_pointer(pointer);
	   				/*pwm1A_start(pwm_value);
	   				pwm_value>100?pwm_value=100:0;
	   				draw_string(0,00,"backlight",0,BACKGROUND_COLOR,RED,TinyFont);
	   				draw_number(100,00,pwm_value,0,BACKGROUND_COLOR,WHITE,TinyFont);
	   				
	   				if(adc_conv_flag){
		   				draw_string(0,40,"bme280_id",0,BACKGROUND_COLOR,RED,TinyFont);
		   				draw_hexnumber(100,40,bme280_readbyte(BME280_ADDR, ID),0,BACKGROUND_COLOR,RED,TinyFont);
		   				draw_string(0,20,"mpu6050",0,BACKGROUND_COLOR,RED,DotMatrix_M_Slash);
		   				draw_hexnumber(100,20,whoami(),0,BACKGROUND_COLOR,RED,TinyFont);
		   				ema_filter_array(ADC_data,255,ALFHA);
		   				uint16_t dt=ADC_average(ADC_data);
		   				float voltage=(dt*4.51)/1024;
		   				draw_string(0,10,"voltage",0,BACKGROUND_COLOR,RED,TinyFont);
		   				draw_float_number(100,10,voltage,"%0.1f",0,BACKGROUND_COLOR,MAGENTA,TinyFont);
		   				draw_string(132,10,"v",0,BACKGROUND_COLOR,MAGENTA,TinyFont);
		   				print_string("ADC_value ");
		   				print_number(dt);
		   				print_string("\r\n");
		   				mpu6050_ready_data(&mpu6050_dt);
		   				print_mpu6050_data();
		   				adc_conv_flag=0;
	   				}
	   				update_bme280();
	   				draw_string(0,30,"mpu_temp",0,BACKGROUND_COLOR,RED,TinyFont);
	   				draw_float_number(100,30,mpu6050_temp(),"%.1f",0,BACKGROUND_COLOR,WHITE,TinyFont);
	   				
	   				if(enc-prev_enc>0){
		   				print_string("clockwise ");
		   				print_number(enc);
		   				print_string("\r\n");
		   				} else if(enc-prev_enc<0){
		   				print_string("counterclockwise ");
		   				print_number(enc);
		   				print_string("\r\n");
	   				}
	   				prev_enc=enc;*/
				
				
				break ;
			}
			
  }
}

