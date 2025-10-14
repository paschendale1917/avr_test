#include "bme280.h"

//[0:2] - давление; [3:5]-температура; [6:7]-влажность
uint8_t raw_data[8] = {0};
compensation comp_bme280={0,};
datatype measure={0,};
stack m_data={0};
uint8_t is_stack_init=0;
record bme280_data;

volatile uint16_t global_ticks=0;
volatile uint8_t flag_drawbme280=0;
volatile uint8_t flag_drawrtc=0;
volatile uint8_t flag_addrecord=0;

uint8_t while_delay=0;

uint8_t bme280_sendbyte(uint8_t dev_addr, uint8_t reg,uint8_t data){
	start_cond();
	i2c_sendbyte(dev_addr);
	i2c_sendbyte(reg);
	i2c_sendbyte(data);
	stop_cond();
	return SUCCESS;
}

uint8_t bme280_send_data(uint8_t dev_addr, uint8_t *data, uint8_t num_bytes){
	start_cond();
	i2c_sendbyte(dev_addr);
	for(uint8_t i=0;i<num_bytes;i++){
		i2c_sendbyte(*(data+i));
	}
	stop_cond();
	return SUCCESS;
}

uint8_t bme280_readbyte(uint8_t dev_addr, uint8_t reg){
	uint8_t byte=0;
	start_cond();
	i2c_sendbyte(dev_addr);
	i2c_sendbyte(reg);
	start_cond();
	i2c_sendbyte(dev_addr|REQUEST_READ);
	byte=i2c_readbyte(NACK);
	stop_cond();
	return byte;
}

uint8_t bme280_readdata(uint8_t dev_addr, uint8_t reg,uint8_t *data,uint8_t num_bytes){
	uint8_t status=0;
	start_cond();
	status=i2c_sendbyte(dev_addr|REQUEST_WRITE);
	if(status!=ACK)return ERROR;
	status=i2c_sendbyte(reg);
	if(status!=ACK)return ERROR;
	start_cond();
	status=i2c_sendbyte(dev_addr|REQUEST_READ);
	if(status!=ACK)return ERROR;
	   for (uint8_t cntr = 0; cntr < num_bytes; cntr++) {
		   if (cntr < num_bytes - 1) {
			   *(data+cntr) = i2c_readbyte(ACK);								// отправл¤ем ACK дл¤ всех, кроме последнего байта
			   } else {
			   *(data+cntr)= i2c_readbyte(NACK);								// отправл¤ем NACK дл¤ последнего байта
		   }
	   }																		
	 stop_cond();
	 return SUCCESS;
}

//ѕередача данных: адрес регистра, затем байт настроек. ћожно писать сразу во все регистры
uint8_t bme280_init(uint8_t hum_oversampling, uint8_t filter, uint8_t standby, uint8_t press_oversampling, uint8_t temp_oversampling, uint8_t mode)
{
  uint8_t  settings[6] = {CONTROL_HUM,                                                                       //адрес регистра настроек измерени¤ влажности
                          SET_OVERHUM(hum_oversampling),                                                     //настройки регистра измерени¤ влажности(установлен оверсэмплинг х8)
                          CONFIG,
                          SET_CONFIG(filter, standby),
                          CONTROL_MEAS,
                          SET_MEAS(press_oversampling, temp_oversampling, mode)
                         };
  if(bme280_readbyte(BME280_ADDR, ID) != DEV_NAME) {
	   return ERROR; 
	} 
  bme280_reset();
  bme280_send_data(BME280_ADDR, settings, sizeof(settings) / sizeof(settings[0]));
  get_compens(&comp_bme280);
  return SUCCESS;
}


void bme280_reset(void)
{
  uint8_t  reset[2] = {RESET, COMMAND_RESET};
  bme280_send_data(BME280_ADDR, reset, 2);
}
 //читаем порцию данных с датчика
uint8_t read_raw_data_bme280(uint8_t start_addr, uint8_t *data,  uint8_t num_bytes){
 // if(read_byte_i2c(BME280_ADDR, ID) != 0x60) { return ERROR; }
 bme280_readdata(BME280_ADDR, start_addr, data, num_bytes);
  return SUCCESS;
}


//запись в структуру компенсационных констант, считанных с датчика
uint8_t get_compens(compensation *dt){
  static uint8_t comp_data[NUM_COMP_BYTES_1 + NUM_COMP_BYTES_2] = {0};
  read_raw_data_bme280(START_COMP_ADDR_1, comp_data, NUM_COMP_BYTES_1);
  read_raw_data_bme280(START_COMP_ADDR_2, &comp_data[NUM_COMP_BYTES_1], NUM_COMP_BYTES_2);
  dt->dig_T1 = comp_data[1] << 8 | comp_data[0];
  dt->dig_T2 = comp_data[3] << 8 | comp_data[2];
  dt->dig_T3 = comp_data[5] << 8 | comp_data[4];
  dt->dig_P1 = comp_data[7] << 8 | comp_data[6];
  dt->dig_P2 = comp_data[9] << 8 | comp_data[8];
  dt->dig_P3 = comp_data[11] << 8 | comp_data[10];
  dt->dig_P4 = comp_data[13] << 8 | comp_data[12];
  dt->dig_P5 = comp_data[15] << 8 | comp_data[14];
  dt->dig_P6 = comp_data[17] << 8 | comp_data[16];
  dt->dig_P7 = comp_data[19] << 8 | comp_data[18];
  dt->dig_P8 = comp_data[21] << 8 | comp_data[20];
  dt->dig_P9 = comp_data[23] << 8 | comp_data[22];
 dt->dig_H1 = (int8_t)comp_data[24];
 dt->dig_H2 = (int16_t)((comp_data[26] << 8) | comp_data[25]);
 dt->dig_H3 = (int8_t)comp_data[27];
 dt->dig_H4 = (int16_t)(((comp_data[28] << 4) | (comp_data[29] & 0x0F)) - 65536);
 dt->dig_H5 = (int16_t)(((comp_data[30] << 4) | (comp_data[29] >> 4)) - 65536);
 dt->dig_H6 = (int8_t)comp_data[31];
  return SUCCESS;
}

//дл¤ отладки
void print_bme280_coef(void){
	char str1[250];
	sprintf(str1, "DIG_T1: %u\r\n", comp_bme280.dig_T1);
	print_string(str1);
	sprintf(str1, "DIG_T2: %d\r\n", comp_bme280.dig_T2);
	print_string(str1);
	sprintf(str1, "DIG_T3: %d\r\n", comp_bme280.dig_T3);
	print_string(str1);
	sprintf(str1, "DIG_P1: %u\r\n", comp_bme280.dig_P1);
	print_string(str1);
	sprintf(str1, "DIG_P2: %d\r\n", comp_bme280.dig_P2);
	print_string(str1);
	sprintf(str1, "DIG_P3: %d\r\n", comp_bme280.dig_P3);
	print_string(str1);
	sprintf(str1, "DIG_P4: %d\r\n", comp_bme280.dig_P4);
	print_string(str1);
	sprintf(str1, "DIG_P5: %d\r\n", comp_bme280.dig_P5);
	print_string(str1);
	sprintf(str1, "DIG_P6: %d\r\n", comp_bme280.dig_P6);
	print_string(str1);
	sprintf(str1, "DIG_P7: %d\r\n", comp_bme280.dig_P7);
	print_string(str1);
	sprintf(str1, "DIG_P8: %d\r\n", comp_bme280.dig_P8);
	print_string(str1);
	sprintf(str1, "DIG_P9: %d\r\n", comp_bme280.dig_P9);
	print_string(str1);
	sprintf(str1, "DIG_H1: %d\r\n", comp_bme280.dig_H1);
	print_string(str1);
	sprintf(str1, "DIG_H2: %d\r\n", comp_bme280.dig_H2);
	print_string(str1);
	sprintf(str1, "DIG_H3: %u\r\n", comp_bme280.dig_H3);
	print_string(str1);
	sprintf(str1, "DIG_H4: %d\r\n", comp_bme280.dig_H4);
	print_string(str1);
	sprintf(str1, "DIG_H5: %d\r\n", comp_bme280.dig_H5);
	print_string(str1);
	sprintf(str1, "DIG_H6: %d\r\n", comp_bme280.dig_H6);
	print_string(str1);
}

//‘ќ–ћ”Ћџ ¬«я“џ »« ƒј“јЎ»“ј

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of У5123Ф equals 51.23 DegC.
// t_fine carries fine temperature as global value

int32_t parse_temp_bme280(compensation *dc){
  //char str[100];
  uint8_t temp_data[3]={0};
  uint32_t ADC_temp=0;
  int32_t  var1=0,var2=0,result=0;
  read_raw_data_bme280(TEMP_MSB, temp_data, 3);
  ADC_temp|=((uint32_t)temp_data[0]<<12)|(uint32_t)(temp_data[1]<<4)|(uint32_t)temp_data[2]>>4;
 // sprintf(str, "ADC_temp: %lu\r\n", ADC_temp);
 // print_string(str);
  var1 = ((((ADC_temp>>3) - ((int32_t)dc->dig_T1<<1))) * ((int32_t)dc->dig_T2)) >> 11;
  var2 = (((((ADC_temp>>4) - ((int32_t)dc->dig_T1)) * ((ADC_temp>>4) - ((int32_t)dc->dig_T1))) >> 12) *((int32_t)dc->dig_T3)) >> 14;
  dc->t_fine=var1+var2;
  result = (dc->t_fine * 5 + 128) >> 8;
//  sprintf(str, "temp: %0.2f\r\n", result/100);
 // print_string(str);
  return result;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of У24674867Ф represents 24674867/256 = 96386.2 Pa = 963.862 hPa
uint32_t parse_press_bme280(compensation *dc){
 // char str[100];
  uint8_t press_data[3]={0};
  volatile int32_t ADC_press=0;
  volatile int64_t  var1=0,var2=0,result=0;
  read_raw_data_bme280(PRESS_MSB, press_data, 3);
  ADC_press|=((int32_t)press_data[0]<<12)|(int32_t)(press_data[1]<<4)|(int32_t)press_data[2]>>4;
  //sprintf(str, "ADC_press: %ld\r\n", ADC_press);
  //print_string(str);
  var1 = ((int64_t)dc->t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)dc->dig_P6;
  var2 = var2 + ((var1*(int64_t)dc->dig_P5)<<17);
  var2 = var2 + (((int64_t)dc->dig_P4)<<35);
  var1 = ((var1 * var1 * (int64_t)dc->dig_P3)>>8) + ((var1 * (int64_t)dc->dig_P2)<<12);
  var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dc->dig_P1)>>33;
  if (var1 == 0) return 0; // avoid exception caused by division by zero
  result = 1048576-ADC_press;
  result= (((result<<31)-var2)*3125)/var1;
  var1 = (((int64_t)dc->dig_P9) * (result>>13) * (result>>13)) >> 25;
  var2 = (((int64_t)dc->dig_P8) * result) >> 19;
  result = ((result + var1 + var2) >> 8) + (((int64_t)dc->dig_P7)<<4);
 // sprintf(str, "press: %0.3F\r\n", result/256/133.3);
 // print_string(str);
  return (uint32_t)result;
}

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of У47445Ф represents 47445/1024 = 46.333 %RH
uint32_t parse_hum_bme280(compensation *dc){
  //char str[100];
  uint8_t hum_data[2]={0};
  uint32_t ADC_hum=0;
  int64_t  result=0;
  read_raw_data_bme280(HUM_MSB, hum_data, 2);
  ADC_hum|=((uint16_t)hum_data[0]<<8)|(uint16_t)hum_data[1] ;
 // sprintf(str, "ADC_hum: %ld\r\n", ADC_hum);
 // print_string(str);
  result= (dc->t_fine - ((int32_t)76800));
  result= (((((ADC_hum << 14) - (((int32_t)dc->dig_H4) << 20) - (((int32_t)dc->dig_H5) * result)) +
  ((int32_t)16384)) >> 15) * (((((((result * ((int32_t)dc->dig_H6)) >> 10) * (((result *((int32_t)dc->dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *((int32_t)dc->dig_H2) + 8192) >> 14));
  result = (result - (((((result >> 15) * (result >> 15)) >> 7) * ((int32_t)dc->dig_H1)) >> 4));
  result = (result < 0 ? 0 : result);
  result = (result > 419430400 ? 419430400 : result);
 // sprintf(str, "hum: %lld\r\n", (result>>12)/1024);
  //print_string(str);
  return (uint32_t)(result>>12);
}


void add_record(record *dt){
  //if(read_byte_i2c(BME280_ADDR, ID) != DEV_NAME ) { return ; };
  dt->temp=(float)parse_temp_bme280(&comp_bme280)/100;
  dt->press=parse_press_bme280(&comp_bme280)/256/133.3;
  dt->hum=(float)parse_hum_bme280(&comp_bme280)/1024;
}

//заводим любой таймер, настраиваем на нужную частоту прерываний и взводим флаг обновлени¤ данных в прерывании
//конкретно здесь используетс¤ прерывание таймера0 по каналу 1, которое обслуживает энкодер и кнопку
//ПО НЕЯСНОЙ ПРИЧИНЕ sprintf ЛОМАЛ работу меню. может стек переполнялся?
//UPD глюки продолжаются и без sprintf
//все проходит, как только убираю эту функцию из работы
void update_bme280(void){
	//char string[20];
	if(update_bme280_flag){
		add_record(&bme280_data);
		//sprintf(string,"TEMPERATURE  %0.1fC",bme280_data.temp);
		draw_string(4,4,"TEMPERATURE",0,BACKGROUND_COLOR,CYAN,SYSTEM_FONT);
		draw_float_number(105,4, bme280_data.temp,"%0.1f",SYSTEMFONT_SPACE,BACKGROUND_COLOR,CYAN,SYSTEM_FONT);
		draw_string(137,4,"C",0,BACKGROUND_COLOR,CYAN,SYSTEM_FONT);
		//sprintf(string,"HUMIDITY     %0.1f%%",bme280_data.hum);
		draw_string(4,14,"HUMIDITY",0,BACKGROUND_COLOR,RED,TinyFont); 
		draw_float_number(105,14, bme280_data.hum,"%0.1f",SYSTEMFONT_SPACE,BACKGROUND_COLOR,RED,SYSTEM_FONT);
		draw_string(137,14,"%",0,BACKGROUND_COLOR,RED,SYSTEM_FONT);
		//sprintf(string,"PRESSURE     %umm",bme280_data.press);
		draw_string(4,24,"PRESSURE",0,BACKGROUND_COLOR,YELLOW,TinyFont);  
		draw_number(105,24, bme280_data.press,SYSTEMFONT_SPACE,BACKGROUND_COLOR,YELLOW,SYSTEM_FONT);
		draw_string(130,24,"mm",0,BACKGROUND_COLOR,YELLOW,SYSTEM_FONT);
		update_bme280_flag=0;
	}
}




/*¬ дальнейшем дл¤ сбора данных, но это не точно*/
void init_stack(stack *stack, uint32_t capacity) {
  stack->capacity = capacity;                                                                                             // заносим количество элементов типа record в стеке
  if (capacity < 1) {
   // clear_display();
   // char *message="Stack capacity must be at least 1";
   // draw_string(0,0,message,-1,BACKGROUND_COLOR,RED,TinyFont);
    return;
  }
  stack->size = 0;                                                                                                        // заносим текущее количество элементов в стеке
  stack->measure = (datatype *)malloc(capacity * sizeof(datatype));                                                       // в указатель стека заносим адрес пам¤ти, выделенной под массив record
                                    
  if (stack->measure ==NULL) {                                                                                            // если malloc вернул NULL, значит пам¤ть не выделена
    //clear_display();
    //char *message="Memory allocation failed for stack";
   // draw_string(0,0,message,-1,BACKGROUND_COLOR,RED,TinyFont);
    return;
  }
}

void free_stack(stack *stack) {
  if (stack->measure) {
    free(stack->measure);                                                                                                 // освобождаем пам¤ть, выделенную под массив record
    stack->measure = NULL;                                                                                                // обнул¤ем указатель
  }
  stack->capacity = 0;                                                                                                    // обнул¤ем размер стека
  stack->size = 0;                                                                                                        // обнул¤ем текущее количество элементов в стеке
}

void push_data(stack *stack, datatype *data) {
  if (stack->size == stack->capacity - 1) {                                                                               // если размер стека равен его вместимости, увеличиваем размер стека
    stack->capacity *= 2;                                                                                                 // увеличиваем размер стека в 2 раза
    stack->measure = (datatype *)realloc(stack->measure, stack->capacity * sizeof(datatype));                             // получаем указатель на увеличенный в 2 раза участок пам¤ти
    if (stack->measure ==NULL) {                                                                                          // если realloc вернул NULL, значит пам¤ть не выделена
      //clear_display();
      //char *message="Memory allocation failed for stack";
      //draw_string(0,0,message,-1,BACKGROUND_COLOR,RED,TinyFont);
      free_stack(stack);
      return;
    }
  }
  stack->measure[stack->size++] = *data;                                                                                  // добавл¤ем элемент в стек и увеличиваем размер стека
}

uint8_t is_stack_empty(stack *st) {
  return (st->size == 0);                                                                                                 // возвращаем 1, если стек пуст, иначе 0
}

void pop_data(stack *st, datatype *data) {
  if (!is_stack_empty(st)) {                                                                                              // если в стеке есть элементы
    *data = st->measure[--st->size];                                                                                      // уменьшаем размер стека и возвращаем  последний элемент
  } else {
    //clear_display();
    //char *message="Stack is empty, cannot pop data";
    //draw_string(0,0,message,0,BACKGROUND_COLOR,RED,TinyFont);
    return;
  }
}


uint8_t read_data(stack *st,uint16_t num_measurements) {
  uint32_t press=0;
  float hum=0,temp=0;
  if(!is_stack_init){
    init_stack(&m_data, 1); 
    is_stack_init=1;
  }
  if(st->capacity<=num_measurements){
    datatype add = {press,hum, temp};                                                                                       //вносим в структуру record считанные данные
    add_record(&add);
    push_data(st, &add);                                                                                                    //добавл¤ем данные в стек
  
  }else{
   free_stack(st);
   is_stack_init=0;
 }
  return SUCCESS;
}

float average_temp(stack *dt) {
  float result = 0;
  if (!dt->size)
    return 0;
  for (uint16_t i = 0; i < STACK_SIZE; i++) {
    result += dt->measure[i].temp;
  }
  return result / dt->size;
}


//void print_temp(uint16_t xpos, uint16_t ypos, int8_t space, uint16_t bcolor,  uint16_t fcolor, record * dt, uint8_t *font){
   //char message[10]="";
  //sprintf(message, "%s %0.1f%c","TEMP: ", dt->temp, 'C');
   //draw_string(xpos,ypos,message,space,bcolor,fcolor,font); 
//}
//
//void print_hum(uint16_t xpos, uint16_t ypos, int8_t space, uint16_t bcolor,  uint16_t fcolor, record * dt, uint8_t *font){
   //char message[10]="";
  //sprintf(message, "%s %0.1f%c","HUM:  ", dt->hum, '%');
   //draw_string(xpos,ypos,message,space,bcolor,fcolor,font); 
//}
//
//void print_press(uint16_t xpos, uint16_t ypos, int8_t space, uint16_t bcolor,  uint16_t fcolor, record * dt, uint8_t *font){
   //char message[10]="";
  //sprintf(message, "%s %u %s","PRESS:", dt->press, "mm");
   //draw_string(xpos,ypos,message,space,bcolor,fcolor,font); 
//}

//void vent_autoreg(uint16_t xpos, uint16_t ypos, int8_t space, uint16_t bcolor,  uint16_t fcolor, record * dt, uint8_t *font){
  //char message[20]="";
  //uint8_t speed=(uint8_t)dt->hum/1;
  //set_ventspeed(speed);
  //sprintf(message,"%s %u","VENTSPEED",speed);
  //draw_string(xpos,ypos,message,space,bcolor,fcolor,font);
//}











