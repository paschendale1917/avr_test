#include "encoder.h"

volatile uint8_t flag = 0;

 volatile uint8_t	shortpress_enc_but=0,
					shortpress_but=0,
					longpress_enc_but=0,
					longpress_but=0;
 
 
 
 volatile uint8_t	newState=3,
					oldState=3,
					upState=0,
					downState=0,
					encoder_value=0,
					buttonState=0;																	//переменные для работы с энкодером

volatile uint8_t	encoder_state=0;
volatile uint8_t	adc_conv_flag=0;
volatile uint8_t	adc_conv=0;  
volatile uint8_t	update_bme280_flag=0;                                                           //переменная, которую будем юзать для всяких отсчетов


 void enc_ports_init(void){
	 CLEAR_BIT(ENCODER_DDR,(1<<ENC_CLK)|(1<<ENC_DT)|(1<<ENC_BUTTON));								//на вход ножки энкодера и кнопка энкодера
	 SET_BIT(ENCODER_PORT,(1<<ENC_BUTTON));															//подтяжка энкодера и кнопки энкодера
	 
	 CLEAR_BIT(BUTTON_DDR,(1<<BUTTON));
	 SET_BIT(BUTTON_PORT,(1<<BUTTON));
													
	 SET_BIT(LED1_DDR,(1<<LED1));																	//ножка светодиода на выход 
	 LED1_ON;																						//логический нуль на ножке светодиода
	 
	 SET_BIT(LED0_DDR,(1<<LED0));																	//ножка светодиода на выход
	 LED0_ON;																						//логический нуль на ножке светодиода
}

 /*инициализация канала А таймера 0*/
 void timer0_init(){ 
	 encoder_state|=(1<<ENC_DT)|(1<<ENC_CLK);
	 TCNT0=0;																						//на всякий случай пишем в счетный регистр 0
	 SET_BIT(TCCR0A,(1<<WGM01));																	//сброс значения в  счетном регистре при совпадении со значением в регистре сравнения 
	 SET_BIT(TCCR0B,(1<<CS02|1<<CS00));																		//делитель 1024; 16000000/1024=15625 тиков в секунду
	 OCR0A=61;																						// 15625/125=125, где 125 - частота опроса(возникновения прерывания при совпадении)
	SET_BIT(TIMSK0,(1<<OCIE0A));																		//разрешаем прерывания при совпадении
 }
 
 ISR(TIMER0_COMPA_vect){
	static uint8_t count_enc_but = 0;																//счетчики
    static uint8_t count_but = 0;
	newState=(ENCODER_PIN&encoder_state)>>2;
	if(newState!=oldState){
		switch(oldState){
			 case 0:
				if(newState == 2) upState++;
				if(newState == 1) downState++;
				break;
			 case 1:
				if(newState == 0) upState++;
				if(newState == 3) downState++;
				break;
			 case 2:
				if(newState == 3) upState++;
				if(newState == 0) downState++;
				break;
			 case 3:
				if(newState == 1) upState++;
				if(newState == 2) downState++;
				break;
			 
		 }
		 oldState=newState;
	 }
	//обработка кнопки энкодера
	 if(!READ_BIT(ENCODER_PIN,(1<<ENC_BUTTON))){										
		if(count_enc_but < 255) {
		 count_enc_but++;
		}
	}
	  if(count_enc_but>LONG_TIMEOUT ) {																//длинное нажатие
		 if(READ_BIT(ENCODER_PIN,(1<<ENC_BUTTON))){
			longpress_enc_but=1;
			 count_enc_but=0;
		 }
	  }
	  if(count_enc_but>0 && count_enc_but<LONG_TIMEOUT) {											//короткое нажатие
		  if(READ_BIT(ENCODER_PIN,(1<<ENC_BUTTON))) {
			  shortpress_enc_but=1;
			  count_enc_but=0;
		  }
	  }
	 //обработка отдельной кнопки
	 if(!READ_BIT(BUTTON_PIN,(1<<BUTTON))){												
		if(count_but < 255) {
		count_but++;
		}
	}
	if(count_but>0 && count_but< LONG_TIMEOUT) {									//короткое нажатие
		if(READ_BIT(BUTTON_PIN,(1<<BUTTON))) {
			shortpress_but=1;
			count_but=0;
		}
	}
	if(count_but >LONG_TIMEOUT ) {													 //длинное нажатие
		if(READ_BIT(BUTTON_PIN,(1<<BUTTON))){
			longpress_but=1;
			count_but=0;
		}
	}
	adc_conv++;
	//SET_BIT(ADCSRA,1<<ADIF);
	//ADCSRA |= (1 << ADSC);
	!adc_conv?adc_conv_flag=1:0;
	adc_conv==125?update_bme280_flag=1:update_bme280_flag;
 }
 
 uint8_t readButtonState(void){
	 if(shortpress_enc_but){
		 return BUTTON_SELECT;
	 }else
	 if(shortpress_but){
		 return BUTTON_UP;
	 }else
	 if(longpress_but){
		 return BUTTON_DOWN;
	 }else
	 if(downState>=2){
		 return ENC_LEFT;
	 }
	 else
	 if(upState>=2){
		 return ENC_RIGHT;
	 }else
	 if(longpress_enc_but){
		 return BUTTON_MENUITEMBACK;
	 }
	 return BUTTON_NOTHING;
 }

 void resetButton(void){
	 
	 switch(readButtonState()){
		 case BUTTON_SELECT:
		 shortpress_enc_but=0;
		 break;
		 case BUTTON_UP :
		 shortpress_but=0;
		 break;
		 case BUTTON_DOWN:
		 longpress_but=0;
		 break;
		 case ENC_LEFT :
		 downState=0;
		 upState=0;
		 break;
		 case ENC_RIGHT :
		 upState=0;
		 downState=0;
		 break;
		 case BUTTON_MENUITEMBACK:
		 longpress_enc_but=0;
		 break;
	 }
 }
