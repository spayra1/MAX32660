#include <Wire.h>    
 
#define disk1 0x50    //Address of 24LC256 eeprom chip
 
void setup(void)
{
  Serial.begin(9600);
  Wire.begin();  
 
  unsigned int address = 0;
 
  //writeEEPROM(disk1, address, 123);
  Serial.print(readEEPROM(disk1, address), DEC);
  Serial.println();
  Serial.print(readEEPROM(disk1, 1), DEC);
  Serial.println();
  Serial.print(readEEPROM(disk1, 2), DEC);
  Serial.println();
  Serial.print(readEEPROM(disk1, 3), DEC);
  Serial.println();
  Serial.print(readEEPROM(disk1, 4), DEC);
  Serial.println();
  Serial.print(readEEPROM(disk1, 5), DEC);
  Serial.println();
  Serial.print(readEEPROM(disk1, 6), DEC);
  Serial.println();
  Serial.print(readEEPROM(disk1, 7), DEC);
  Serial.println();
  Serial.print(readEEPROM(disk1, 8), DEC);
}
 
void loop(){}
 
/*void writeEEPROM(int deviceaddress, unsigned int eeaddress, byte data ) 
{
  Wire.beginTransmission(deviceaddress);
  Wire.send((int)(eeaddress >> 8));   // MSB
  Wire.send((int)(eeaddress & 0xFF)); // LSB
  Wire.send(data);
  Wire.endTransmission();
 
  delay(5);
}
 */
byte readEEPROM(int deviceaddress, unsigned int eeaddress ) 
{
  byte rdata = 0xFF;
 
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
 
  Wire.requestFrom(deviceaddress,1);
 
  if (Wire.available()) rdata = Wire.read();
 
  return rdata;
}
