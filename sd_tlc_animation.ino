#include "Tlc5940.h"
#include <SD.h>
#include <SPI.h>

const int chipSelect = 10;

File dataFile;

void PrintHex8(uint8_t *data, uint8_t length)
{
  Serial.print("0x"); 
  for (int i=0; i<length; i++) { 
    if (data[i]<0x10) {
      Serial.print("0");
    } 
    Serial.print(data[i],HEX); 
    Serial.print(" "); 
  }
}

void setup()
{
  Tlc.init();
  Serial.begin(115200);
  while (!Serial) {
  }

  Serial.print("Initializing SD card... a, b, c...");
  
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");
  dataFile = SD.open("degrade.bin");
  if (!dataFile) {
    Serial.println("error opening degrade.bin");
  }
}

void loop()
{
  //Serial.print(dataFile.position());
  //Serial.print(") ");
  dataFile.read(tlc_GSData, NUM_TLCS * 24);
  //PrintHex8(tlc_GSData, 12);
  //Serial.println();
  Tlc.update();

  if (dataFile.available() == false) {
    dataFile.seek(0);
  }

}
