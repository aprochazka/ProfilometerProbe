# Profilometer Probe
## Firmware for profilometer probe

### Build

- Build using Makefile inside Firmware folder
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

#### Probe design block Scheme
![alt text](https://github.com/aprochazka/ProfilometerProbe/blob/main/deviceScheme.png?raw=true)

### Current State

- Sends test JPEG image sequence through USB CDC

### Test Current State

- Tested on: Arch Linux

- Inside Receiver folder build using Makefile  

- Code counts with CDC device on /dev/ttyACM2 (can be changed in code)

- Run with sudo permission, so that serial port is openable

### Test Script Usage - outdated

- Build test app using provided Makefile and then run the executable

- App will then start saving files received by uart to filesystem

### In progress


