/**
 ******************************************************************************
 * @file           : Cam.h
 * @brief          : Methods for interacting with camera module
 *                   This file contains the definitions the methods for interacting with camera module.
 * @author         : Adam Prochazka <xproch0f>
 ******************************************************************************
 */

#include "OV5642_regs.h"
#include "stm32l4xx_hal.h"

/**
 * @brief Disables the chip select line for the camera module.
 */
void CS_Off();

/**
 * @brief Enables the chip select line for the camera module.
 */
void CS_On();


/**
 * @brief Turns on the LED on Nucleo board.
 */
void LED_On();

/**
 * @brief Turns on the LED on Nucleo board.
 */
void LED_Off();

/**
 * @brief Turns on the optional debug LED.
 */
void Debug_LED_On();

/**
 * @brief Turns off the optional debug LED.
 */
void Debug_LED_Off();

/**
 * @brief Initializes SPI communication with the camera module.
 * @param hspi Pointer to an SPI_HandleTypeDef structure that contains the configuration information for the specified SPI.
 */
void SPI_Init(SPI_HandleTypeDef *hspi);

/**
 * @brief Reads data from the camera module using SPI.
 * @param hspi Pointer to an SPI_HandleTypeDef structure that contains the configuration information for the specified SPI.
 * @param address The address to read data from.
 * @return The data read from the specified address.
 */
int Cam_SPI_read(SPI_HandleTypeDef *hspi, uint8_t address);

/**
 * @brief Writes data to the camera module using SPI.
 * @param hspi Pointer to an SPI_HandleTypeDef structure that contains the configuration information for the specified SPI.
 * @param addr The address to write data to.
 * @param data The data to write to the specified address.
 * @return Status of the write operation (0 for success, non-zero for failure).
 */
int Cam_SPI_write(SPI_HandleTypeDef *hspi, uint8_t addr, uint8_t data);


/**
 * @brief Writes data to the camera module using I2C.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure that contains the configuration information for the specified I2C.
 * @param address The address to write data to.
 * @param data The data to write to the specified address.
 * @return Status of the write operation (1 for success, 0 for failure).
 */
int Cam_I2C_write(I2C_HandleTypeDef *hi2c, uint16_t address, uint8_t data);

/**
 * @brief Writes a sensor register structure to the camera module using I2C.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure that contains the configuration information for the specified I2C.
 * @param reg The sensor register structure to write.
 * @return Status of the write operation (1 for success, 0 for failure).
 */
int Cam_I2C_write_struct(I2C_HandleTypeDef *hi2c, sensor_reg reg);

/**
 * @brief Writes a list of sensor registers to the camera module using I2C.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure that contains the configuration information for the specified I2C.
 * @param regList An array of sensor register structures to write.
 */
void Cam_I2C_write_bulk(I2C_HandleTypeDef *hi2c, const struct sensor_reg regList[]);

/**
 * @brief Initializes the camera module.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure that contains the configuration information for the specified I2C.
 * @param hspi Pointer to an SPI_HandleTypeDef structure that contains the configuration information for the specified SPI.
 */
void Cam_Init(I2C_HandleTypeDef *hi2c, SPI_HandleTypeDef *hspi);

/**
 * @brief Retrieves the length of the FIFO buffer in the camera module.
 * @param hspi Pointer to an SPI_HandleTypeDef structure that contains the configuration information for the specified SPI.
 * @return The length of the FIFO buffer.
 */
int Cam_FIFO_length(SPI_HandleTypeDef *hspi);

/**
 * @brief Captures an image using the camera module.
 * @param hspi Pointer to an SPI_HandleTypeDef structure that contains the configuration information for the specified SPI.
 */
void Cam_Capture(SPI_HandleTypeDef *hspi);

/**
 * @brief Starts the capture process in the camera module.
 * @param hspi Pointer to an SPI_HandleTypeDef structure that contains the configuration information for the specified SPI
*/
void Cam_Start_Capture(SPI_HandleTypeDef *hspi);

/**
 * @brief Waits for the capture process to complete in the camera module.
 * @param hspi Pointer to an SPI_HandleTypeDef structure that contains the configuration information for the specified SPI.
*/
void Cam_Wait_Capture_Done(SPI_HandleTypeDef *hspi);

/**
* @brief Starts the burst read process from the camera module's FIFO buffer.
* @param hspi Pointer to an SPI_HandleTypeDef structure that contains the configuration information for the specified SPI.
*/
void Cam_Start_Burst_Read(SPI_HandleTypeDef *hspi);
