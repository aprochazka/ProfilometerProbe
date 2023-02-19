

#include "OV5642_regs.h"
#include "stm32l4xx_hal.h"

void CS_Off();
void CS_On();

void LED_On();
void LED_Off();
void Debug_LED_On();
void Debug_LED_Off();

void SPI_Init(SPI_HandleTypeDef *hspi);
int Cam_SPI_read(SPI_HandleTypeDef *hspi, uint8_t address);
int Cam_SPI_write(SPI_HandleTypeDef *hspi, uint8_t addr, uint8_t data);
int Cam_I2C_write(I2C_HandleTypeDef *hi2c, uint16_t address, uint8_t data);
int Cam_I2C_write_struct(I2C_HandleTypeDef *hi2c, sensor_reg reg);
void Cam_I2C_write_bulk(I2C_HandleTypeDef *hi2c, const struct sensor_reg regList[]);
void Cam_Init(I2C_HandleTypeDef *hi2c, SPI_HandleTypeDef *hspi);
int Cam_FIFO_length(SPI_HandleTypeDef *hspi);
void Cam_Capture(SPI_HandleTypeDef *hspi);

void Cam_Start_Capture(SPI_HandleTypeDef *hspi);
void Cam_Wait_Capture_Done(SPI_HandleTypeDef *hspi);
void Cam_Start_Burst_Read(SPI_HandleTypeDef *hspi);
