/*
  - LED attached from pin 13 to ground
  - pushbutton attached to pin 2 from +5V 
  - 10K resistor attached to pin 2 from ground*/
#include <SPI.h>
const int SINE = 0x2000;
const int SQUARE = 0x2028;
const int TRIANGLE = 0x2002;
int wave = 0;
int waveType = SINE;

const int buttonPin = 2;
const int PUpin = 3;
const int PDpin = 4; 
const int buttonPin2 = 5;
const float refFreq = 25000000.0;
const int FSYNC = 53; // Standard SPI pins for the AD9833 waveform generator.
const int CLK = 52;
const int DATA = 51;
unsigned long freq = 1000;
long myfreq[] = {10000,20000, 50000, 100000, 200000, 500000, 1000000};
int i = 0;
int buttonState = 0;
int buttonState2 = 0;

void setup() {
pinMode(LED_BUILTIN, OUTPUT);
pinMode(buttonPin, INPUT_PULLUP);
pinMode(buttonPin2, INPUT_PULLUP);
pinMode(PUpin, OUTPUT);
pinMode(PDpin, OUTPUT);
digitalWrite(PUpin, HIGH);
digitalWrite(PDpin, HIGH);
SPI.begin();
delay(50);
  
Serial.begin(9600);   // initialize serial communications at 9600 bps
AD9833reset();
AD9833setFrequency(freq, waveType);
delay(1000);
AD5228setVolume(-32);    // reset AD5228 - volume 0
delay(1000); }   

void loop() {
   while(digitalRead(buttonPin2)== 0 ){};
   buttonState = digitalRead(buttonPin);
      if (buttonState == HIGH) {
            waveType = SINE ; } else {
            waveType = SQUARE ; }
  freq = myfreq[i];
  AD9833setFrequency(freq, waveType);
  i= (i+1)%7 ;
      Serial.print("Freqeuency = "); Serial.print(freq,DEC); Serial.print(" Hz");    
      Serial.print("\t type = ");
      Serial.println(waveType,HEX);
      AD5228scanVolume();
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void AD9833reset() {
WriteRegister(0x100);   // Write '1' to AD9833 Control register bit D8.
delay(10);
}

void AD9833setFrequency(long frequency, int Waveform) {
  
long FreqWord = (frequency * pow(2, 28)) / refFreq;
int MSB = (int)((FreqWord & 0xFFFC000) >> 14);   //Only lower 14 bits are used for data
int LSB = (int)(FreqWord & 0x3FFF);
//Set control bits 15 ande 14 to 0 and 1, respectively, for frequency register 0
LSB |= 0x4000;
MSB |= 0x4000; 
WriteRegister(0x2100);
WriteRegister(LSB);                  // Write lower 16 bits to AD9833 registers
WriteRegister(MSB);                    // Write upper 16 bits to AD9833 registers.
WriteRegister(0xC000);                // Phase register
WriteRegister(Waveform);               // Exit & Reset to SINE, SQUARE or TRIANGLE
}
void WriteRegister(int dat) { 
      // Display and AD9833 use different SPI MODES so it has to be set for the AD9833 here.
SPI.setDataMode(SPI_MODE2);  
digitalWrite(FSYNC, LOW);  // Set FSYNC low before writing to AD9833 registers
delayMicroseconds(10);     // Give AD9833 time to get ready to receive data.
SPI.transfer(highByte(dat));   // Each AD9833 register is 32 bits wide and each 16
SPI.transfer(lowByte(dat));    // bits has to be transferred as 2 x 8-bit bytes.
digitalWrite(FSYNC, HIGH);      //Write done. Set FSYNC high
}
void AD5228scanVolume(void){
  int j;
  for (j=1; j< 33; ++j){
    AD5228setVolume(1);
    Serial.print("Volume = ");
    Serial.println(j,DEC);
       while(digitalRead(buttonPin2)== 0 ){};
     }
     
  for (j=1; j< 33; ++j){
    AD5228setVolume(-1);
    Serial.print("Volume = ");
    Serial.println(-j,DEC);
       while(digitalRead(buttonPin2)== 0 ){};
     }

  }

void AD5228setVolume(int volume) {
  int i;
  if (volume > 0) {
      for (i=0; i< volume; ++i){
        digitalWrite(PUpin, LOW);
        delay(12);
        digitalWrite(PUpin, HIGH);
        delay(1);}}
   else  
       for (i=0; i< -volume; ++i){
        digitalWrite(PDpin, LOW);
        delay(12);
        digitalWrite(PDpin, HIGH);
        delay(1);}
  delay(2000);
}
