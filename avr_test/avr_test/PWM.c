#include "PWM.h"

void tim1A_init(void){
	WRITE_REG(TCNT1,0);
	WRITE_REG(ICR1,FREQ_VALUE);
	MODIFY_REG(TCCR1A,RESET_COM_A,(1<<COM1A1));                                     //non inverting
	SET_BIT(TCCR1A,(1<<WGM11));														//FAST PWM
	SET_BIT(TCCR1B,(1<<WGM12|1<<WGM13));
	//MODIFY_REG(TCCR1B,RESET_DIVIDER,(1<<CS12));                                 
	
}

 long map(long x, long in_min, long in_max, long out_min, long out_max){
	 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
 }

void pwm1A_start(uint8_t duty){
	duty>100?duty=100:0;
	WRITE_REG(OCR1A,(FREQ_VALUE*duty)/100);
	SET_BIT(TCCR1A,(1<<WGM11));                                           
	SET_BIT(TCCR1B,(1<<WGM12|1<<WGM13));
	MODIFY_REG(TCCR1B,RESET_DIVIDER,(1<<CS12));                                 
}

void pwm1A_stop(void){
	MODIFY_REG(TCCR1B,RESET_DIVIDER,0x00); 
	CLEAR_BIT(TCCR1A,(1<<WGM11));                                           //FAST PWM
	CLEAR_BIT(TCCR1B,(1<<WGM12|1<<WGM13));                             
}