#include "Graphic_driver.h"
#include "Led_matrix.h"

uint8_t numbers[10][ROWS] = {
  {
    B00000000,
    B00001110,
    B00001010,
    B00001010,
    B00001010,
    B00001110,
  },
  {
    B00000000,
    B00000100,
    B00000110,
    B00000100,
    B00000100,
    B00001110,
  },
  {
    B00000000,
    B00001110,
    B00001000,
    B00001110,
    B00000010,
    B00001110,
  },
  {
    B00000000,
    B00001110,
    B00001000,
    B00001110,
    B00001000,
    B00001110,
  },
  {
    B00000000,
    B00001010,
    B00001010,
    B00001110,
    B00001000,
    B00001000,
  },
  {
    B00000000,
    B00001110,
    B00000010,
    B00001110,
    B00001000,
    B00001110,
  },
  {
    B00000000,
    B00001110,
    B00000010,
    B00001110,
    B00001010,
    B00001110,
  },
  {
    B00000000,
    B00001110,
    B00001000,
    B00001000,
    B00001000,
    B00001000,
  },
  {
    B00000000,
    B00001110,
    B00001010,
    B00001110,
    B00001010,
    B00001110,
  },
  {
    B00000000,
    B00001110,
    B00001010,
    B00001110,
    B00001000,
    B00001110,
  },
};



void GraphicDriver::drawFrame(uint8_t frameData[ROWS]) {
  uint8_t ledNumber = 0;
  for (uint8_t y = 0; y < ROWS; y++) {
    uint8_t rowData = frameData[y];
    for (uint8_t x = 0; x < COLOMNS; x++) {
      //calculate led_matrixaddr from colomn & row nr
      uint8_t ledNumber=(y*COLOMNS+(COLOMNS-x))-1; //calculate led_matrixaddr from colomn & row nr
      if (rowData >> x & 1) {
        DDRC = LedMatrix::matrix[ledNumber][0];
        PORTC = LedMatrix::matrix[ledNumber][1];
        delayMicroseconds(50); // to make the led shine brighter
      } else {
        DDRC = 0;
        PORTC = 0;
      }
//      delayMicroseconds(100); // to make the led shine brighter
      ledNumber++;
    }
  }
}

void GraphicDriver::drawNumber(uint8_t number, uint16_t delay) {
  for (int i = 0; i < delay; i++) {
    GraphicDriver::drawFrame(numbers[number]);
  }
}

// clear all LEDs | This function doens't empty the FrameData buffer! - it can only clears the LED's - e.g. in the setup
void GraphicDriver::emptyFrame() {
  uint8_t frameData[ROWS] = {0,0,0,0,0,0};
  GraphicDriver::drawFrame(frameData);
}

void GraphicDriver::drawSprite(uint8_t *rowData, int waitTime){
  uint8_t frameData[ROWS] = {0,0,0,0,0,0};
  for(int y = 0; y < ROWS; y++) frameData[y] = rowData[y];
  boolean waitLoop=true;
  int i=0;
  waitTime = waitTime/100;
  unsigned long previousMillis = millis(); 
  unsigned long currentMillis = millis();
  while (waitLoop){
    currentMillis = millis();
    if (currentMillis - previousMillis >= 100) {
      previousMillis = currentMillis;
      i++;
      if (i>=waitTime) waitLoop=false;
    }
  GraphicDriver::drawFrame(frameData); 
  }
}
