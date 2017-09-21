#include <Wire.h>

unsigned long time;
byte* TS = (byte*) &time; 

void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin (9600);
}

void loop()
{
 for (int size=0; size < 10; size++) {
    time = micros();
  
    Wire.beginTransmission(4); // transmit to device #4
    for (int i=0; i <4; i++) {
      Wire.write (TS[i]); 
    }
    for (int i = 0; i <size; i++) Wire.write("x");
    Wire.endTransmission();    // stop transmitting
   
    Wire.requestFrom(4, 4);    // request 4 bytes from slave device #
    for (int i = 0; Wire.available() && (i<4); i++) { 
      byte c = Wire.read(); // receive a byte as character
      TS[i] = c;
    }
      
    Serial.print(size+4); Serial.print ("  ");
    Serial.println(micros()-time); 
    delay(400);
  }
}
