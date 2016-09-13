// sketch to write 2532 and 2516 EPROMs
// data to write is placed in a static array in the sketch
//
// set the chip type by #defining ROM_TYPE as TMS2532 or TMS2516
//
// set the VPP switch to 5V before powering up
// when LED1 and LED2 will flash alternately
// open the serial monitor to start
// programmer will perform blank check, program device and verify data was written ok
// serial output will prompt you to apply/disconnect VPP at the appropriate points

// put your EPROM image in 'image.h' 
#include "image.h"

// define chip types
#define TMS2532 0
#define TMS2516 1

// select the programming algorithm
#define ROM_TYPE  TMS2532
//#define ROM_TYPE  TMS2516

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
#define VPP   15 // programming voltage select (5V/25V) (not used, yet!)

void setDataOutput(void)
{
  // init data pins to output (write mode)
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
}

void setDataInput(void)
{
  // init data pins
  pinMode(D0, INPUT);
  pinMode(D1, INPUT);
  pinMode(D2, INPUT);
  pinMode(D3, INPUT);
  pinMode(D4, INPUT);
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  pinMode(D7, INPUT);
}

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

  // only 2532 devices have the AD11 line
  if(ROM_TYPE==TMS2532)
    digitalWrite(AD11, address&0x800?HIGH:LOW);
}

void writeData(byte data)
{
  digitalWrite(D0,  data&0x01?HIGH:LOW);
  digitalWrite(D1,  data&0x02?HIGH:LOW);
  digitalWrite(D2,  data&0x04?HIGH:LOW);
  digitalWrite(D3,  data&0x08?HIGH:LOW);
  digitalWrite(D4,  data&0x10?HIGH:LOW);
  digitalWrite(D5,  data&0x20?HIGH:LOW);
  digitalWrite(D6,  data&0x40?HIGH:LOW);
  digitalWrite(D7,  data&0x80?HIGH:LOW);
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


// make sure the device is blank (all locations 0xFF)
// or verify the program was written ok
bool verify(bool verifyProgram)
{
  unsigned int address;
  byte data;
  String textDump;
  bool success=true;
  
  for(address=0x0000;address<DATA_LENGTH;address++)//2532 or 2516
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

    setAddress(address);
    data = readData();

  // blank check or program check
    if(verifyProgram)
    {
      if(data!=pgm_read_byte_near(ROM_DATA+address))
      {
        Serial.print("*");// mark bad bytes
        success=false;
      }
      else
      {
        Serial.print(" ");
      }
    }
    else
    {
      if(data!=0xff)
        success=false;
    }
    
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

  return success;

}


// program and verify byte
int programByte(byte data, unsigned int address)
{
  byte rdata;

  // no point in writing 0xFF to an EPROM
  if(data==0xff)
  {
    Serial.println("");
    return 0;
  }
  
  // set up address and data
  setAddress(address);
  writeData(data);

  // wait for address and data to settle (probably unnecessary)
  delay(1);

  if(ROM_TYPE==TMS2532)
  {
    // programming pulse to write data
    digitalWrite(PD,LOW);
    delay(50);// 50ms
    digitalWrite(PD,HIGH);

    // we can't verify the byte when writing to 2532s
    // because we can't control VPP programatically 
  
  }

  // on 2516:
  //  PD is CS
  // AD11 is PD
  if(ROM_TYPE==TMS2516)
  {
    // programming pulse to write data
    digitalWrite(AD11, HIGH);
    delay(50);// 50ms
    digitalWrite(AD11, LOW);

    // read byte back to verify
    delay(1);
  
    setDataInput();
    digitalWrite(PD, LOW);
  
    delay(1);
  
    rdata = readData();
        
    digitalWrite(PD, HIGH);
    setDataOutput();  

    if(rdata!=data)
    {
      Serial.print("... Error writing byte, read back 0x");
      Serial.print((rdata>>4)&0x0f, HEX);
      Serial.print(rdata&0x0f, HEX);
      //return -1;
    }
    else
    {
      Serial.print("... OK");
    }
      
  }

  Serial.println("");

  return 0;
}

// program some bytes
void programBytes(void)
{
  unsigned int address,ptr;
  int result;

  for(address=0,ptr=0;address<DATA_LENGTH;address++,ptr++)
  {
    // read data byte from flash
    byte data = pgm_read_byte_near(ROM_DATA+address);
    
    Serial.print("Writing byte 0x");
    Serial.print((data>>4)&0x0f, HEX);
    Serial.print(data&0x0f, HEX);
    Serial.print(" to 0x");
    Serial.print((address>>8)&0x0f, HEX); // address
    Serial.print((address>>4)&0x0f, HEX); // address 
    Serial.print(address&0x0f, HEX);  // address

    result = programByte(data,address);

    // if byte not read back ok then stop programming
    if(result<0)
    {
      return;
    }
    
  }
}

// the setup function runs once when you press reset or power the board
void setup() {

  // disable chip
  pinMode(PD, OUTPUT);
  digitalWrite(PD,HIGH);

  // 5V on VPP
  pinMode(VPP, OUTPUT);
  digitalWrite(VPP,LOW);

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

  setAddress(0x000);

  setDataOutput();

  writeData(0xff);

  Serial.begin(9600);
   
  // while the serial stream is not open, alternately flash LED1 & LED2
  // to show the device is ready for VPP to be applied
  while (!Serial)
  {
    digitalWrite(AD0, HIGH);
    digitalWrite(D1,  LOW);
    delay(500);
    digitalWrite(AD0, LOW);
    digitalWrite(D1,  HIGH);
    delay(500);
  }

  // tuen LEDs off
  digitalWrite(AD0, LOW);
  digitalWrite(D1,  LOW);

  if(ROM_TYPE==TMS2532)
  {
    Serial.println("Programming 2532 device");
  }

  if(ROM_TYPE==TMS2516)
  {
    Serial.println("Programming 2516 device");
    
    // on 2516:
  //  PD is CS
  // AD11 is PD
    digitalWrite(PD, HIGH);
    digitalWrite(AD11, LOW);
  }

setDataInput();
digitalWrite(PD,LOW);

  Serial.println("Starting blank check...");
  if(verify(false)!=true)
  {
    Serial.println("Device not blank, stopping.");
    return;    
  }

digitalWrite(PD,HIGH);// important to set this high before VPP is applied
digitalWrite(AD11, LOW);// similarly for 2516
setDataOutput();

 while (Serial.available())
 Serial.read();
  Serial.println("Device is blank, connect VPP and press any key to start programming");
  while(!Serial.available()) ;

  delay(100);
  programBytes();

  while (Serial.available())
    Serial.read();

  Serial.println("Disconnect VPP, then press any key to verify ");
  while(!Serial.available()) ;

digitalWrite(PD,LOW);
setDataInput();

  if(verify(true)!=true)
  {
    Serial.println("Error programming EPROM :(");
    return;    
  }

  Serial.println("EPROM programmed successfully! :)");
  Serial.println("Finished.");
}

// the loop function runs over and over again forever
void loop() {
}


