//Sample app for the SC16IS752_ESP8266 library
//Turns on LED at GPIO0 then delays 2 seconds
//Toggles the LED and reads data from the PMS3003 dust sensor every 4 seconds
//Wiring guide PDF found in above folder

#include <Arduino.h>
#include <SC16IS752_ESP8266.h>

uint8_t GPIO_status;

uint8_t SC16IS752_ADDRESS = 0x48;

void setup() {
  //Run once
  Serial.begin(9600);
  Serial.print("\nSerial has begun \n");

  SC16IS752_setup(SC16IS752_ADDRESS, 12);
  Serial.print("SC16IS752 has been setup \n");

  //Write to GPIO0 to turn on LED
  SC16IS752_readReg(SC16IS752_ADDRESS, SC16IS752_IOState, &GPIO_status, SC16IS752_CHANNEL_A);           //read current state of LED
  SC16IS752_writeReg(SC16IS752_ADDRESS, SC16IS752_IOState, (GPIO_status | 0x01), SC16IS752_CHANNEL_A);  //write to LED's the current GPIO status, with GPIO0 turned on in addition
  delay(2000);
}

void loop() {
  //Run repeatedly after setup

  //Toggle LED
  SC16IS752_readReg(SC16IS752_ADDRESS, SC16IS752_IOState, &GPIO_status, SC16IS752_CHANNEL_A);             //read current state of LED
  if((GPIO_status & 0x01) == 0x01) {   
    SC16IS752_writeReg(SC16IS752_ADDRESS, SC16IS752_IOState, (GPIO_status & 0xFE), SC16IS752_CHANNEL_A);  //write to LED's the current GPIO status, with GPIO0 turned off in addition
  }
  else {
    SC16IS752_writeReg(SC16IS752_ADDRESS, SC16IS752_IOState, (GPIO_status | 0x01), SC16IS752_CHANNEL_A);  //write to LED's the current GPIO status, with GPIO0 turned on in addition
  }

  //Read Dust
  uint8_t ReadDust[64] = {}, Dust[24] = {};                             //Buffers for raw register read, and refined data once start characters found
  int index = 0, checksum = 0;
  Serial.print("Begin cycle\n");
  SC16IS752_readUart(SC16IS752_ADDRESS, ReadDust, SC16IS752_CHANNEL_A); //Read data from dustport TX
  Serial.print("Dust port read starting at ");
  for(unsigned int i = 0; i < sizeof(ReadDust); i++) {
    if(ReadDust[i] == 66) {                                             //Find start character (B, should be followed by 77(M))
      index = i;
      break;
    }
  }
  Serial.print(index);
  Serial.print("\n");
  for(unsigned int i = 0; i < sizeof(ReadDust); i++) {                //Print off the raw data from sensor buffer for reference
    Serial.print(ReadDust[i]);
    Serial.print(" ");
  }
  Serial.print("\n\n");
  for(int i = 0; i < 24; i++) {                                       //Construct the 24 byte long actual data line
    Dust[i] = ReadDust[index + i];
  }
  for(unsigned int i = 0; i < sizeof(Dust); i++) {                    //Print off data line
    Serial.print(Dust[i]);
    Serial.print(" ");
    checksum += Dust[i];                                              //Tally up total value of entire data line summed
  }
  Serial.print('\n');
  if((Dust[22] * 256 + Dust[23] + Dust[22] + Dust[23]) == checksum) { //Compare sum with sensor-calculated expected value (refer to PMS3003 datasheet) to check validity of data
    Serial.print("Valid data");
  }
  else {
    Serial.print("Invalid data");
  }
  Serial.print("\n\n");

  delay(4000);
}