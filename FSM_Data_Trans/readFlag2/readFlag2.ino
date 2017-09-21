/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogReadSerial
*/

enum FSM {sendLOW, sendHIGH, clockHIGH, dataHIGH} state;
int res = 0;
int d_bit = 0;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  state = sendLOW;
}


// the loop routine runs over and over again forever:
void loop() {
  delay(10);
  // read the input on analog pin 0:
  bool data = analogRead(A0);
  bool flag = analogRead(A2);
  bool clock = analogRead(A1);
  
  switch (state) {
    case sendLOW:
      if(flag==1) {
        d_bit = 0;
        state = sendHIGH;
      }
      break;
    case sendHIGH:
      if(flag==0){
        //print result + reset
        Serial.println(res, HEX);
        res = 0;
        d_bit = 0;
        state = sendLOW;
        break;
      }
      if(clock==1){
        d_bit = 0;
        state = clockHIGH;
      }
      break;
    case clockHIGH:
      if(flag==0){
        //print result + reset
        res = 0;
        d_bit = 0;
        state = sendLOW;
        break;
      }
      if(clock==0){
        //add bit to result
        res = res*2 + d_bit;
        state = sendHIGH;
        break;
      }
      if(data==1){
        d_bit = 1;
        state = dataHIGH;
        break;
      }
      break;
    case dataHIGH:
      if(flag==0){
        //print result + reset
        res = 0;
        d_bit = 0;
        state = sendLOW;
        break;
      }
      if(clock==0){
        //add bit to result
        res = res*2 + d_bit;
        state = sendHIGH;
        break;
      }
      if(data==0){
        state = clockHIGH;
        break;
      }
      break;
  }
  
}
