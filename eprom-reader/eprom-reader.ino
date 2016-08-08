// sketch to read 2532 EPROMs
// data output as hex dump or Intel Hex file format
//
// Insert EPROM into the socket and power up the Arduino
// Once the sketch is uploaded open the serial monitor
// The EPROM contents will be read out and dumped over the serial monitor
// Comment/uncomment dumpRomText() and dumpRomIntelHex() to select output format

// address lines
#define AD11  A2
#define AD10  A3
#define AD9   A4
#define AD8   A5
#define AD7   0
#define AD6   1
#define AD5   2
#define AD4   3
#define AD3   4
#define AD2   5
#define AD1   6
#define AD0   7

// data lines
#define D7    A1
#define D6    A0
#define D5    11
#define D4    12
#define D3    13
#define D2    10
#define D1    9
#define D0    8

// control lines
#define PD    14 // power down/!program

void setAddress(unsigned int address)
{
  digitalWrite(AD0,  address&0x001?HIGH:LOW);
  digitalWrite(AD1,  address&0x002?HIGH:LOW);
  digitalWrite(AD2,  address&0x004?HIGH:LOW);
  digitalWrite(AD3,  address&0x008?HIGH:LOW);
  digitalWrite(AD4,  address&0x010?HIGH:LOW);
  digitalWrite(AD5,  address&0x020?HIGH:LOW);
  digitalWrite(AD6,  address&0x040?HIGH:LOW);
  digitalWrite(AD7,  address&0x080?HIGH:LOW);
  digitalWrite(AD8,  address&0x100?HIGH:LOW);
  digitalWrite(AD9,  address&0x200?HIGH:LOW);
  digitalWrite(AD10, address&0x400?HIGH:LOW);
  digitalWrite(AD11, address&0x800?HIGH:LOW);
}

byte readData(void)
{
  byte data = 0;
 
  data|=digitalRead(D0)==HIGH?0x01:0x00;
  data|=digitalRead(D1)==HIGH?0x02:0x00;
  data|=digitalRead(D2)==HIGH?0x04:0x00;
  data|=digitalRead(D3)==HIGH?0x08:0x00;
  data|=digitalRead(D4)==HIGH?0x10:0x00;
  data|=digitalRead(D5)==HIGH?0x20:0x00;
  data|=digitalRead(D6)==HIGH?0x40:0x00;
  data|=digitalRead(D7)==HIGH?0x80:0x00;

  return data;
}

byte readAddress(unsigned int address)
{
    setAddress(address);
    return readData();
}

// dump a 4k 2532 ROM as text
void dumpRomText(void)
{
  unsigned int address;
  byte data;
  String textDump;

  for(address=0x0000;address<0x1000;address++)
  {
    if(address%0x010==0)
    {
      Serial.print("  ");
      Serial.println(textDump);
      
      Serial.print("0x0");
      Serial.print((address>>8)&0x0f, HEX); 
      Serial.print((address>>4)&0x0f, HEX); 
      Serial.print(address&0x0f, HEX); 
      Serial.print("  ");

      textDump="";
    }

    data = readAddress(address);
    Serial.print(data>>4, HEX); 
    Serial.print(data&0xf, HEX); 
    Serial.print(" ");

    if(data>31 && data<127)
    {
      char ch = (char)data;
      textDump+=ch;
    }
    else
    {
      textDump+='.';
    }
  }

  Serial.print("  ");
  Serial.println(textDump);

}

// dump a 4k 2532 ROM in Intel hex format
void dumpRomIntelHex(void)
{
  unsigned int address;
  byte data;
  byte csum;

  for(address=0x0000;address<0x1000;address++)
  {
    if(address%0x010==0)
    {
      if(address>0)
      {
          csum = 0xff-(csum-1);
          Serial.print(csum>>4, HEX);
          Serial.println(csum&0x0f, HEX);
      }
      
      Serial.print(":100");// header
      Serial.print((address>>8)&0x0f, HEX); // address
      Serial.print((address>>4)&0x0f, HEX); // address 
      Serial.print(address&0x0f, HEX);  // address
      Serial.print("00"); // type (data)

      csum=0x10+(address>>8)+(address&0xff)+0x00;
    }

    data = readAddress(address);

    csum+=data;
    
    Serial.print(data>>4, HEX); 
    Serial.print(data&0xf, HEX); 
  }

  if(address>0)
  {
      csum = 0xff-(csum-1);
      Serial.print(csum>>4, HEX);
      Serial.println(csum&0x0f, HEX);
  }
  Serial.println(":00000001FF");// EOF

}

// the setup function runs once when you press reset or power the board
void setup() {

  // ensable chip
  pinMode(PD, OUTPUT);
  digitalWrite(PD,LOW);

  // init address pins
  pinMode(AD11, OUTPUT);
  pinMode(AD10, OUTPUT);
  pinMode(AD9, OUTPUT);
  pinMode(AD8, OUTPUT);
  pinMode(AD7, OUTPUT);
  pinMode(AD6, OUTPUT);
  pinMode(AD5, OUTPUT);
  pinMode(AD4, OUTPUT);
  pinMode(AD3, OUTPUT);
  pinMode(AD2, OUTPUT);
  pinMode(AD1, OUTPUT);
  pinMode(AD0, OUTPUT);

  // init data pins
  pinMode(D0, INPUT);
  pinMode(D1, INPUT);
  pinMode(D2, INPUT);
  pinMode(D3, INPUT);
  pinMode(D4, INPUT);
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  pinMode(D7, INPUT);

  setAddress(0x000);

  Serial.begin(9600);
  
  // while the serial stream is not open, do nothing:
  while (!Serial) 

  delay(100);

  // dump the ROM contents in the format of your choice
  //dumpRomText();
  dumpRomIntelHex();
}

// the loop function runs over and over again forever
void loop() {
}


