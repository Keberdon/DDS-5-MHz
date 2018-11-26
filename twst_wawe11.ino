#include <SPI.h>
const int SINE = 0x2000;
const int SQUARE = 0x2028;
const int TRIANGLE = 0x2002;
int wave = 0;
int waveType = SINE;
//int waveType = SQUARE;
int wavePin = 7;

const float refFreq = 25000000.0;
const int FSYNC = 53; // Standard SPI pins for the AD9833 waveform generator.
const int CLK = 52;
const int DATA = 51;
unsigned long freq = 1000000;

void setup() {
pinMode(wavePin, INPUT_PULLUP);
SPI.begin();
delay(50);
AD9833reset();
AD9833setFrequency(freq, waveType);
}

void loop() {

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
