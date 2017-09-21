/*
  Program to send characters bit by bit to another arduino
*/
int DATA = 13;
int FLAG = 11;
int CLOCK = 12;
//bool lflag = 0;

void sendByte(char B);
void sendBit(int D);

char C = 'A';

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);


  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
  
  sendByte(C);
  
}

//Function for flag; signals start of message
void sendByte(char B)
{
  digitalWrite (FLAG, HIGH);
  delay(50);
  //lflag = 1;
  int mask = 0x80;
  for (int i=0; i<8; i++)
  {
    sendBit(B & (mask>>i));
  }
  Serial.print("\n");
  delay(50);
  digitalWrite (FLAG, LOW);
  delay(250);
}

//Function for sending data; sets clock high for each bit transfer
void sendBit(int D)
{
  digitalWrite (CLOCK, HIGH);
  if(D)
  {
    digitalWrite (DATA, HIGH);
    Serial.print ("1");
    delay(250);
    digitalWrite (DATA, LOW);
  }
  else
  {
    digitalWrite (DATA, LOW);
    Serial.print ("0");
    delay(250);
  }
  delay(100);
  digitalWrite (CLOCK, LOW);
  delay(50);
  
}


