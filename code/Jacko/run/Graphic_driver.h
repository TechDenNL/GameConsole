#ifndef GRAPHIC_DRIVER_H
#define GRAPHIC_DRIVER_H

#include "Led_matrix.h"

class GraphicDriver {
  public:
    static void drawFrame(uint8_t[AMOUNT_OF_LEDS_VERTICAL]);
    static void drawNumber(uint8_t, uint16_t);
    static void emptyFrame();
};

#endif