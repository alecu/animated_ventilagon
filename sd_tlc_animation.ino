#include "Tlc5940.h"
#include <SD.h>
#include <SPI.h>

const int chipSelect = 10;
const int HALL_SENSOR = 2;

File animFolder;
File dataFile;
#define DEBUG(x) Serial.println(x)

void PrintHex8(uint8_t *data, uint8_t length)
{
#if DEBUG
  Serial.print("0x"); 
  for (int i=0; i<length; i++) { 
    if (data[i]<0x10) {
      Serial.print("0");
    } 
    Serial.print(data[i],HEX); 
    Serial.print(" "); 
  }
#endif
}


volatile unsigned long last_turn = 0;
volatile unsigned long last_turn_duration = 10L;
volatile bool new_loop = true;

void hall_activated()
{
  unsigned long this_turn = micros();
  unsigned long this_turn_duration = this_turn - last_turn;
  last_turn_duration = this_turn_duration;
  last_turn = this_turn;
  new_loop = true;;
}

void setup()
{
  Tlc.init();

#if DEBUG
  Serial.begin(115200);
  while (!Serial) {
  }
#endif

  pinMode(HALL_SENSOR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(HALL_SENSOR), hall_activated, FALLING);

  if (!SD.begin(chipSelect)) {
    DEBUG("Card failed, or not present");
    return;
  }
  DEBUG("card initialized.");

  animFolder = SD.open("anim");
  if (!animFolder) {
    DEBUG("no hay carpeta de animaciones");
  }
}

void loop()
{  
  if (new_loop) {
    new_loop = false;
    dataFile = animFolder.openNextFile();
    if (!dataFile) {
      animFolder.rewindDirectory();
      dataFile = animFolder.openNextFile();
    }
  }
  if (dataFile.available()) {
    dataFile.read(tlc_GSData, NUM_TLCS * 24);
    Tlc.update();
    delayMicroseconds(200);
  }
}
