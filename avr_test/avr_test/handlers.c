#include "handlers.h"

uint8_t menustate =255;
uint8_t clear_display_flag=0;

void hardware_init(void){
	enc_ports_init();
	timer0_init();                //таймер обслуживает средства ввода и используется для выставления флага обновления данных с bme280
	st7735_ports_init();          //инициализация дополнительных ножек микроконтроллерадля работы с дисплеем(cs,dc,backlight,reset)
	spi_master_init();			  //инициализация харварного spi в режиме мастера
    st7735_init();				  //"драйвер" дисплея. работает только на передачу данных в дисплей. никакие функции чтения регистров и настройки дисплея кроме инициализации не реализованы
	background(BACKGROUND_COLOR); //заливка дисплея черным фоном(можно настройть цвет фона в дефайнах)
	uart_init();	
	ADC_init();
	i2c_init();
	bme280_init(over_1,filter_off,standby_1000,over_1,over_1,normal_mode);
	tim1A_init();				 //таймер, отвечающий за подстветку. самый "жирный" на этом мк. взял его, чтобы поиграться с частотой и скважностью
	pwm1A_start(50);
	mpu6050_init();
}

void test_handler(void){
	pwm1A_start(pwm_value);
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
	 prev_enc=enc;	
}

void menu_process(void){
	switch(readButtonState()){
				case ENC_LEFT:
					//pwm_value--;
					//enc--;
					menu_navigate_prev(); //если поворот влево, то в указатель пункта меню пишем адрес структуры, описывающей предыдущий пункт меню
					pointer_clear_flag=1; //флаг используется для того, чтобы указатель перерисовывался не постоянно, тратя ресурсы мк, а только тогда, когда происходит поворот ручки энкодера
					resetButton();
					break ;
				
				case ENC_RIGHT:
					//pwm_value++;
					//enc++;
					menu_navigate_next();
					pointer_clear_flag=1;
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
					resetButton();
					break ;
				
				case BUTTON_SELECT:
					execute_menu_action();											//запуск обработчика функции 
					enter_submenu();												//в current_menu адрес дочернего пункта меню
					resetButton();
					break ;
				
				default:
					display_pointer(pointer);
					break ;
	}
}

void return_from_handler(void){
	menustate=MENU_PROCESS;												//пишем в переменную статуса меню любое значение, которое не отслеживает switch  в бесконечном цикле
	clear_display();													//очищаем дисплей от экрана обработчика
	display_current_menu(X_MENU_OFFSET,Y_MENU_OFFSET);					//выводим текущее меню
	draw_pointer(current_menu);											//отрисовываем указатель заново
	resetButton();														//обязательно сбросить флаг нажатой кнопки, иначе он будет висеть и кнопка окажется недееспособной
}

void main_screen_handler(void){
	menustate=0;
	if(clear_display_flag){
		clear_display();
		clear_display_flag=0;
	}
	update_bme280();
	if(readButtonState()==BUTTON_MENUITEMBACK){								//в обработчике ожидаем нжатие кнопки возврата, если оно происходит, то
		return_from_handler();												//вызываем функцию возврата к меню
	}
}

void backlight_handler(void){
	menustate=2;
	char buf[20]="";
	sprintf(buf,"%u%%",pwm_value);
	switch(readButtonState()){
		case BUTTON_MENUITEMBACK:
			return_from_handler();
			break ;
		case ENC_LEFT:
			pwm_value--;
			resetButton();
			break ;
				
		case ENC_RIGHT:
			pwm_value++;
			resetButton();
			break ;
				
		default:
			pwm1A_start(pwm_value);	
			pwm_value>100?pwm_value=100:0;
			draw_string(15,0,"BACKLIGHT",-3,BACKGROUND_COLOR,RED,BigFont);
			//draw_number(30,20,pwm_value,0,BACKGROUND_COLOR,WHITE,Grotesk16x32);
			draw_string(45,20,buf,0,BACKGROUND_COLOR,RED,Grotesk16x32);
			break ;
		}
	
}

void ADC_handler(void){
	
}



void about_handler(void){
	menustate=3;
	//resetButton();
	/*if(clear_display_flag){													//очистить нужно единожды, поэтому был введен флаг очистки дисплея                           
		clear_display();
		clear_display_flag=0;
	}*/
	draw_string(0,0,"Bryansk 2025",0,BACKGROUND_COLOR,MAGENTA,TinyFont);
	if(readButtonState()==BUTTON_MENUITEMBACK){								//в обработчике ожидаем нжатие кнопки возврата, если оно происходит, то
		return_from_handler();												//вызываем функцию возврата к меню
	}
}