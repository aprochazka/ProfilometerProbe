# Profilometer Probe
## Firmware for profilometer probe

### Build

- Build using Makefile inside root folder
- Flash onto hardware using flash option for make command `$ make flash`

### Supported HW:
- Nucleo L432KC 
- Arducam 5MP Plus


### HW Setup

Before running code, connect camera module to microcontroler through pins described in .ioc file (open with STM32CubeMX) or in table below.

| Name        | Nucleo L432KC | Arducam 5MP Plus |
| ----------- | ------------- | ---------------- |
| SPI SCLK    | A4            | SCK              |
| SPI MISO    | A5            | MISO             |
| SPI MOSI    | A6            | MOSI             |
| SPI CS      | D3            | CS               |
| I2C SCL     | D1            | SCL              |
| I2C SDA     | D0            | SDA              |

You can connect optional Debug LED to A1 pin in Nucleo board.

### Usage

- After turning on device, it initializes camera module and starts to capture JPEG images and continuously sends them through UART. 

- Currently only supports JPEG images in resolution 320*240

- UART Baud Rate is set to 115200 Bits/s

### In progress

- Implementing tinyUSB UVC to send JPEGS instead of UART
