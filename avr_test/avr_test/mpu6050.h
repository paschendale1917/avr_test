#ifndef MPU6050_H_
#define MPU6050_H_

#include "i2c.h"

#define MPU6050_ADDR					0xD0

//–егистры 
#define MPU_CONFIG						0x1A
#define FILTER_0                        0x00
#define FILTER_1                        0x01
#define FILTER_2                        0x02
#define FILTER_3                        0x03
#define FILTER_4                        0x04
#define FILTER_5                        0x05
#define FILTER_6                        0x06

#define CONFIG_GYRO                     0x1B
#define FS_SEL_0                        (0<<3)
#define FS_SEL_1                        (1<<3)
#define FS_SEL_2                        (2<<3)
#define FS_SEL_3                        (3<<3)

#define CONFIG_ACCEL					0x1C 
#define AFS_SEL_0                       (0<<3)
#define AFS_SEL_1                       (1<<3)
#define AFS_SEL_2                       (2<<3)
#define AFS_SEL_3                       (3<<3)  

#define ACCEL_XOUT_H					0x3B
#define ACCEL_XOUT_L					0x3C
#define ACCEL_YOUT_H					0x3D
#define ACCEL_YOUT_L					0x3E          
#define ACCEL_ZOUT_H					0x3F
#define ACCEL_ZOUT_L					0x40

#define TEMP_OUT_H						0x41
#define TEMP_OUT_L						0x42

#define GYRO_XOUT_H						0x43
#define GYRO_XOUT_L						0x44
#define GYRO_YOUT_H						0x45
#define GYRO_YOUT_L						0x46
#define GYRO_ZOUT_H						0x47
#define GYRO_ZOUT_L						0x48

#define SIGNAL_PATH_RESET				0x68
#define GYRO_RESET						(1<<2) 
#define ACCEL_RESET						(1<<1) 
#define TEMP_RESET						(1<<0) 

#define PWR_MGMNT						0x6B
#define MPU6050_RESET                   (1<<7)
#define MPU6050_SLEEP                   (1<<6)
#define MPU6050_CYCLE                   (1<<5) 
#define MPU6050_TEMP_OFF                (1<<3)

#define WHO_AM_I						0x75

typedef struct {
	int16_t accel_xout;  
	int16_t accel_yout;
	int16_t accel_zout;
	int16_t  temp_out;
	int16_t gyro_xout;
	int16_t gyro_yout;
	int16_t gyro_zout;
} mpu6050_data;

extern mpu6050_data mpu6050_dt;
extern mpu6050_data mpu6050_ready_dt;

void mpu6050_init(void);
uint8_t whoami(void);
float mpu6050_temp(void);
void mpu6050_init(void);
uint8_t mpu6050_ready_data(mpu6050_data *dt);
void print_mpu6050_data(void);
#endif /* MPU6050_H_ */