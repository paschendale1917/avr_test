#define F_CPU 16000000UL
#include "st7735.h"

const uint8_t frame_ctrl_data[] = {0x01, 0x2C, 0x2D};
const uint8_t pwctr1_data[] = {0xA2, 0x02, 0x84};
const uint8_t gmctrp1_data[] = {
	0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d,
	0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10
};

const uint8_t gmctrn1_data[] = {
	0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D,
	0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10
};

	
void st7735_ports_init(void){
SET_BIT(SPI_DDR,(1<<BKL));   // подсветка
SET_BIT(SPI_DDR,(1<<DC));    // данные/команда
SET_BIT(SPI_DDR,(1<<RESET)); // сброс
SET_BIT(SPI_DDR,(1<<CS));    // чип селект
}

void send_command(uint8_t byte){
	ST7735_COMMAND;
	spi_sendbyte(byte);
}

void send_data(uint8_t byte){
	ST7735_DATA;
	spi_sendbyte(byte);
}

void st7735_multiply_send(const uint8_t *data, uint32_t num){
	ST7735_DATA;
	for (uint32_t i = 0; i < num; i++)
	{
		send_data(*(data+i));	
	}
}

void st7735_init(void){

	START_Tx;
	ST7735_RESET_1;
	_delay_ms(5);
	ST7735_RESET_0;
	_delay_ms(5);
	ST7735_RESET_1;
	_delay_ms(5);
	send_command(ST7735_SWRESET);
	_delay_ms(150);
	send_command(ST7735_SLPOUT);
	_delay_ms(500);
	send_command(ST7735_FRMCTR1);
	st7735_multiply_send(frame_ctrl_data, sizeof(frame_ctrl_data));
	send_command(ST7735_INVCTR);
	send_data(0x07);
	send_command(ST7735_PWCTR1);
	st7735_multiply_send(pwctr1_data, sizeof(pwctr1_data));
	send_command(ST7735_VMCTR1);
	send_data(0x0E);
#ifdef STN_PORTRAIT
	send_command(ST7735_INVOFF);
#elif defined(STN_LANDSCAPE)
	send_command(ST7735_INVOFF);
#elif defined(IPS_LANDSCAPE)
	send_command(ST7735_INVON);
#endif
	send_command(ST7735_MADCTL);
	send_data(MADCTL); //0b01110000
	send_command(ST7735_COLMOD);
	send_data(0x05);
	send_command(ST7735_GMCTRP1);
	st7735_multiply_send(gmctrp1_data, sizeof(gmctrp1_data));
	send_command(ST7735_GMCTRN1);
	st7735_multiply_send(gmctrn1_data, sizeof(gmctrn1_data));
	send_command(ST7735_NORON);
	_delay_ms(ST7735_SPI_TIMEOUT);
	send_command(ST7735_DISPON);
	_delay_ms(ST7735_SPI_TIMEOUT);
	STOP_Tx;
}


void setXY(uint16_t xstart,  uint16_t xstop, uint16_t ystart,  uint16_t ystop){
	send_command(ST7735_RASET);
	send_data((ystart + LCD_WIDTH_OFFSET) >> 8);
	send_data((uint8_t)ystart + LCD_WIDTH_OFFSET);
	send_data((ystart + ystop + LCD_WIDTH_OFFSET - 1) >> 8);
	send_data((uint8_t)ystart + ystop + LCD_WIDTH_OFFSET - 1);

	send_command(ST7735_CASET);
	send_data((xstart + LCD_HEIGHT_OFFSET) >> 8);
	send_data((uint8_t)xstart + LCD_HEIGHT_OFFSET);
	send_data((xstart + xstop + LCD_HEIGHT_OFFSET - 1) >> 8);
	send_data((uint8_t)xstart + xstop + LCD_HEIGHT_OFFSET - 1);
}



void draw_rect(uint16_t xstart, uint16_t ystart, uint16_t height, uint16_t lenth,  uint16_t color){
	START_Tx;
	setXY(xstart, height, ystart, lenth);
	send_command(ST7735_RAMWR);
	for(uint32_t i = 0; i < lenth * height; i++) {
		send_data((uint8_t)(color>>8));
		send_data((uint8_t)color);
	}

	STOP_Tx;
}

void draw_hline(uint16_t xstart, uint16_t ystart, uint16_t lenth,  uint16_t color){
	draw_rect( xstart, ystart, lenth,2,color);
}

void draw_vline(uint16_t xstart, uint16_t ystart, uint16_t height,  uint16_t color){
	draw_rect( xstart, ystart, 2,height,color);
}

void draw_border80x160(uint16_t color){
	draw_hline(0,0,160,color);
	draw_hline(0,78,160,color);
	draw_vline(0,0,80,color);
	draw_vline(158,0,80,color);
}

void background(uint16_t color){
	draw_rect(0,0,LCD_WIDTH_SIZE ,LCD_HEIGHT_SIZE,color);
}

void clear_display(void){
	background(BACKGROUND_COLOR);
}

void draw_image(uint16_t x_start, uint16_t y_start, uint16_t height, uint16_t lenth,  const unsigned short *image){
	START_Tx;
	setXY(x_start, height, y_start, lenth);
	send_command(ST7735_RAMWR);
	for(uint32_t i = 0; i < lenth * height; i++) {
		uint16_t color = pgm_read_word(&image[i]);
		send_data(color >> 8);      // Старший байт (R/G)
		send_data((uint8_t)color);    // Младший байт (B)
	}
	STOP_Tx;
}


void draw_char(uint16_t xstart, uint16_t ystart, const char letter, uint16_t bcolor,uint16_t fcolor,uint8_t *font) {
	
	// Получаем параметры шрифта
	const uint8_t data_w = pgm_read_byte(font) / 8;     // ширина в байтах
	const uint8_t letter_height = pgm_read_byte(font + 1);  // высота символа
	const uint8_t letter_width = data_w * 8;            // ширина символа в пикселях
	const uint8_t ascii_offset = pgm_read_byte(font + 2);  // смещение ASCII
	
	// Вычисляем смещение символа в массиве
	const uint16_t offset = (letter - ascii_offset) * letter_height * data_w + 4;
	const uint16_t lettersize = letter_height * data_w;
	
	// Буфер для хранения данных символа
	uint8_t letter_buf[400] = {0};
	
	// Считываем данные символа из памяти
	for(uint16_t i = 0; i < lettersize; i++) {
		letter_buf[i] = pgm_read_byte(font + offset + i);
	}
	
	// Устанавливаем позицию и режим записи
	START_Tx;
	setXY(xstart, letter_width, ystart, letter_height);  // Указываем реальные размеры символа
	send_command(ST7735_RAMWR);
	ST7735_DATA;
	
	for(uint16_t y = 0; y < letter_height; y++) {
		for(uint16_t x = 0; x < letter_width; x += 8) {
			uint8_t data = letter_buf[y * data_w + x / 8];
			for(uint8_t bit = 0; bit < 8; bit++) {
				if (x + bit >= letter_width) break;			// Защита от выхода за границы
				spi_sendbyte((data & 0x80) ? (fcolor >> 8) : (bcolor >> 8));
				spi_sendbyte((data & 0x80) ? fcolor : bcolor);
				data <<= 1;
			}
		}
	}
	
	STOP_Tx;
}


void draw_string(uint16_t xpos, uint16_t ypos, const char *string, int8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font){
	while(*string != 0) {
		draw_char(xpos, ypos, *string++, bcolor, fcolor, font);
		xpos += (uint16_t)(pgm_read_byte(font) + space);
	}
}

void draw_string_progmem(uint16_t xpos, uint16_t ypos, const char *string, int8_t space, uint16_t bcolor, uint16_t fcolor, const uint8_t *font) {
    char current_char;
    while((current_char = pgm_read_byte(string)) != 0) {
        draw_char(xpos, ypos,current_char, bcolor, fcolor, font);
        xpos += (uint16_t)(pgm_read_byte(font) + space);
        string++;
    }
}
void draw_dot(uint16_t x, uint16_t y, uint16_t color)
{
	START_Tx;
	setXY(x, 0, y, 1);
	send_command(ST7735_RAMWR);
	ST7735_DATA;
	send_data((uint8_t)(color>>8));
	send_data((uint8_t)color);
	STOP_Tx;
}

void draw_circle(uint8_t x, uint8_t y, uint8_t r, uint16_t color){
	int16_t f = 1 - r;
	int16_t x0 = 0;
	int16_t y0 = r;
	while(x0 <= y0) {
		draw_dot(x + x0, y + y0, color);
		draw_dot(x - x0, y + y0, color);
		draw_dot(x + x0, y - y0, color);
		draw_dot(x - x0, y - y0, color);

		draw_dot(x + y0, y + x0, color);
		draw_dot(x - y0, y + x0, color);
		draw_dot(x + y0, y - x0, color);
		draw_dot(x - y0, y - x0, color);
		x0++;
		if(f > 0) {
			y0--;
			f += 2 * (x0 - y0) + 1;
		} else{
			 f += 2 * x0 + 1; }
	}
}

/*
void draw_number(uint16_t xpos,  uint16_t ypos, int32_t number, int8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font){
	char buf[10] = {0};
	/ *uint8_t len = * /sprintf(buf, "%li", number);					//определяем длину числа
	//memset(buf + len, ' ', sizeof(buf) - len - 1);				// очищаем лишние позиции                                                                                   
	draw_string(xpos,ypos,buf,space,bcolor,fcolor,font);
}*/

void draw_number(uint16_t xpos, uint16_t ypos, int32_t number, int8_t space, uint16_t bcolor, uint16_t fcolor, uint8_t *font) {
	char buf[12];
	uint8_t idx = 11;
	buf[idx--] = '\0'; // Конец строки
	
	int32_t n = number;
	uint8_t is_negative = 0;
	
	// Обработка отрицательных чисел
	if (n < 0) {
		is_negative = 1;
		n = -n;
	}
	
	// Преобразуем цифры с конца
	do {
		buf[idx--] = '0' + (n % 10);
		n /= 10;
	} while (n > 0 && idx > 0);
	
	// Добавляем знак минус если нужно
	if (is_negative && idx > 0) {
		buf[idx--] = '-';
	}
	
	// Добавляем ведущие пробелы
	if (space > 0) {
		uint8_t current_len = 11 - idx; // ИСПРАВЛЕНО: было 11 - idx - 1
		if (current_len < space) {
			uint8_t spaces_needed = space - current_len;
			while (spaces_needed > 0 && idx > 0) {
				buf[idx--] = ' ';
				spaces_needed--;
			}
		}
	}
	
	// Передаем указатель на начало числа
	draw_string(xpos, ypos, &buf[idx + 1], space, bcolor, fcolor, font);
}

void draw_hexnumber(uint16_t xpos,  uint16_t ypos, int32_t number, uint8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font){
    char buf[10] = {0};
	sprintf(buf, "%s%lx","0x", number);
	draw_string(xpos, ypos, buf, space, bcolor, fcolor, font);
}

//draw_float_number(60,70,84.67,"%0.2f",0,BACKGROUND_COLOR,GREEN,TinyFont);
/*
void draw_float_number(uint16_t xpos,  uint16_t ypos, float number, const char *dimens, uint8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font){
	char buf[10] = {0};
	sprintf(buf, dimens, number);
	draw_string(xpos, ypos, buf, space, bcolor, fcolor, font);
}*/

void draw_float_number(uint16_t xpos, uint16_t ypos, float number,  uint8_t space, uint16_t bcolor, uint16_t fcolor, uint8_t *font) {
	char buf[12] = {0};
	uint8_t idx = 0;

	// Обработка знака
	if (number < 0) {
		buf[idx++] = '-';
		number = -number;
	}
	
	// Целая и дробная части
	int16_t integer_part = (int16_t)number;
	int16_t fractional = (int16_t)((number - integer_part) * 100 + 0.5); // ×100 и округление
	
	// Целая часть
	if (integer_part == 0) {
		buf[idx++] = '0';
		} else {
		uint8_t digits[5];
		uint8_t digit_count = 0;
		int16_t temp = integer_part;
		
		while (temp > 0 && digit_count < 5) {
			digits[digit_count++] = temp % 10;
			temp /= 10;
		}
		
		for (int8_t i = digit_count - 1; i >= 0; i--) {
			buf[idx++] = '0' + digits[i];
		}
	}
	
	// Дробная часть
	buf[idx++] = '.';
	buf[idx++] = '0' + (fractional / 10) % 10;
	
	buf[idx] = '\0';
	draw_string(xpos, ypos, buf, space, bcolor, fcolor, font);
}

/*потом поковырять*/
uint8_t draw_2num_zero(uint16_t xpos,  uint16_t ypos, uint8_t number, uint8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font){
	if(number > 9 || number < 1)
	{ return 0; }

	uint8_t tens, units;
	int32_t temp_num = number;
	int32_t *pt_num;
	pt_num = &temp_num;
	int32_t num = 0;
	num = *pt_num;

	char buf_in[2] = {0}; //разделение числа на цифры и складирование в буфер
	char buf_out[2] = {0};
	num = *pt_num;
	tens = num / 10;
	units = num % 10;
	buf_in[0] = tens;
	buf_in[1] = units;

	for(uint8_t j = 0; j < 2; j++) {   //преобразование буфера в строку
		sprintf(&buf_out[j], "%d", buf_in[j]);
	}


	for(uint8_t y = 0; y < 2; y++) {   //вывод строки с положительным числом
		draw_char(xpos, ypos, *(buf_out + y), bcolor, fcolor, font);
		ypos += font[0] + space;
	}

	return 1;
}



void draw_clocks(uint16_t xpos,  uint16_t ypos, int32_t number, uint8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font){
	draw_number(xpos + (font[0] + space) * 2, ypos, number % 10, space, bcolor, fcolor, font);
	draw_number(xpos + font[0] + space, ypos, number / 10 % 10, space, bcolor, fcolor, font);
	draw_number(xpos, ypos, number / 100 % 10, space, bcolor, fcolor, font);
}

void draw_minutes(uint16_t xpos,  uint16_t ypos, int32_t number, uint8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font){
	draw_number(xpos + font[0] + space, ypos, number % 10, space, bcolor, fcolor, font);
	draw_number(xpos, ypos, number / 10 % 10, space, bcolor, fcolor, font);

}

