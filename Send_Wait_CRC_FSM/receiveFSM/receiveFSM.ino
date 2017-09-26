/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogReadSerial
*/
#include <Wire.h>
enum FSM {waitPDU0, waitPDU1} state;
#define DATALEN 30
#define TRACE 1

long t_start;
long t_end;
int CRCflag = 0;

struct DataPDU{
  byte data[DATALEN];
  byte frame_info;
  uint8_t cksum;
};

struct AckPDU{
  byte frame_info;
  uint8_t cksum;
};

struct DataPDU frame;
byte* ptr = (byte*) &frame;

struct AckPDU ack;
byte* ptrACK = (byte*) &ack;


//int res = 0;
//int d_bit = 0;

//CRC Function
uint8_t CRC(const byte *buf, unsigned size)
{
	uint8_t R = (uint8_t) 0x0;
        //uint32_t G = 0x0000011021; // X16 + X12 + X5 + 1
//          uint32_t G = 0x0000000107; // X7 + X2 + X + 1
        uint8_t G = 0x0000000013; // X4 + X + 1
        
        uint8_t rank = 31; // start from the left

        // compute G rank     
        while (rank && !(((uint8_t) 0x01 << rank) & G)) rank--;
        
        for (int i=0; i < 8*size+rank; i++) {
           R = R << 1 ; 
            
           if (i < 8*size) { //after 8*size add 0
              if  (buf[i/8] & (0x80 >> (i%8))) R |= 0x01;
           }                 
           
                       
           if (R & ((uint8_t) 0x01 << rank)) { // R left bit == 1
                   R ^= G;       
           }
       }
       return R;
}

void receivePDU (int howMany) //receive PDU and check CRC
{
  int  idx = 0;
  while(Wire.available() and (idx < sizeof(struct DataPDU))) 
    ptr[idx++] =  Wire.read();
  
  
  byte full_frame[DATALEN + 1];
  full_frame[0] = 0;
  for(int i=0; i < DATALEN + 1; i++)
    full_frame[i+1] = frame.data[i];
  uint8_t CRCres = CRC(full_frame, DATALEN + 1); 
  
  if(CRCres == frame.cksum)
    CRCflag = 1;
    
}

void sendACK ()
{
  Wire.write(ptrACK, sizeof(struct AckPDU));
}
// the setup routine runs once when you press reset:
void setup() {
  Wire.begin(4);
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  Wire.onReceive(receivePDU);
  Wire.onRequest(sendACK);  
  state = waitPDU0;
}


// the loop routine runs over and over again forever:
void loop() {
//  delay(10);
//  // read the input on analog pin 0:
//  bool data = analogRead(A0);
//  bool flag = analogRead(A2);
//  bool clock = analogRead(A1);
  if(CRCflag == 1)
  {
    switch (state) 
    {
      case waitPDU0:
        if(frame.frame_info == B00000000){
          //TODO. FORWARD DATA
          for(int it = 0; it < DATALEN; it++) {
            Serial.print ((char) frame.data[it]);
            if((char) frame.data[it] == '\n') break;
          }
          ack.frame_info = B10000000;
          ack.cksum = frame.cksum;
       
         
          sendACK();
          state = waitPDU1;
        }
        if(frame.frame_info == B01000000){
          //TODO. FORWARD DATA
          ack.frame_info = B11000000;
          ack.cksum = frame.cksum;
          sendACK();
        }
        
      break;
      case waitPDU1:
        if(frame.frame_info == B01000000){
          //TODO. FORWARD DATA
          for(int it = 0; it < DATALEN; it++) {
            Serial.print ((char) frame.data[it]);
            if((char) frame.data[it] == '\n') break;
          }
          ack.frame_info = B11000000;
          ack.cksum = frame.cksum;
          sendACK();
          state = waitPDU0;
        }
        if(frame.frame_info == B00000000){
          //TODO. FORWARD DATA
          ack.frame_info = B10000000;
          ack.cksum = frame.cksum;
          sendACK();
        }
      break;
    }
  }
  
}
