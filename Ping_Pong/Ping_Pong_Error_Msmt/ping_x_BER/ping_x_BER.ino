#include <Wire.h>


void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin (9600);
  Wire.setBER(30);
}

void loop()
{
    Wire.beginTransmission(4); // transmit to device #4
    for (int i = 0; i <16; i++) Wire.write("x");
    Wire.endTransmission();    // stop transmitting
   
    delay(500);
}
