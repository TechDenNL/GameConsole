#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#define AMOUNT_OF_LEDS 30
#define AMOUNT_OF_LEDS_HORIZONTAL 5
#define AMOUNT_OF_LEDS_VERTICAL 6

#include <Arduino.h>

class LedMatrix {
  public:
    static uint8_t matrix[AMOUNT_OF_LEDS][2];
};

#endif
