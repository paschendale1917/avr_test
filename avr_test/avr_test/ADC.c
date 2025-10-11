#include "ADC.h"

volatile uint16_t ADC_data[256]={0,};
volatile uint16_t ADC_data_filtered[256]={0,};
volatile uint8_t adc_dt=0;
	
void set_ADC_channel(uint8_t adc_ch){
	switch(adc_ch){
		case 0:
			MODIFY_REG(ADMUX, RESET_MUX, ADC0);
			MODIFY_REG(DIDR0, RESET_DIDR0, ADC0); //отключение цифрового входа
			break;
		case 1:
			MODIFY_REG(ADMUX, RESET_MUX, ADC1);
			MODIFY_REG(DIDR0, RESET_DIDR0, ADC1);
			break;
		case 2:
			MODIFY_REG(ADMUX, RESET_MUX, ADC2);
			MODIFY_REG(DIDR0, RESET_DIDR0, ADC2);
			break;
		case 3:
			MODIFY_REG(ADMUX, RESET_MUX, ADC3);
			MODIFY_REG(DIDR0, RESET_DIDR0, ADC3);
			break;
		case 4:
			MODIFY_REG(ADMUX, RESET_MUX, ADC4);
			MODIFY_REG(DIDR0, RESET_DIDR0, ADC4);
			break;
		case 5:
			MODIFY_REG(ADMUX, RESET_MUX, ADC5);
			MODIFY_REG(DIDR0, RESET_DIDR0, ADC5);
			break;
		case 6:
			MODIFY_REG(ADMUX, RESET_MUX, ADC6);
			MODIFY_REG(DIDR0, RESET_DIDR0, ADC6);
			break;
		case 7:
			MODIFY_REG(ADMUX, RESET_MUX, ADC7);
			MODIFY_REG(DIDR0, RESET_DIDR0, ADC7);
			break;
		case 8:
			MODIFY_REG(ADMUX, RESET_MUX, TEMP_SENSOR);
			break;
	}
}

void ADC_init(void){
	MODIFY_REG(ADMUX,RESET_REFERENCE,AVCC_REF<<6);		//опора(измерить мультиметром) 4.64в
	MODIFY_REG(ADCSRA, RESET_ADC_PRESC, ADCDIV_128);	//делитель 16000000/128= 125 кГц
	MODIFY_REG(ADCSRB,RESET_TRIGGER,TC0_COMPA); 
	SET_BIT(ADCSRA,1 << ADATE|1 << ADIE);				//автозапуск отключен,включены прерывания АЦП
	set_ADC_channel(ADC1);
	SET_BIT(ADCSRA,(1<<ADEN));								//включаем ацп	
	
}

//функция для работы во free running mode
uint16_t get_ADC_data(uint8_t adc_ch){
	if(adc_ch>8)return ERROR;
	uint16_t result=0;
	set_ADC_channel(adc_ch);							//выбор канала мультиплексора ацп
	SET_BIT(ADCSRA,(1<<ADSC));                        //начало преобразования
	while(!READ_BIT(ADCSRA,(1<<ADIF))){};
	return result = ADCL | (ADCH << 8);	
}

uint16_t ADC_average(volatile uint16_t *data){
	uint32_t result=0;
	for(uint8_t i=0;i<255;i++){
		result+=*(data+i);
	}
	return (uint16_t) (result/256);
}


uint16_t ema_filter(uint16_t input, uint16_t prev_output, float alpha) {
	return (uint16_t)(alpha * input + (1.0 - alpha) * prev_output);      
}


void ema_filter_array(volatile uint16_t *data,  uint8_t size, float alpha) { 
	for(uint8_t i = 1; i < size; i++) {
		data[i-1] = ema_filter(data[i], data[i-1], alpha);
	}
}
//256 раз АЦП пинается таймером 0
ISR(ADC_vect){
	ADC_data[adc_dt++] = ADCL|(ADCH << 8);
}