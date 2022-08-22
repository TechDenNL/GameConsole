#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#define COLOMNS 5 // number of LED horizontal
#define ROWS 6    // number of LED vertical
#define AMOUNT_OF_LEDS COLOMNS*ROWS

#define EDGE_LEFT COLOMNS-1
#define EDGE_RIGHT 0
#define EDGE_UP 0
#define EDGE_DOWN ROWS-1

#include <Arduino.h>

class LedMatrix {
  public:
    static uint8_t matrix[AMOUNT_OF_LEDS][2];
};

#endif
