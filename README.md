# SC16IS752_ESP8266

SC16IS752_ESP8266 is a C++ library for communicating with an SC16IS752 GPIO/UART expansion board using the ESP8266 using the brzo_i2c library found [here](https://github.com/pasko-zh/brzo_i2c).

This only supports ESP8266 modules because of the brzo_i2c which is implemented in Xtensa assembly language specifically for these boards.

# Getting started

## Prerequisites
SC16IS752_ESP8266 requires the brzo_i2c library for i2c communication using the ESP8266 board. It can be found [here](https://github.com/pasko-zh/brzo_i2c) with full installation instructions.

## Installation
Installation of this library can be done by first pulling from github, and then placing in the lib folder of your project. Then add #include "SC16IS752_ESP8266.h" near the start of your main project file to include its definitions and functions for your use.

# Usage
Below is an example project to get an ESP8266 to communicate with an SC16IS752 expansion board using i2c with appropriate pull ups and pull downs in order to turn on and off an LED connected to GPIO0 every 2 seconds.
```c++
#include  <Arduino.h>
#include  <SC16IS752_ESP8266.h>

uint8_t GPIO_status;

uint8_t SC16IS752_ADDRESS =  0x48;

void  setup() {
  Serial.begin(9600);

  //Setup the SC16IS752 device with divisor 12 for 9600 baud rate
  SC16IS752_setup(SC16IS752_ADDRESS, 12);

  //Write to GPIO0 to turn on LED
  SC16IS752_readReg(SC16IS752_ADDRESS, SC16IS752_IOState, &GPIO_status, SC16IS752_CHANNEL_A);
  SC16IS752_writeReg(SC16IS752_ADDRESS, SC16IS752_IOState, (GPIO_status |  0x01), SC16IS752_CHANNEL_A);

  delay(2000);
}

void  loop() {
  //Read current LED status
  SC16IS752_readReg(SC16IS752_ADDRESS, SC16IS752_IOState, &GPIO_status, SC16IS752_CHANNEL_A);
	
  //Toggle the LED at GPIO0 depending on current state
  if((GPIO_status &  0x01) ==  0x01) {
    SC16IS752_writeReg(SC16IS752_ADDRESS, SC16IS752_IOState, (GPIO_status &  0xFE), SC16IS752_CHANNEL_A);
  }
  else {
    SC16IS752_writeReg(SC16IS752_ADDRESS, SC16IS752_IOState, (GPIO_status |  0x01), SC16IS752_CHANNEL_A);
  }

  delay(2000);
}
```
The SC16IS752_ESP8266.cpp file in src contains descriptions for each of the functions which includes the inputs and outputs of each function.

## Authors
* [Liam Hartill](https://github.com/liamaha) - Initial work

## Acknowledgements
* [brzo_i2c](https://github.com/pasko-zh/brzo_i2c) for i2c communication library
* piersfinlayson's [SC16IS752 sample app](https://github.com/piersfinlayson/sc16is752-esp) for inspiration on setup function
* KISSLoRa [JosenePM](https://github.com/YourproductSmarter/KISSLoRa-demo/tree/master/Examples/JosenePM) example program for inspiration on register read/write functions

## Built with
* [Visual Studio Code](https://code.visualstudio.com/) for source code editing
* [PlatformIO IDE](https://platformio.org/platformio-ide) for Arduino programming
* [KiCad](http://kicad-pcb.org/) for planning schematic for example
* [StackEdit](https://stackedit.io/) for writing the readme
