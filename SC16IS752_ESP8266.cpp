#include "SC16IS752_ESP8266.h"

//Definitions for the ESP8266 SDA and SCL pins
uint8_t SDA_PIN = 4;
uint8_t SCL_PIN = 5;

//setup
//*******
//Inputs:
//Address - the address of your SC16IS752,
//BaudrateDivisor - the baud rate divisor you require for your specific baud rate requirement given your oscillating crystal
//*******
//Outputs:
//nothing returned
void ICACHE_FLASH_ATTR SC16IS752_setup(uint8_t Address, uint8_t BaudrateDivisor) {
    brzo_i2c_setup(SDA_PIN, SCL_PIN, 400);                              //begin i2c communication with the SC16IS752
    SC16IS752_writeReg(Address, SC16IS752_IER, 0b00000001, 0);          //enable receive RHR interrupt
    SC16IS752_writeReg(Address, SC16IS752_FCR, 0b00000001, 0);          //set rx, tx triggers to 8 chars
    SC16IS752_writeReg(Address, SC16IS752_LCR, 0b00000011, 0);          //no parity, 1 stop bit, 8 bit word length
    SC16IS752_writeReg(Address, SC16IS752_MCR, 0b00000000, 0);          //divide by 1 clock
    SC16IS752_writeReg(Address, SC16IS752_IODir, 0b00000011, 0);        //Set GPIOs 0/1 to output
    SC16IS752_writeReg(Address, SC16IS752_IOIntEna, 0b00000000, 0);     //No interrupts for GPIOs
    SC16IS752_writeReg(Address, SC16IS752_IOControl, 0b00000000, 0);    //GPIOs as GPIOs, unlatched
    SC16IS752_writeReg(Address, SC16IS752_EFCR, 0b00000000, 0);         //defaults

    SC16IS752_writeReg(Address, SC16IS752_LCR, 0b10000011, 0);          //enable special register set
    SC16IS752_writeReg(Address, SC16IS752_DLL, BaudrateDivisor, 0);     //set Baud rate divisor MSB (use 12 for BaudrateDivisor assuming 1.8432MHz crystal)
    SC16IS752_writeReg(Address, SC16IS752_DLH, 0, 0);                   //set Baud rate divisor LSB
    SC16IS752_writeReg(Address, SC16IS752_LCR, 0b00000011, 0);          //disable special register set
}

//Basic register read/write functions

//Register write function
//*******
//Inputs:
//Address - the address of your SC16IS752,
//regAddr - the register you wish to write to,
//val - the data you wish to write to the register,
//Channel - address of Channel (use SC16IS752_CHANNEL_A or SC16IS752_CHANNEL_B defined in .h file)
//*******
//Outputs:
//nothing returned,
//Prints an error message if the I2C transaction fails (refer to brzo_i2c for error meaning),
//Writes your value to the selected register
void ICACHE_FLASH_ATTR SC16IS752_writeReg(uint8_t Address, uint8_t regAddr, uint8_t val, uint8_t Channel) {
    uint8_t Buffer[2];
    int result;
    Buffer[0] = (regAddr << 3 | Channel << 1);
    Buffer[1] = val;
    brzo_i2c_start_transaction(Address, 100);
    brzo_i2c_write(Buffer, 2, false);
    result = brzo_i2c_end_transaction();
    if(result != 0) {
        Serial.print("\n");
        Serial.print("ERROR");
        Serial.print("\n");
        Serial.print(result);
        Serial.print("\n");
    }
}

//Register read function
//*******
//Inputs:
//Address - the address of your SC16IS752,
//regAddr - the register you wish to read from,
//*val - A pointer to the buffer you wish to store the read data to,
//Channel - address of Channel (use SC16IS752_CHANNEL_A or SC16IS752_CHANNEL_B defined in .h file)
//*******
//Outputs:
//nothing returned,
//Prints an error message if the I2C transaction fails (refer to brzo_i2c for error meaning),
//Writes the register's data to your selected buffer
void ICACHE_FLASH_ATTR SC16IS752_readReg(uint8_t Address, uint8_t regAddr, uint8_t *val, uint8_t Channel) {
    uint8_t Buffer[1];
    int result;
    Buffer[0] = (regAddr << 3 | Channel << 1);
    brzo_i2c_start_transaction(Address, 100);
    brzo_i2c_write(Buffer, 1, false);
    brzo_i2c_read(val, 1, false);
    result = brzo_i2c_end_transaction();
    if(result != 0) {
        Serial.print("\n");
        Serial.print("ERROR");
        Serial.print("\n");
        Serial.print(result);
        Serial.print("\n");
    }
}

//Function to check number of bytes in the RX terminal of the UART (similar to arduino .available)
//*******
//Inputs:
//Address - the address of your SC16IS752,
//Channel - address of Uart Channel (use SC16IS752_CHANNEL_A or SC16IS752_CHANNEL_B defined in .h file)
//*******
//Outputs:
//RxLvl - the number of bytes found in the RX terminal of your selected UART channel
uint8_t ICACHE_FLASH_ATTR SC16IS752_checkRxLvl(uint8_t Address, uint8_t Channel) {
    uint8_t RxLvl;
    SC16IS752_readReg(Address, SC16IS752_RXLVL, &RxLvl, Channel);
    return RxLvl;
}

//Uart read/write functions

//Uart write function
//*******
//Inputs:
//Address - the address of your SC16IS752,
//val - the byte you wish to write to the Uart,
//Channel - address of Uart Channel (use SC16IS752_CHANNEL_A or SC16IS752_CHANNEL_B defined in .h file)
//*******
//Outputs:
//nothing returned,
//Prints an error message if the I2C transaction fails (refer to brzo_i2c for error meaning)
void ICACHE_FLASH_ATTR SC16IS752_writeUart(uint8_t Address, uint8_t val, uint8_t Channel) {
    uint8_t Buffer[2];
    int result;
    Buffer[0] = (SC16IS752_THR << 3 | Channel << 1);
    Buffer[1] = val;
    brzo_i2c_start_transaction(Address, 100);
    brzo_i2c_write(Buffer, 2, false);
    result = brzo_i2c_end_transaction();
    if(result != 0) {
        Serial.print("\n");
        Serial.print("ERROR");
        Serial.print("\n");
        Serial.print(result);
        Serial.print("\n");
    }
}

//Uart read function
//*******
//Inputs:
//Address - the address of your SC16IS752,
//*val - A pointer to the buffer you wish to store the read data to,
//Channel - address of Channel (use SC16IS752_CHANNEL_A or SC16IS752_CHANNEL_B defined in .h file)
//*******
//Outputs:
//nothing returned,
//Prints an error message if the I2C transaction fails (refer to brzo_i2c for error meaning),
//Writes the UART's data to your selected buffer
void ICACHE_FLASH_ATTR SC16IS752_readUart(uint8_t Address, uint8_t *val, uint8_t Channel) {
    uint8_t Buffer[1];
    int result;
    Buffer[0] = (SC16IS752_RHR << 3 | Channel << 1);
    int length = SC16IS752_checkRxLvl(Address, Channel);
    brzo_i2c_start_transaction(Address, 100);
    brzo_i2c_write(Buffer, 1, false);
    brzo_i2c_read(val, length, false);
    result = brzo_i2c_end_transaction();
    if(result != 0) {
        Serial.print("\n");
        Serial.print("ERROR");
        Serial.print("\n");
        Serial.print(result);
        Serial.print("\n");
    }
}