#include <Wire.h>

#define DATALEN 16
#define TRACE 1

struct DataPDU{
  byte data[DATALEN];
  uint16_t cksum;
};

struct DataPDU frame;
byte* ptr = (byte*) &frame;

#ifdef TRACE
void Binary (uint32_t b, int l) {
   for (int i=0; i<l; i++) {
      if (b & ((uint32_t)0x01<<(l-i-1))) Serial.print("1"); 
      else Serial.print("0");
   } 
}

void Space (int n) {
  for (int i=0; i < n; i++) Serial.print(" ");
}
#endif

uint32_t CRC(const byte *buf, unsigned size)
{
	uint32_t R = (uint32_t) 0x0;
        uint32_t G = 0x0000011021; // X16 + X12 + X5 + 1
//          uint32_t G = 0x0000000107; // X7 + X2 + X + 1
//        uint32_t G = 0x0000000013; // X4 + X + 1
        
        uint8_t rank = 31; // start from the left

#ifdef TRACE
        for (int i=0; i< size; i++) Binary (buf[i], 8);
        Serial.println();
#endif        
        // compute G rank     
        while (rank && !(((uint32_t) 0x01 << rank) & G)) rank--;
        
        for (int i=0; i < 8*size+rank; i++) {
           R = R << 1 ; 
            
           if (i < 8*size) { //after 8*size add 0
              if  (buf[i/8] & (0x80 >> (i%8))) R |= 0x01;
           }                 
           
                       
           if (R & ((uint32_t) 0x01 << rank)) { // R left bit == 1
#ifdef TRACE
                   Space (i-rank); Binary (R, rank+1); Serial.println();
                   Space (i-rank); Binary (G, rank+1); Serial.println();       
#endif
                   R ^= G;       
           }
       }
#ifdef TRACE       
       Space (8*size); Binary (R, rank); Serial.println("*");
#endif

       return R;
}

void setup()
{
  uint16_t c; 
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin (9600);
  
  for (int i=0; i< DATALEN; i++) frame.data[i]='x';
  frame.cksum = CRC(frame.data, DATALEN); 
  
  Serial.println (frame.cksum, HEX);
}

void loop()
{
  long counter[5];
  
    for (int err=10; err< 10000; err+=10) {
      Wire.setBER(err); 
      
      for (int k=1; k < 5; k++) counter[k]=0;   
        for (int j=0; j < 1000; j++) {
          Wire.beginTransmission(4); 
          Wire.write(ptr, sizeof(struct DataPDU)); 
          Wire.endTransmission();    
        
          delay (5); 
        
          Wire.requestFrom(4, 1);
          while(Wire.available()) {
            byte c = Wire.read();  
            if ((c > 0) and (c <5)) counter[c]++; 
          }
        }
        
      Serial.print (err); Serial.print (" ");
      for (int k=1; k < 5; k++) {
         Serial.print(counter[k]); Serial.print(" "); 
      }
      Serial.println();
    }
}
