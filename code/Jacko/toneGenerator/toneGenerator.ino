
/*
  Schematic; Snakegame.pro | v3.1
  Board; Arduino Pro or Pro Mini
  Snakegame for Charlieplexing with 30 LEDs

  Ledmatrix;
  led1  led2  led3  led4  led5
  led6  led7  led8  led9  led10
  led11 led12 led13 led14 led15
  led16 led17 led18 led19 led20
  led21 led22 led23 led24 led25
  led26 led27 led28 led29 led30
*/

// DDRB: direction port b
// DDRC: direction port c
// PORTB: data port b
// PORTC: data port c
#define AMOUNT_OF_LEDS 30
#define AMOUNT_OF_LEDS_HORIZONTAL 5
#define AMOUNT_OF_LEDS_VERTICAL 6

#define DIRECTION_UP 0
#define DIRECTION_RIGHT 1
#define DIRECTION_DOWN 2
#define DIRECTION_LEFT 3

// Buttons & buzzer pin defines
#define BUTTON_L 8
#define BUTTON_D 7
#define BUTTON_U 6
#define BUTTON_R 5
#define BUZZER 9

uint8_t matrix[AMOUNT_OF_LEDS][2] = {
  {B00000011, B00000001}, //  01  CHP_A CHP_B
  {B00000101, B00000001}, //  02  CHP_A CHP_C
  {B00001001, B00000001}, //  03  CHP_A CHP_D
  {B00010001, B00000001}, //  04  CHP_A CHP_E
  {B00100001, B00000001}, //  05  CHP_A CHP_F
  {B00000011, B00000010}, //  06  CHP_B CHP_A
  {B00000110, B00000010}, //  07  CHP_B CHP_C
  {B00001010, B00000010}, //  08  CHP_B CHP_D
  {B00010010, B00000010}, //  09  CHP_B CHP_E
  {B00100010, B00000010}, //  10  CHP_B CHP_F
  {B00000101, B00000100}, //  11  CHP_C CHP_A
  {B00000110, B00000100}, //  12  CHP_C CHP_B
  {B00001100, B00000100}, //  13  CHP_C CHP_D
  {B00010100, B00000100}, //  14  CHP_C CHP_E
  {B00100100, B00000100}, //  15  CHP_C CHP_F
  {B00001001, B00001000}, //  16  CHP_D CHP_A
  {B00001010, B00001000}, //  17  CHP_D CHP_B
  {B00001100, B00001000}, //  18  CHP_D CHP_C
  {B00011000, B00001000}, //  19  CHP_D CHP_E
  {B00101000, B00001000}, //  20  CHP_D CHP_F
  {B00010001, B00010000}, //  21  CHP_E CHP_A
  {B00010010, B00010000}, //  22  CHP_E CHP_B
  {B00010100, B00010000}, //  23  CHP_E CHP_C
  {B00011000, B00010000}, //  24  CHP_E CHP_D
  {B00110000, B00010000}, //  25  CHP_E CHP_F
  {B00100001, B00100000}, //  26  CHP_F CHP_A
  {B00100010, B00100000}, //  27  CHP_F CHP_B
  {B00100100, B00100000}, //  28  CHP_F CHP_C
  {B00101000, B00100000}, //  29  CHP_F CHP_D
  {B00110000, B00100000}  //  30  CHP_F CHP_E
};

uint8_t frameData[AMOUNT_OF_LEDS_VERTICAL] = {
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
};

uint8_t track[6] = {1,5,2,4,3,3};
uint16_t levels[6] = {0,1,2,3,4,5};
uint16_t duration = 200;
uint16_t activeTone = 200;
uint8_t activeFrame = 0;

void tone() {
  
}

void setup() {
  Serial.begin(19200);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);

  //tatatadaa welcome
//  tone(BUZZER, 250, 80); //pin, freq, duration
//  delay (150);
//  tone(BUZZER, 250, 80);
//  delay (150);
//  tone(BUZZER, 250, 80);
//  delay (150);
//  tone(BUZZER, 200, 500);
}

void loop() {
  while (true) {
    readInput();

    updateFrameBuffer();
    drawFrame(35);

    activeFrame++;
    if(activeFrame == 6) {
      activeFrame = 0;
    }
  }
}

void writeFrame() {
  
}

void readInput() {
  //Serial.println(PIND, BIN);
  //delay(100);
  //return;
//  if (!digitalRead(BUTTON_U)) {
//    direction = DIRECTION_UP;
//  } else if (!digitalRead(BUTTON_R)) {
//    direction = DIRECTION_RIGHT;
//  } else if (!digitalRead(BUTTON_D)) {
//    direction = DIRECTION_DOWN;
//  } else if (!digitalRead(BUTTON_L)) {
//    direction = DIRECTION_LEFT;
//  }
}

uint8_t tailLength = 2;


void updateFrameBuffer() {
  for (uint8_t i = 0; i < AMOUNT_OF_LEDS_VERTICAL; i++) {
    frameData[i] = track[i] ? 1 << (track[i] - 1) : 0;

    if (i == activeFrame) {
      if(track[i]) {
        activeTone = 25 << track[i];
      } else {
        activeTone = 0;
      }
      
      frameData[i] = B11111111;
    }
  }
}

void drawFrame(uint32_t repeat) {
  for (uint32_t i = 0; i < repeat; i++) {
    uint8_t ledNumber = 0;
    for (uint8_t y = 0; y < AMOUNT_OF_LEDS_VERTICAL; y++) {
      uint8_t rowData = frameData[y];
      //Serial.println(rowData, BIN);
      for (uint8_t x = 0; x < AMOUNT_OF_LEDS_HORIZONTAL; x++) {
        // is on

        if (rowData >> x & 1) {
          DDRC = matrix[ledNumber][0];
          PORTC = matrix[ledNumber][1];
        } else {
          DDRC = 0;
          PORTC = 0;
        }

        delayMicroseconds(20);
        tone(BUZZER, activeTone, 20);

        ledNumber++;
      }
    }
  }

}
