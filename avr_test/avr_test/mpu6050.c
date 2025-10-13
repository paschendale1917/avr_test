#include "mpu6050.h"

mpu6050_data mpu6050_dt;
mpu6050_data mpu6050_ready_dt;
 
uint8_t mpu6050_readbyte(uint8_t dev_addr, uint8_t reg){
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

uint8_t mpu6050_sendbyte(uint8_t dev_addr, uint8_t reg,uint8_t data){
	start_cond();
	i2c_sendbyte(dev_addr);
	i2c_sendbyte(reg);
	i2c_sendbyte(data);
	stop_cond();
	return SUCCESS;
}

void mpu6050_init(void){
	mpu6050_sendbyte(MPU6050_ADDR,CONFIG_ACCEL,AFS_SEL_0);
	mpu6050_sendbyte(MPU6050_ADDR,CONFIG_GYRO,FS_SEL_0);
	mpu6050_sendbyte(MPU6050_ADDR,PWR_MGMNT,0x00);
}

uint8_t whoami(void){
	return mpu6050_readbyte(MPU6050_ADDR,WHO_AM_I);
}


int16_t twos_complement_convert(uint16_t value) {
	if (value & 0x8000) {
		value = ~value;
		value++;
		return -(int16_t)value;
	}
	return (int16_t)value;
}


uint8_t mpu6050_readdata(uint8_t dev_addr, uint8_t reg,uint8_t *data,uint8_t num_bytes){
	uint8_t status=0;
	int16_t sum=0; 
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
			*(data+cntr) = i2c_readbyte(ACK);								// Отправляем ACK для всех, кроме последнего байта
			} else {
			*(data+cntr)= i2c_readbyte(NACK);								// Отправляем NACK для последнего байта
		}
	}
	stop_cond();
	for(uint8_t i=0;i<num_bytes;i++)sum+=data[i];
	!sum?mpu6050_init():0;
	return SUCCESS;
}

uint8_t read_raw_data_mpu6050(uint8_t start_addr, uint8_t *data,  uint8_t num_bytes){
	mpu6050_readdata(MPU6050_ADDR, start_addr, data, num_bytes);
	return SUCCESS;
}

int16_t convert_accel(int16_t raw, uint8_t range) {
	switch(range) {
		case 0: return raw/16384;
		case 1: return raw/8192;
		case 2: return raw/4096;
		case 3: return raw/2048;
		default: return 0.0;
	}
}


int16_t convert_gyro(int16_t raw, uint8_t range) {
	switch(range) {
		case 0: return raw/131.0;
		case 1: return raw/65.0;
		case 2: return raw/33.0;
		case 3: return raw/16.4;
		default: return 0;
	}
}

uint8_t mpu6050_ready_data(mpu6050_data *dt){
	static uint8_t raw_data[14] = {0};
	read_raw_data_mpu6050(ACCEL_XOUT_H, raw_data, 14);
    dt->accel_xout=convert_accel(twos_complement_convert(raw_data[0]<<8|raw_data[1]),0);
	dt->accel_yout=convert_accel(twos_complement_convert(raw_data[2]<<8|raw_data[3]),0);
	dt->accel_zout=convert_accel(twos_complement_convert(raw_data[4]<<8|raw_data[5]),0);
	dt->temp_out=(int16_t)raw_data[6]<<8|(int8_t)raw_data[7];
	dt->gyro_xout=convert_gyro(twos_complement_convert(raw_data[0]<<8|raw_data[1]),0);
	dt->gyro_yout=convert_gyro(twos_complement_convert(raw_data[2]<<8|raw_data[3]),0);
	dt->gyro_zout=convert_gyro(twos_complement_convert(raw_data[4]<<8|raw_data[5]),0);
	return SUCCESS;
}


float mpu6050_temp(void){
	return (float)(mpu6050_dt.temp_out/340+36.53);
}

void print_mpu6050_data(void){
	char str1[20];
	sprintf(str1, "accel_xout: %d\r\n", mpu6050_dt.accel_xout);
	print_string(str1);
	sprintf(str1, "accel_yout: %d\r\n", mpu6050_dt.accel_yout);
	print_string(str1);
	sprintf(str1, "accel_zout: %d\r\n", mpu6050_dt.accel_zout);
	print_string(str1);
	sprintf(str1, "gyro_xout: %d\r\n", mpu6050_dt.gyro_xout);
	print_string(str1);
	sprintf(str1, "gyro_yout: %d\r\n", mpu6050_dt.gyro_yout);
	print_string(str1);
	sprintf(str1, "gyro_zout: %d\r\n", mpu6050_dt.gyro_zout);
	print_string(str1);
	sprintf(str1, "temp_out: %d\r\n", mpu6050_dt.temp_out);
	print_string(str1);
}


