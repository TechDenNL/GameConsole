#ifndef RUN_GAME_H
#define RUN_GAME_H
#include <Arduino.h>
#include "Led_matrix.h"

class RunGame {
  public:
    RunGame();
  private:
    void splash();
    void start();
    void nextLevel();
};

#endif
