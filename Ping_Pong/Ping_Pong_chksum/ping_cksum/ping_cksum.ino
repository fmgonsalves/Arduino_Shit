#include <Wire.h>

#define DATALEN 16

struct DataPDU{
  byte data[DATALEN];
  uint16_t cksum;
};

struct DataPDU frame;
byte* ptr = (byte*) &frame;

/* from http://locklessinc.com/articles/tcp_checksum/ */

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
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin (9600);
  
  for (int i=0; i< DATALEN; i++) frame.data[i]='x';
  frame.cksum = checksum(frame.data, DATALEN); 
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
