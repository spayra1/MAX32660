//  I2C Scanner from Arduino.cc
//  Attribution to Krodal, Nick Gammon, Anonymous

 
#include <Wire.h>
 
void setup()
{
  Wire.begin();
  
  Serial.begin(9600); //can be change to 115200
  Wire.setClock(4000);
  Serial.println("\nI2C Scanner");
}
 
void loop()
{
  byte error, address;
  int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address <= 127; address++)
  {
    // The i2c_scanner uses the return value of
    // Wire.endTransmission to see if
    // a device acknowledged the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
 
  delay(1000);           // wait 5 seconds for next scan
}

        
