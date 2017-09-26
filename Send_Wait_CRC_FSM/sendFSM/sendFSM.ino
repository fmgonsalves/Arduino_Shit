
#include <Wire.h>
enum FSM {state0, state1, waitACK0, waitACK1} state;
#define DATALEN 30
#define TRACE 1

long t_start;
long t_end;

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

void sendPDU (byte* ptr, int sizePDU)
{
  Wire.beginTransmission(4); 
  Wire.write(ptr, sizePDU); 
  Wire.endTransmission();  
}
// the setup routine runs once when you press reset:
void setup() {
  Wire.begin();
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  state = state0;
}


// the loop routine runs over and over again forever:
void loop() {
//  delay(10);
//  // read the input on analog pin 0:
//  bool data = analogRead(A0);
//  bool flag = analogRead(A2);
//  bool clock = analogRead(A1);
  
  switch (state) 
  {
    case state0:
      if(Serial.available())
      {
        
        int i = 0; 
        frame.frame_info = B00000000; //DataPDU0 + reserved bits
        while(i < 30 && Serial.available() > 0) 
        {
          frame.data[i] = Serial.read();
          delay(20);
          i++;
        }

        //include frame_info in checksum
        byte full_frame[DATALEN + 1];
        full_frame[0] = 0;
        for(int i=0; i < DATALEN + 1; i++)
          full_frame[i+1] = frame.data[i];
        frame.cksum = CRC(full_frame, DATALEN + 1); 
//        for(int p = 0; p < DATALEN+1; p++){ Serial.print(full_frame[p]); Serial.print(" ");}
//        Serial.println(" ");
//        Serial.println(frame.cksum);
        sendPDU(ptr, sizeof(frame));
        //START TIMER
        t_end = millis() + 1000; //setting a max duration of 1000ms for ACK to be received
        
        
        state = waitACK0;
      }
      
      break;
      
    case waitACK0:
      if (millis() > t_end)
      {
        sendPDU(ptr, sizeof(frame));
        
        //START TIMER
        t_end = millis() + 1000; //setting a max duration of 1000ms for ACK to be received
        
      }
      Wire.requestFrom(4,2);
      if(Wire.available())
      {
       int idx = 0;
       while(Wire.available() && (idx < sizeof(struct AckPDU)))
       {
          ptrACK[idx++] =  Wire.read();
       }
        
        if(ack.frame_info == B10000000 && ack.cksum == frame.cksum)
        {
          for(int j = 0; j < DATALEN; j++) frame.data[j] = 0;
          frame.cksum = 0;
          frame.frame_info = 0;
          state = state1;
        }
      }
      break;
      
    case state1:
      if(Serial.available())
      {
        int i = 0; 
        frame.frame_info = B01000000; //DataPDU0 + reserved bits
        while(Serial.available() > 0 && i < 30) 
        {
          frame.data[i] = Serial.read();
          delay(20);
          i++;
        }
        frame.cksum = CRC(frame.data, DATALEN); 
        
        //include frame_info in checksum
        byte full_frame[DATALEN + 1];
        full_frame[0] = 0;
        for(int i=0; i < DATALEN + 1; i++)
          full_frame[i+1] = frame.data[i];
        frame.cksum = CRC(full_frame, DATALEN + 1); 
        
       
        
        sendPDU(ptr, sizeof(frame));
        //START TIMER
        t_end = millis() + 1000; //setting a max duration of 1000ms for ACK to be received
        state = waitACK1;
      }
      
      break;
    case waitACK1:
      if (millis() > t_end)
      {
        sendPDU(ptr, sizeof(frame));
        
        //START TIMER
        t_end = millis() + 1000; //setting a max duration of 1000ms for ACK to be received
      }
      
      Wire.requestFrom(4,2);
      
      if(Wire.available())
      {
        int idx = 0;
        while(Wire.available() and (idx < sizeof(struct AckPDU))) 
          ptrACK[idx++] =  Wire.read();
        
        if(ack.frame_info == B11000000 && ack.cksum == frame.cksum)
        {
          for(int j = 0; j < DATALEN; j++) frame.data[j] = 0;
          frame.cksum = 0;
          frame.frame_info = 0;
          state = state0;
        }
      }
      break;
  }
  
}
