//#include <Wire.h>
//#include <LiquidCrystal.h>
//
//LiquidCrystal lcd(3, 2, 4);

volatile long v;
volatile int length;
byte* TS= (byte*) &v;

void setup()
{
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveTS); // register event
  Wire.onRequest(requestTS);
  Serial.begin(9600); 

}

void loop()
{ 
  Serial.print (length); Serial.print (" ") ;
  Serial.print (v); Serial.print ("        ") ;

  delay(200);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveTS(int howMany)
{
  length = howMany; // can be shown in the loop
  v = 0;
  for (int i=0; i < 4; i++) {  // read 32 bit byte per byte
    TS [i] = Wire.read();
  }

  while(Wire.available()) //skip the rest of the frame
  {
    char c = Wire.read(); 
  }
}

void requestTS()
{
    Wire.write(TS, 4);
}
