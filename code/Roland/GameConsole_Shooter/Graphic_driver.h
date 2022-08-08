#ifndef GRAPHIC_DRIVER_H
#define GRAPHIC_DRIVER_H

#include "Led_matrix.h"



class GraphicDriver {
  public:
    static void drawFrame(uint8_t[ROWS]);
    static void drawNumber(uint8_t, uint16_t);
    static void emptyFrame();
    static void drawSprite(uint8_t *, int);
};

#endif
