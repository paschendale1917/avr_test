#define F_CPU 16000000UL
#include "st7735.h"

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

void st7735_multiply_send(uint8_t *data, uint32_t num){
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
	send_data(0x01);
	send_data(0x2C);
	send_data(0x2D);
	send_command(ST7735_FRMCTR2);
	send_data(0x01);
	send_data(0x2C);
	send_data(0x2D);
	send_command(ST7735_FRMCTR3);
	send_data(0x01);
	send_data(0x2C);
	send_data(0x2D);
	send_data(0x01);
	send_data(0x2C);
	send_data(0x2D);
	send_command(ST7735_INVCTR);
	send_data(0x07);
	send_command(ST7735_PWCTR1);
	send_data(0xA2);
	send_data(0x02);
	send_data(0x84);
	send_command(ST7735_PWCTR2);
	send_data(0xC5);
	send_command(ST7735_PWCTR3);
	send_data(0x0A);
	send_data(0x00);
	send_command(ST7735_PWCTR4);
	send_data(0x8A);
	send_data(0x2A);
	send_command(ST7735_PWCTR5);
	send_data(0x8A);
	send_data(0xEE);
	send_command(ST7735_VMCTR1);
	send_data(0x0E);
	send_command(ST7735_INVON);
	send_command(ST7735_MADCTL);
	send_data(MADCTL); //0b01110000
	send_command(ST7735_COLMOD);
	send_data(0x05);
	send_command(ST7735_GMCTRP1);
	send_data(0x02);
	send_data(0x1c);
	send_data(0x07);
	send_data(0x12);
	send_data(0x37);
	send_data(0x32);
	send_data(0x29);
	send_data(0x2d);
	send_data(0x29);
	send_data(0x25);
	send_data(0x2B);
	send_data(0x39);
	send_data(0x00);
	send_data(0x01);
	send_data(0x03);
	send_data(0x10);
	send_command(ST7735_GMCTRN1);
	send_data(0x03);
	send_data(0x1d);
	send_data(0x07);
	send_data(0x06);
	send_data(0x2E);
	send_data(0x2C);
	send_data(0x29);
	send_data(0x2D);
	send_data(0x2E);
	send_data(0x2E);
	send_data(0x37);
	send_data(0x3F);
	send_data(0x00);
	send_data(0x00);
	send_data(0x02);
	send_data(0x10);
	send_command(ST7735_NORON);
	_delay_ms(10);
	send_command(ST7735_DISPON);
	_delay_ms(100);
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



void draw_rect(uint16_t xstart, uint16_t ystart, uint16_t lenth, uint16_t height, uint16_t color){
	START_Tx;
	setXY(xstart, lenth, ystart, height);
	send_command(ST7735_RAMWR);
	ST7735_DATA;
	for(uint32_t i = 0; i < lenth * height; i++) {
		send_data((uint8_t)(color>>8));
		send_data((uint8_t)color);
	}

	STOP_Tx;
}

void background(uint16_t color){
	draw_rect(0,0,LCD_WIDTH_SIZE ,LCD_HEIGHT_SIZE,color);
}

void draw_image(uint16_t x_start, uint16_t x_stop, uint16_t y_start, uint16_t y_stop, const uint16_t *image){
	uint16_t s=0;
	START_Tx;
	setXY(x_start,x_stop,y_start,y_stop);
	send_command(ST7735_RAMWR);
	ST7735_DATA;
	while(s++ < x_stop*y_stop){
		spi_sendbyte((uint8_t)image[s]);
		spi_sendbyte((uint8_t)(image[s]>>8));
	}
	STOP_Tx;
}


void draw_char(uint16_t xstart, uint16_t ystart, const char letter, uint16_t bcolor, uint16_t fcolor, uint8_t *font){
	uint8_t data_w = pgm_read_byte(font)/8 ;                                     //ширина шрифта в байтах
	volatile uint16_t offset = (letter - pgm_read_byte(font+2)) * pgm_read_byte(font+1) * data_w + 4;     //смещение начала искомого символа в массиве , описывающем символы//4-первые 4 байта с инфой о шрифте, letter-численное представление символа по таблице ascii, font[2]==32(0х20)-значение смещения символов в таблице ascii(в имеющемся шрифте нет первых 32 значков из таблицы),font[1]*data_w -вычисление, сколько байт нужно забрать(размер буквы в байтах)
	uint8_t lettersize = pgm_read_byte(font+1) * data_w;
	uint8_t letter_buf[400] = {0};                                  //размер массива обуславливается размером шрифтов, например, для шрифта 16*16=256 байт.Ставлю с запасом, так как шрифты есть бОльшего размера
	uint8_t data = 0;
    for(uint16_t i = 0; i < lettersize; i++) {                        //забираем байты буквы из массива, описывающего шрифт
	    letter_buf[i] = pgm_read_byte(font+offset + i);
    }
	START_Tx;
	setXY(xstart, pgm_read_byte(font+1), ystart, pgm_read_byte(font+1));                          //по х ширина символа, по у высота символа
	send_command(ST7735_RAMWR);                                       //команда записи в память дисплея
	ST7735_DATA;
	for(uint16_t  u = 0; u < lettersize; u += data_w) {
		for(uint8_t w = 0; w < data_w; w++) {
			data = letter_buf[u + w];
			for(uint8_t j = 0; j < 8; j++) {                              //отправка побитово в дисплей
				if(data & 0x80) {
					spi_sendbyte((uint8_t)(fcolor>>8));
					spi_sendbyte((uint8_t)fcolor);
					} else {
					spi_sendbyte((uint8_t)(bcolor>>8));
					spi_sendbyte((uint8_t)bcolor);
				}
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

void draw_number(uint16_t xpos,  uint16_t ypos, int32_t number, int8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font){
	char buf[10] = {0};
	uint8_t len = sprintf(buf, "%li", number);					//определяем длину числа
	memset(buf + len, ' ', sizeof(buf) - len - 1);				// очищаем лишние позиции                                                                                   
	draw_string(xpos,ypos,buf,space,bcolor,fcolor,font);
}

void draw_hexnumber(uint16_t xpos,  uint16_t ypos, int32_t number, uint8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font){
    char buf[10] = {0};
	sprintf(buf, "%s%lx","0x", number);
	draw_string(xpos, ypos, buf, space, bcolor, fcolor, font);
}

//draw_float_number(60,70,84.67,"%0.2f",0,BACKGROUND_COLOR,GREEN,TinyFont);
void draw_float_number(uint16_t xpos,  uint16_t ypos, float number, const char *dimens, uint8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font){
	char buf[10] = {0};
	sprintf(buf, dimens, number);
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

void draw_minutes(uint16_t xpos,  uint16_t ypos, int32_t number, uint8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font)
{
	draw_number(xpos + font[0] + space, ypos, number % 10, space, bcolor, fcolor, font);
	draw_number(xpos, ypos, number / 10 % 10, space, bcolor, fcolor, font);

}

