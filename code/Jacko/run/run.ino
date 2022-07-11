#include "Test2.h"
#include "Graphic_driver.h"
#include "Audio_driver.h"
#include "Led_matrix.h"

// DDRB: direction port b
// DDRC: direction port c
// PORTB: data port b
// PORTC: data port c

#define DIRECTION_HOLD 0
#define DIRECTION_UP 1
#define DIRECTION_RIGHT 2
#define DIRECTION_DOWN 3
#define DIRECTION_LEFT 4

// Buttons & buzzer pin defines
#define BUTTON_L 8
#define BUTTON_D 7
#define BUTTON_U 6
#define BUTTON_R 5


#define TARGET_BLINK_DELAY 6

uint8_t direction = DIRECTION_UP;
uint8_t frameData[AMOUNT_OF_LEDS_VERTICAL] = {
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
};

void setup() {
  Serial.begin(19200);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);

  
  AudioDriver::buzz(500, 200);
  delay(300);
  AudioDriver::buzz(1000, 200);
  delay(300);
  AudioDriver::buzz(1500, 200);
}

void loop() {
  for (int i = 0; i < 10; i++) {
    GraphicDriver::drawNumber(1, 50);
    delay(100);
  }

  for (int i = 3; i > 0; i--) {
    GraphicDriver::drawNumber(i, 500);
    GraphicDriver::emptyFrame();
    delay(100);
  }
  
  randomizePositionTarget();
  while (true) {
    readInput();
    updateGameLogic();
    updateFrameBuffer();
    for (uint32_t i = 0; i < 20; i++) {
      GraphicDriver::emptyFrame();
    }
  }
}

void readInput() {
  if (!digitalRead(BUTTON_U)) {
    direction = DIRECTION_UP;
  } else if (!digitalRead(BUTTON_R)) {
    direction = DIRECTION_RIGHT;
  } else if (!digitalRead(BUTTON_D)) {
    direction = DIRECTION_DOWN;
  } else if (!digitalRead(BUTTON_L)) {
    direction = DIRECTION_LEFT;
  } else {
    direction = DIRECTION_HOLD;
  }
}

int8_t positionX = 0;
int8_t positionY = 0;
int8_t moved = false;

int8_t targetPositionX = 0;
int8_t targetPositionY = 0;

void pickedUpTarget() {
  AudioDriver::buzz(600, 35);
  delay(300);
  randomizePositionTarget();
}

void updateGameLogic() {
  if (targetPositionX == positionX && targetPositionY == positionY) {
    pickedUpTarget();

  }

  if (direction == DIRECTION_HOLD) {
    moved = false;
    return;
  }

  if (moved) {
    return;
  }

  if (direction == DIRECTION_LEFT) {
    if (positionX == 0) {
      positionX = AMOUNT_OF_LEDS_HORIZONTAL - 1;
    } else {
      positionX--;
    }
  } else if (direction == DIRECTION_RIGHT) {
    if (positionX == AMOUNT_OF_LEDS_HORIZONTAL - 1) {
      positionX = 0;
    } else {
      positionX++;
    }
  }

  if (direction == DIRECTION_UP) {
    if (positionY == 0) {
      positionY = AMOUNT_OF_LEDS_VERTICAL - 1;
    } else {
      positionY--;
    }
  } else if (direction == DIRECTION_DOWN) {
    if (positionY == AMOUNT_OF_LEDS_VERTICAL - 1) {
      positionY = 0;
    } else {
      positionY++;
    }
  }
  moved = true;
}

void randomizePositionTarget() {
  targetPositionX = random(0, 4);
  targetPositionY = random(0, 5);
}

int8_t targetBlinkTimer = TARGET_BLINK_DELAY;

void updateFrameBuffer() {
  targetBlinkTimer--;
  for (uint8_t y = 0; y < AMOUNT_OF_LEDS_VERTICAL; y++) {
    frameData[y] = 0;
  }

  frameData[positionY] = 1 << positionX;

  if (targetBlinkTimer < 2) {
    frameData[targetPositionY] = 1 << targetPositionX | frameData[targetPositionY];
  }

  if (targetBlinkTimer == 0) {
    targetBlinkTimer = TARGET_BLINK_DELAY;
  }
}
