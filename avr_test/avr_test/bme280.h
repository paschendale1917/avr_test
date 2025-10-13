#ifndef bme280_H
#define bme280_H

#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "i2c.h"
#include "st7735.h"
#include "encoder.h"
#include "uart.h"



typedef struct {
  uint16_t dig_T1;  //коэффициенты компенсационные(считываются с датчика и требуются для расчета температуры, влажности и давления)
  int16_t dig_T2;
  int16_t dig_T3;
  uint16_t dig_P1;
  int16_t dig_P2;
  int16_t dig_P3;
  int16_t dig_P4;
  int16_t dig_P5;
  int16_t dig_P6;
  int16_t dig_P7;
  int16_t dig_P8;
  int16_t dig_P9;
  int8_t dig_H1;
  int16_t dig_H2;
  uint8_t dig_H3;
  int16_t dig_H4;
  int16_t dig_H5;
  int8_t dig_H6;
  int32_t t_fine; //дополнительное поле для хранения рассчитанного в попугаях значения температуры/необходимо для расчетов давления и влажности
} compensation;



typedef struct {
  uint32_t press;
  float hum;
  float temp;
} record;



typedef record datatype;

typedef struct stack {
  int32_t capacity;         //длина стека(вместимость стека)
  uint32_t size;            //текущее количество элементов в стеке
  datatype *measure;       //указатель на массив элементов стека
} stack;


extern stack full_data;
extern stack m_data;
extern datatype measure;
extern compensation comp_bme280;
extern uint8_t raw_data[];
extern volatile uint16_t global_ticks;


extern volatile uint8_t flag_drawbme280;
extern volatile uint8_t flag_drawrtc;
extern volatile uint8_t flag_addrecord;

extern uint8_t while_delay;

enum {over_0 = 0x00,
      over_1 ,
      over_2,
      over_4,
      over_8,
      over_16
     } ;

enum {sleep_mode = 0x00,
      forced_mode = 0x02,
      normal_mode
     } ;

//в режиме normal датчик автоматически через заданные промежутки времени производит измерения(0.5,125,250 и т.д. мс)
enum {standby_05 = 0x01,
      standby_125,
      standby_250,
      standby_500,
      standby_1000,
      standby_10,
      standby_20
     } ;

enum {filter_off = 0x00,
      filter_2,
      filter_4,
      filter_8,
      filter_16
     } ;


#define BME280_ADDR                           0b11101100    //0xEC 
#define DEV_NAME                              0x60
     
#define START_COMP_ADDR_1                     0x88          //адрес начала компенсационных данных
#define NUM_COMP_BYTES_1                      25            //количество компенсационных байт 
                 
#define START_COMP_ADDR_2                     0xE1         
#define NUM_COMP_BYTES_2                      7   
            
#define HUM_LSB                               0xFE
#define HUM_MSB                               0xFD

#define TEMP_xLSB                             0xFC
#define TEMP_LSB                              0xFB
#define TEMP_MSB                              0xFA
//over
// 000 Skipped (output set to 0x80000) 
// 001 ?1 16 bit / 0.0050 °C
// 010 ?2 17 bit / 0.0025 °C
// 011 ?4 18 bit / 0.0012 °C
// 100 ?8 19 bit / 0.0006 °C
// 101, 110, 111 ?16 20 bit / 0.0003 °C

#define PRESS_xLSB                            0xF9
#define PRESS_LSB                             0xF8
#define PRESS_MSB                             0xF7
//over
// 000 Skipped (output set to 0x80000) 
// 001 ?1 16 bit / 2.62 Pa
// 010 ?2 17 bit / 1.31 Pa
// 011 ?4 18 bit / 0.66 Pa
// 100 ?8 19 bit / 0.33 Pa
// 101, 110, 111 ?16 20 bit / 0.16 Pa

#define BME280_CONCAT_BYTES(msb, lsb)         (((uint16_t)msb << 8) | (uint16_t)lsb)

#define STATUS                                0xF3
#define IM_UPDATE                             (bme280_readbyte(BME280_ADDR, STATUS)&0x01)    //данные загружены в регистр, если 0
#define MEASURING                             (bme280_readbyte(BME280_ADDR, STATUS)&0x08)    //если происходит преобразование, то даст 1

#define CONTROL_HUM                           0xF2
#define SET_OVERHUM(x)                        x

#define CONTROL_MEAS                          0xF4
#define SET_MEAS(overpress,overtemp,mode)     (overpress<<2)|(overtemp<<5)|mode
#define SET_OVERTEMP(x)                       (x<<5)
#define SET_OVERPRESS(x)                      (x<<2)
#define SET_MODE(x)                           x

#define ID                                    0xD0

#define CONFIG                                0xF5
#define SET_CONFIG(filter,standby)            (filter<<2)|(standby<<5)
#define SET_FILTER(x)                         (x<<2)
#define SET_STANDBY(x)                        (x<<5)

#define REG_RESET                             0xE0
#define COMMAND_RESET                         0xB6

#define STACK_SIZE                            32                                 

void i2c_init(void);
uint8_t start_cond(void);
uint8_t bme280_readbyte(uint8_t dev_addr, uint8_t reg);
uint8_t bme280_sendbyte(uint8_t dev_addr, uint8_t reg, uint8_t data);
void bme280_reset(void);
void print_bme280_coef(void);
uint8_t bme280_init(uint8_t hum_oversampling, uint8_t filter,uint8_t standby,uint8_t press_oversampling,uint8_t temp_oversampling,uint8_t mode);
uint8_t read_raw_data_bme280(uint8_t start_addr, uint8_t *data,  uint8_t num_bytes);
uint8_t get_compens(compensation *dt);
int32_t parse_temp_bme280(compensation *dc);
uint32_t parse_hum_bme280(compensation *dc);
uint32_t parse_press_bme280(compensation *dc);
void update_bme280(void);

#endif