#include <Wire.h>


volatile uint16_t valid;
byte* check= (byte*) &valid;
char Rcv[20];

#define DATALEN 16

#define VP 1 /* Frame OK Cks OK */
#define FP 2 /* Frame NO Cks NO */
#define VN 3 /* Frame NO Cks OK */
#define FN 4 /* Frame OK Cks NO */

struct DataPDU{
  byte data[DATALEN];
  uint16_t cksum;
};

struct DataPDU frame;
byte* ptr = (byte*) &frame;
byte stat;

volatile unsigned long nbFrame = 0; 
volatile unsigned long tvp = 0;
volatile unsigned long tfp = 0;
volatile unsigned long tvn = 0;
volatile unsigned long tfn = 0;
volatile byte ack = 0;
volatile unsigned short CRCres;


uint32_t CRC(const byte *buf, unsigned size)
{
	uint32_t R = (uint32_t) 0x0;
        uint32_t G = 0x0000011021; // X16 + X12 + X5 + 1
//          uint32_t G = 0x0000000107; // X7 + X2 + X + 1
//        uint32_t G = 0x0000000013; // X4 + X + 1
        
        uint8_t rank = 31; // start from the left

        // compute G rank     
        while (rank && !(((uint32_t) 0x01 << rank) & G)) rank--;
        
        for (int i=0; i < 8*size+rank; i++) {
           R = R << 1 ; 
            
           if (i < 8*size) { //after 8*size add 0
              if  (buf[i/8] & (0x80 >> (i%8))) R |= 0x01;
           }                 
           
                       
           if (R & ((uint32_t) 0x01 << rank)) { // R left bit == 1
                   R ^= G;       
           }
       }
       return R;
}


void setup()
{
  Serial.begin(9600);
    
  Wire.begin(4);                
  Wire.onReceive(receivePDU);   
  Wire.onRequest(sendPDU); 
  
}

void loop()
{   
  Serial.print(nbFrame);  Serial.print (" TVP");
  Serial.print(tvp); Serial.print (" TVN"); Serial.print(tfp); Serial.print (" TFP"); 
  Serial.print(tvn); Serial.print (" TFN"); Serial.println(tfn); 
  
  Serial.println(ack);
  Serial.print(CRCres);Serial.println ("   ");
  delay(200);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receivePDU(int howMany)
{
  int  idx =0;
  bool ok = true;
    
  nbFrame++; 
  while(Wire.available() and (idx < sizeof(struct DataPDU))) 
    ptr[idx++] =  Wire.read();

  for (idx = 0; idx < DATALEN; idx++){
    char c = frame.data[idx];
    
    for (int i = 0; i < 8; i++) {
      int msk = 1 << i;
      
      if (( c & msk) != ('x' & msk)) {
         ok = false;
      }
    }    
  }
  
  CRCres = CRC((byte*)&frame.data, 16);

  if (ok && (CRCres==frame.cksum)) {
    tvp++;
    ack = VP;
  } else  if (!ok && (CRCres!=frame.cksum)) {
    tfp++;
    ack = FP;    
  } else  if (!ok && (CRCres==frame.cksum)) {
    tvn++;
    ack = VN;    
  }else  if (ok && (CRCres!=frame.cksum)) {
    tfn++;
    ack = FN;    
  }

}

void sendPDU()
{
    Wire.write(ack);
}
