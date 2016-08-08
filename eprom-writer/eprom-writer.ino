// sketch to write 2532 EPROMs
// data to write is placed in a static array in the sketch
//
// set the VPP switch to 5V before powering up
// once LEDx lights up, set VPP switch to select the programming voltage
// open the serial monitor to start programming
// when finished, switch VPP back to 5V and disconnect the Arduino


  #define DATA_LENGTH 4096
  
  const PROGMEM byte ROM_DATA[DATA_LENGTH]=
  {
    // your data here!
  };

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

void programByte(byte data, unsigned int address)
{
  // set up address and data
  setAddress(address);
  writeData(data);

  // wait for address and data to settle (probably unnecessary)
  delay(1);

  // programming pulse to write data
  digitalWrite(PD,LOW);
  delay(50);// 50ms
  digitalWrite(PD,HIGH);
}

// program some bytes
void programBytes(void)
{
  unsigned int address,ptr;

  for(address=0,ptr=0;address<DATA_LENGTH;address++,ptr++)
  {
    // read data byte from flash
    byte data = pgm_read_byte_near(ROM_DATA+address);
    
    programByte(data,address);
    Serial.print("Wrote byte ");
    Serial.print((data>>4)&0x0f, HEX);
    Serial.print(data&0x0f, HEX);
    Serial.print(" to ");
    Serial.print((address>>8)&0x0f, HEX); // address
    Serial.print((address>>4)&0x0f, HEX); // address 
    Serial.println(address&0x0f, HEX);  // address
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

  // init data pins to output (write mode)
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);

  writeData(0xff);

  Serial.begin(9600);
   
  // while the serial stream is not open, do nothing:
  while (!Serial) 

  delay(100);

  programBytes();

  Serial.println("Finished.");
  
}

// the loop function runs over and over again forever
void loop() {
}


