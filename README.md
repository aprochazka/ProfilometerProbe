# Profilometer Probe

![alt text](https://github.com/aprochazka/ProfilometerProbe/blob/main/Probe_Render.png?raw=true)
![alt text](https://github.com/aprochazka/ProfilometerProbe/blob/main/Probe_Assembled.png?raw=true)

## Hardware

### Models

- All 3D printable models are located inside Models folder
- Models are designed to be printed with standard 2mm layer height
- When slicing be sure to print outer walls first to make print more accurate

### Components

- Development board: Nucleo L432KC
- Camera: ArduCam mini 5MP Plus
- Distance sensor: Pimoroni LV53L1X
- Complete list of components and connectors for probe can be found inside HW/Parts.md

## Firmware for profilometer probe

#### Probe design block Scheme
![alt text](https://github.com/aprochazka/ProfilometerProbe/blob/main/deviceScheme.png?raw=true)

### Build

- Build using Makefile inside Firmware folder (dependency: arm-none-eabi)
- Flash onto hardware using flash option for make command `$ make flash`

### Usage

- Connect probe to pc using usb c cable and firmware will start sending sending data automatically

### Documentation

- Create documentation using 'doc' option of Makefile '$ make doc' 

## Software for receiving data

### Build

- Build using Makefile inside Receiver folder

### Usage

- Has to be run with sudo permissions to correctly access USB CDC port.
- '$ sudo ./main -p <name of the port> [-s Save received jpeg to files] [-r Print raw received CDC data to standard output]'
- Prefer running with optional -s command for ability to inspect already captured images 

### Documentation

- Create documentation using 'doc' option of Makefile '$ make doc' 

### TODO

- Process image and filter only laser line from it
- Use precessed data to determine depth or height of irregularities 


