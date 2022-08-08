#ifndef AUDIO_DRIVER_H
#define AUDIO_DRIVER_H
#include "musicNotes.h"
#define BUZZER 9

class AudioDriver {
  public:
    static void buzz (int, int);
    static void playNote(int, int);
};

#endif
