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

unsigned short checksum(const byte *buf, unsigned size)
{
	unsigned sum = 0;
	int i;

	/* Accumulate checksum */
	for (i = 0; i < size - 1; i += 2)
	{
		unsigned short word16 = *(unsigned short *) &buf[i];
		sum += word16;
	}

	/* Handle odd-sized case */
	if (size & 1)
	{
		unsigned short word16 = (unsigned char) buf[i];
		sum += word16;
	}

	/* Fold to get the ones-complement result */
	while (sum >> 16) sum = (sum & 0xFFFF)+(sum >> 16);

	/* Invert to get the negative in ones-complement arithmetic */
	return ~sum;
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
  Serial.print(nbFrame);  Serial.print (" TVP ");
  Serial.print(tvp); Serial.print (" TVN"); Serial.print(tfp); Serial.print (" TFP"); 
  Serial.print(tvn); Serial.print (" TFN"); Serial.println(tfn); 
  
  Serial.println(ack);
  delay(200);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receivePDU(int howMany)
{
  int  idx =0;
  bool ok = true;
  unsigned short cksumRecv;
    
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
  
  cksumRecv = checksum((byte*)&frame.data, 16);

  if (ok && (cksumRecv == frame.cksum)) {
    tvp++;
    ack = VP;
  } else  if (!ok && (cksumRecv != frame.cksum)) {
    tfp++;
    ack = FP;    
  } else  if (!ok && (cksumRecv == frame.cksum)) {
    tvn++;
    ack = VN;    
  }else  if (ok && (cksumRecv != frame.cksum)) {
    tfn++;
    ack = FN;    
  }

}

void sendPDU()
{
    Wire.write(ack);
}
