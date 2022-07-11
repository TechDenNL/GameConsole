#include "Graphic_driver.h"
#include "Led_matrix.h"

uint8_t numbers[10][AMOUNT_OF_LEDS_VERTICAL] = {
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

void GraphicDriver::drawFrame(uint8_t frameData[AMOUNT_OF_LEDS_VERTICAL]) {
  uint8_t ledNumber = 0;
  for (uint8_t y = 0; y < AMOUNT_OF_LEDS_VERTICAL; y++) {
    uint8_t rowData = frameData[y];
    for (uint8_t x = 0; x < AMOUNT_OF_LEDS_HORIZONTAL; x++) {
      if (rowData >> x & 1) {
        DDRC = LedMatrix::matrix[ledNumber][0];
        PORTC = LedMatrix::matrix[ledNumber][1];
      } else {
        DDRC = 0;
        PORTC = 0;
      }
      delayMicroseconds(100);
      ledNumber++;
    }
  }
}

void GraphicDriver::drawNumber(uint8_t number, uint16_t delay) {
  for (int i = 0; i < delay; i++) {
    GraphicDriver::drawFrame(numbers[number]);
  }
}

void GraphicDriver::emptyFrame() {
  uint8_t frameData[AMOUNT_OF_LEDS_VERTICAL] = {0,0,0,0,0,0};
  GraphicDriver::drawFrame(frameData);
}