/**
 ******************************************************************************
 * @file           : Cam.c
 * @brief          : Interacting with camera module
 *                   This file contains the methods for communication with camera module, including I2C and SPI communication, sending commands to camera and reading data from it.
 * @author         : Adam Prochazka <xproch0f>
 ******************************************************************************
 */

#include "Cam.h"

void CS_Off()
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
}

void CS_On()
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
}

void LED_On()
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
}

void LED_Off()
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
}

void Debug_LED_On()
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
}

void Debug_LED_Off()
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
}

void SPI_Init(SPI_HandleTypeDef *hspi)
{
    uint8_t dummyReceiver = 0x00;
    CS_On();
    HAL_SPI_Transmit(hspi, &dummyReceiver, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(hspi, (uint8_t *)&dummyReceiver, 1, HAL_MAX_DELAY);
    CS_Off();
}

int Cam_SPI_read(SPI_HandleTypeDef *hspi, uint8_t address)
{
    uint8_t addrMasked = address & 0x7F;
    uint8_t empty = 0x00;
    uint8_t ret;

    CS_On();

    HAL_SPI_TransmitReceive(hspi, &addrMasked, &ret, 1, HAL_MAX_DELAY);
    HAL_SPI_TransmitReceive(hspi, &empty, &ret, 1, HAL_MAX_DELAY);

    CS_Off();

    return ret;
}

int Cam_SPI_write(SPI_HandleTypeDef *hspi, uint8_t addr, uint8_t data)
{
    HAL_StatusTypeDef ret;
    uint8_t addr_write = addr | 0x80;

    CS_On();

    ret = HAL_SPI_Transmit(hspi, (uint8_t *)&addr_write, 1, HAL_MAX_DELAY);
    if (ret != HAL_OK)
    {
        CS_Off();
        return ret;
    }

    ret = HAL_SPI_Transmit(hspi, (uint8_t *)&data, 1, HAL_MAX_DELAY);
    if (ret != HAL_OK)
    {
        CS_Off();
        return ret;
    }

    CS_Off();
    return 0;
}

int Cam_I2C_write(I2C_HandleTypeDef *hi2c, uint16_t address, uint8_t data)
{
    HAL_StatusTypeDef ret;
    uint8_t buf[3];

    buf[0] = address >> 8;
    buf[1] = address & 0x00ff;
    buf[2] = data;
    ret = HAL_I2C_Master_Transmit(hi2c, 0x78, buf, 3, HAL_MAX_DELAY);

    if (ret == HAL_OK)
        return (1);

    return (0);
}

int Cam_I2C_write_struct(I2C_HandleTypeDef *hi2c, sensor_reg reg)
{
    return Cam_I2C_write(hi2c, reg.reg, reg.val);
}

void Cam_I2C_write_bulk(I2C_HandleTypeDef *hi2c, const struct sensor_reg regList[])
{
    unsigned int i = 0;
    do
    {
        struct sensor_reg reg = regList[i];
        if ((reg.reg == 0xffff) & (reg.val == 0xff))
        {
            break;
        }
        else
        {
            Cam_I2C_write_struct(hi2c, regList[i]);
        }
    } while (++i);
}

void Cam_Init(I2C_HandleTypeDef *hi2c, SPI_HandleTypeDef *hspi)
{
    // Test SPI communication
    Cam_SPI_write(hspi, 0x07, 0x80);
    HAL_Delay(100);
    Cam_SPI_write(hspi, 0x07, 0x00);
    HAL_Delay(100);

    // Reset camera chip
    Cam_I2C_write(hi2c, (uint16_t)0x3008, 0x80);
    
    // Delay for reset complete.
    HAL_Delay(5);

    // Set registers for desired configuration - need to be in this specific order
    Cam_I2C_write_bulk(hi2c, OV5642_QVGA_Preview);
    HAL_Delay(200); // Delay for first configuration to take effect
    Cam_I2C_write_bulk(hi2c, OV5642_JPEG_Capture_QSXGA);
    Cam_I2C_write_bulk(hi2c, ov5642_1024x768);

    HAL_Delay(100);
    
    Cam_I2C_write(hi2c, (uint16_t)0x3818, 0xa8); // TIMING CONTROL - ENABLE COMPRESSION, THUMBNAIL MODE DISABLE, VERTICAL FLIP, MIRROR
    Cam_I2C_write(hi2c, (uint16_t)0x3621, 0x10); // REGISTER FOR CORRECT MIRROR FUNCTION
    Cam_I2C_write(hi2c, (uint16_t)0x3801, 0xb0); // TIMING HORIZONTAL START - ALSO FOR MIRROR
    Cam_I2C_write(hi2c, (uint16_t)0x4407, 0x09); // COMPRESSION CONTROL

    HAL_Delay(5);
    
    // H-sync: High, V-sync:high, Sensor_delay: no Delay, FIFO_mode:FIFO enabled, power_mode:Low_power
    Cam_SPI_write(hspi, 0x03, 0x02);

    HAL_Delay(5);

    Cam_I2C_write_bulk(hi2c, ov5642_1024x768); // Necessary to be called again

    HAL_Delay(1000); // Delay for camera to stabilize
    
    Cam_SPI_write(hspi, 0x04, 0x01); // Reset camera capture status and memory flags
    
    HAL_Delay(5);
    
    Cam_SPI_write(hspi, 0x01, 0x00); // Capture Control Register - Set to capture n+1 frames
    
    HAL_Delay(5);
}

int Cam_FIFO_length(SPI_HandleTypeDef *hspi)
{
    uint32_t len1, len2, len3, len = 0;
    len1 = Cam_SPI_read(hspi, 0x42);
    len2 = Cam_SPI_read(hspi, 0x43);
    len3 = Cam_SPI_read(hspi, 0x44)& 0x7f;
    len = ((len3 << 16) | (len2 << 8) | len1) & 0x07fffff; // logic & operation needed, because content of the registry 0x44 isn't all length data.
    return len;
}

void Cam_Start_Capture(SPI_HandleTypeDef *hspi)
{
    // Reset memory pointers and capture status flags
    Cam_SPI_write(hspi, 0x04, 0x01); 
    HAL_Delay(3);
    Cam_SPI_write(hspi, 0x04, 0x01);

    // Start capture
    HAL_Delay(3);
    Cam_SPI_write(hspi, 0x04, 0x02); 
    HAL_Delay(3);
}

void Cam_Wait_Capture_Done(SPI_HandleTypeDef *hspi)
{
    // Check if bit meaning capture is done in registry 0x41 is set 
    while (1)
    {
        uint8_t regValue = Cam_SPI_read(hspi, 0x41);
        uint8_t captureDoneMask = 0x8;
        if (regValue & captureDoneMask)
            break;
    }
}

void Cam_Start_Burst_Read(SPI_HandleTypeDef *hspi)
{
    // Reset memory pointers and capture status flags
    Cam_SPI_write(hspi, 0x04, 0x01);
    Cam_SPI_write(hspi, 0x04, 0x01);

    uint8_t BURST_FIFO_READ = 0x3c;
    uint8_t empty = 0x00;

    // Set camera CS chip to high for SPI communication
    CS_On();

    // Send command for burst read
    HAL_SPI_TransmitReceive(hspi, &BURST_FIFO_READ, &empty, 1, HAL_MAX_DELAY);
}

void Cam_Capture(SPI_HandleTypeDef *hspi)
{
    Cam_SPI_write(hspi, 0x01, 0x00); // Capture Control Register - Set to capture n+1 frames

    Cam_Start_Capture(hspi);

    Cam_Wait_Capture_Done(hspi);
}
