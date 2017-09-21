/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
char C = 'W';



// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  Serial.begin (9600);  
}

// the loop routine runs over and over again forever:
void loop() {

  for(int i = 0, mask = 128; i<8; i++)
  {
    if(C & (mask>>i))
    {
      digitalWrite(led, HIGH);
      delay(100);               // wait for a second
      Serial.print("1");
    }
    else
    {
      digitalWrite(led, LOW);
      delay(100);               // wait for a second
      Serial.print("0");
    }
  }
  Serial.print("\n");
}
