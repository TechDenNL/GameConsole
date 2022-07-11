#include "Audio_driver.h"
#include <Arduino.h>

void AudioDriver::buzz (int frequency, int durationLength) {
  tone(BUZZER, frequency, durationLength);
}