#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include "encoder.h"
#include "st7735.h"
#include "uart.h"
#include "ADC.h"
#include "bme280.h"
#include "PWM.h"
#include "mpu6050.h"


volatile char string[100];
uint8_t pwm_value=50;
uint8_t enc=0,prev_enc=0;

int main(void)
{
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
  	draw_string(0,40,"bme280_id",0,BACKGROUND_COLOR,RED,TinyFont);
	draw_hexnumber(100,40,bme280_readbyte(BME280_ADDR, ID),0,BACKGROUND_COLOR,RED,TinyFont);
	draw_string(0,20,"mpu6050",0,BACKGROUND_COLOR,RED,TinyFont);
	draw_hexnumber(100,20,whoami(),0,BACKGROUND_COLOR,RED,TinyFont);
	
	//i2c_scanner(scanned_addr);
    while (1) 
    {
		pwm1A_start(pwm_value);
		pwm_value>100?pwm_value=100:0;
		draw_string(0,00,"backlight",0,BACKGROUND_COLOR,RED,TinyFont);
		draw_number(100,00,pwm_value,0,BACKGROUND_COLOR,WHITE,TinyFont);

	if(adc_conv_flag){
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
	
	if(shortpress_but){
		shortpress_but=0;
	}
		
	if(shortpress_enc_but){
		pwm1A_stop();
		shortpress_enc_but=0;
	}
		
	if(upState>=2){
		pwm_value++;
		enc++;
		upState=0;
		downState=0;
	}
		
	if(downState>=2){
		pwm_value--;
		enc--;
		downState=0;
		upState=0;
	}	
  }
}

