#include "Audio_driver.h"
#include <Arduino.h>

void AudioDriver::buzz (int frequency, int durationLength) {
  tone(BUZZER, frequency, durationLength);
}

void AudioDriver::playNote(int melodyNote, int noteDuration){
  // to calculate the note duration, take one second divided by the note type.
  //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
  noteDuration = 1000 / noteDuration;
  AudioDriver::buzz(melodyNote, noteDuration);

  // to distinguish the notes, set a minimum time between them.
  // the note's duration + 30% seems to work well:
  int pauseBetweenNotes = noteDuration * 1.30;
  delay(pauseBetweenNotes); // Hence; the LED Display will stop during playing a melody
}
