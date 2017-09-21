#include <Wire.h>

char Rcv[20];
volatile unsigned long noerror = 0;
volatile unsigned long total = 0; 
volatile unsigned long frame = 0; 

void setup()
{
  
  Wire.begin(4);             // join i2c bus with address #4
  Wire.onReceive(receiveTS);
  Serial.begin(9600);
   // register event

}

void loop()
{ 
  for (int i = 0; i < 16; i++) Serial.print (Rcv[i]);

    Serial.print (" No of correct frames is "); Serial.println(noerror);
    Serial.print (" Ratio of correct to total frames is "); Serial.println((float)noerror/frame);
    Serial.print (" Bits/Erroneous Bits is "); Serial.println((float)8*16*frame/total);
    Serial.println (" ");
  delay(100);
}

void receiveTS(int howMany)
{
int  idx =0;
bool ok = true;
    
  frame++; 
  while(Wire.available()) 
  {
    char c = Wire.read(); 
    Rcv[idx++] = c;

    for (int i = 0; i < 8; i++) {
      int msk = 1 << i;
      
      if (( c & msk) != ('x' & msk)) {
         total++;
         ok = false;
      }
    }    
  }
  if (ok) noerror++;
}


