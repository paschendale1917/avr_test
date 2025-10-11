#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "macros.h"

#define RESET_MUX          0x0F
#define RESET_ADC_PRESC    0x07
#define RESET_DIDR0        0xFF
#define RESET_TRIGGER      0x07
#define RESET_REFERENCE    0xC0

#define ADC0               0x00
#define ADC1               0x01
#define ADC2               0x02
#define ADC3               0x03
#define ADC4               0x04
#define ADC5               0x05
#define ADC6               0x06
#define ADC7               0x07
#define TEMP_SENSOR        0x08
#define REF_VOLT		   0x0E

#define AVCC_REF           0x01
#define INT_REF            0x03            

#define ADCDIV_2           0x01
#define ADCDIV_4           0x02
#define ADCDIV_8           0x03
#define ADCDIV_16          0x04
#define ADCDIV_32          0x05
#define ADCDIV_64          0x06
#define ADCDIV_128         0x07

#define FREE_RUN           0x00
#define AN_COMP            0x01 //компаратор
#define INT_REQ_0          0x02
#define TC0_COMPA          0x03
#define TC0_OVF            0x04
#define TC1_COMPB          0x05
#define TC1_OVF            0x06
#define TC1_CAPTURE        0x07

#define ALFHA              0.1


extern volatile uint16_t ADC_data[];
extern volatile uint16_t ADC_data_filtered[]; 

void ADC_init(void);
uint16_t get_ADC_data(uint8_t adc_ch);
uint16_t ADC_average(volatile uint16_t *data);
void ema_filter_array(volatile uint16_t *data_in,  uint8_t size, float alpha);


#endif /* ADC_H_ */