/*
  Schematic; GameConsole v3.31
  Board; Arduino Uno 
  Programmer; AVRISP mkII
  
  Name; GameConsole Test_Board to test LEDs, Buzzer and Buttons

*/

#include "Graphic_driver.h"
#include "Audio_driver.h"
#include "Led_matrix.h"
#include "Sprites.h"

// Buttons pin defines
#define BUTTON_L 8 // Button Left pin - Arduino pin D8
#define BUTTON_D 7 // Button Down pin - Arduino pin D7
#define BUTTON_U 6 // Button Up pin - Arduino pin D6
#define BUTTON_R 5 // Button Right pin - Arduino pin D5

//define directions
#define LEFT 1
#define DOWN 2
#define UP 3
#define RIGHT 4

#define GO_LEFT 1
#define GO_RIGHT -1
#define GO_UP -1
#define GO_DOWN 1

byte frameData[ROWS]; //Led image buffer

int ballPosX = EDGE_LEFT; // init ball X position
int ballPosY = EDGE_UP ; // init ball Y position
int ballDirX;
int ballDirY;

#define ALIVE_LED LED_BUILTIN   // LED_BUILTIN (arduino pin13) as alive-blink | chip pin19 (SCK) 
#define ALIVE_LED_TIME 2000 //aliveLED blink time
//bool aliveLEDState= HIGH; //for active LOW LED
bool aliveLEDState= LOW; //for active HIGH Led
uint8_t aliveLEDCounter=0;
unsigned long aliveLEDTime=ALIVE_LED_TIME; //long time off

unsigned long currentMillis;
// store last times
unsigned long previousBallMillis = 0; 
unsigned long previousDispBlinkMillis = 0; 
unsigned long previousBlinkMillis = 0; 
unsigned long previous_aliveLEDMillis = 0;

#define BALLSPEED 150    // speed of ball (mS) | the lower the value the faster the speed

#define BLINKTIME 150    // Display blinktime (mS)
uint8_t blinkTime = BLINKTIME;

#define MAXBLINK 5  //5 times on and off
uint8_t blinkSet = MAXBLINK;

bool blinkDisplay=false;
uint8_t blinkCount = 0;
bool displayOff = false;

//Melodies // Hence; the LED Display will stop during playing a melody
#define WELCOME 1 // welcome melody
// notes in the Welcome melody:
int welcomeMelody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int welcomeNoteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};

//Sounds // Hence; The LED Display will stop during playing long sounds
#define YOURTURN 1  
#define GOTIT 2 
#define BYEBYE 3 
//.... so this sounds must me realy short because the are played during playing
#define PLOK 4 // PLOK sound
#define PLIK 5 // PLIK sound

void setup() {
  // initialize serial communication:
  Serial.begin(19200);

  Serial.println("Charlieplexing Test board");
  Serial.println("");     
  Serial.print("press buttons to scroll the ball");   
  Serial.println("");  
  
   pinMode(ALIVE_LED, OUTPUT);
  
  // Setup the the buttons with an internal pull-up
  pinMode(BUTTON_L,INPUT_PULLUP);
  pinMode(BUTTON_D,INPUT_PULLUP);
  pinMode(BUTTON_U,INPUT_PULLUP);
  pinMode(BUTTON_R,INPUT_PULLUP);

  //welcome show
  scrollLeds(50,1);
  scrollLeds(50,0);

  //When Sprites are executed in Loop the will disturb the loopsequence
  GraphicDriver::drawSprite(empty, 500);
  GraphicDriver::drawSprite(smile, 1000); // show smile 1000mS
  GraphicDriver::drawSprite(wink, 500); 
  GraphicDriver::drawSprite(smile, 1000); 
  
  playMelody(WELCOME);  // Hence; the LED Display will stop during playing a melody
  
  updateFrameBuffer(); //initial leds on
  blinkDisplay=true;
 
}

void loop() {
//in Loop:
  //1 - Always update Milli's
  currentMillis = millis(); 

  //2 - Trigger drawFrame
  if (!blinkDisplay) GraphicDriver::drawFrame(frameData); // write the frameData buffer into the LED's; 
  else blinkLeds();   //check for blinking display

  //3 - I am alive blinking
  IamAlive();

//===================================================================
// >>enter your code from here<<

  uint8_t button = readButtons();
  if (currentMillis - previousBallMillis >= BALLSPEED) {
    previousBallMillis = currentMillis;    
    if (button) {
      doSomething(button);
     }
  }
}
//======================Loop ends=================

//====================FUNCTIONS===================
uint8_t readButtons(){
  uint8_t button=0;  
  // read button status
  if(!digitalRead(BUTTON_L)) button = LEFT;
  else if(!digitalRead(BUTTON_R)) button = RIGHT;
  else if(!digitalRead(BUTTON_U)) button = UP;
  else if(!digitalRead(BUTTON_D)) button = DOWN;
  return(button);
}

void doSomething(uint8_t button){
  //define your button actions here
  switch (button) {
    case LEFT:    
      Serial.print("Button; ");
      Serial.println("Left");
      ballPosX+=GO_LEFT;
      break;

    case RIGHT:    
      Serial.print("Button; ");
      Serial.println("Right");
      ballPosX+=GO_RIGHT;
      break;

    case DOWN:
      Serial.print("Button; ");    
      Serial.println("Down");
      ballPosY+=GO_DOWN;
      break;
      
    case UP:    
      Serial.print("Button; ");
      Serial.println("Up");
      ballPosY+=GO_UP;      
      break;
      
  }
  updateFrameBuffer();
}

void updateFrameBuffer() {
//  GraphicDriver::emptyFrame();
    emptyFrame(); // clears the frameData buffer  
  //=====================================
  // >>define your game logic here<<

  // in this case:
  // 1 - check borders
  if (ballPosX > EDGE_LEFT) { 
    ballPosX=EDGE_RIGHT;
    ballPosY+=GO_UP;
    if (ballPosY < EDGE_UP) ballPosY=EDGE_DOWN;
    playSound (PLIK);
  }
  if (ballPosX < EDGE_RIGHT) {
    ballPosX=EDGE_LEFT;
    ballPosY+=GO_DOWN;
    if (ballPosY > EDGE_DOWN) ballPosY=EDGE_UP;    
    playSound (PLOK);
  }
  if (ballPosY < EDGE_UP) {
    ballPosY=EDGE_DOWN;
    ballPosX+=GO_LEFT;
    if (ballPosX > EDGE_LEFT) ballPosX=EDGE_RIGHT;    
    playSound (PLIK);
  }
  if (ballPosY > EDGE_DOWN) {
    ballPosY=EDGE_UP;
    ballPosX+=GO_RIGHT;
    if (ballPosX < EDGE_RIGHT) ballPosX=EDGE_LEFT;    
    playSound (PLOK);
  }
  //2 - update frameData
  frameData[ballPosY] = 1 << ballPosX;
}

void scrollLeds(int fillSpeed, boolean value){
  boolean fillLoop=true;
  unsigned long previousMillis = millis(); 
  uint8_t x = 0;
  uint8_t y = 0;
  int tempData;
  while (fillLoop){
    currentMillis = millis();
    if (currentMillis - previousMillis >= fillSpeed) {
      previousMillis = currentMillis;
      if (value) {
        tempData = 1 << x; //shift "one"
        frameData[y] = frameData[y] | tempData;
      }
      if (!value){
        tempData = 254 << x; //shift "zero"
        frameData[y] = frameData[y] & tempData;
      }
      x++;
      if (x>=COLOMNS) {
        y++;
        x=0;
      }
      if (y>=ROWS) fillLoop=false;
    }
  GraphicDriver::drawFrame(frameData); 
  }
}

void emptyFrame(){
  for(uint8_t i = 0; i < ROWS; i++) {
    frameData[i] = 0; //reset all leds
  }
}

void blinkLeds(){
  if (currentMillis - previousBlinkMillis >= blinkTime) {
      previousBlinkMillis = currentMillis;
        displayOff = !displayOff;
        if (blinkSet>0) blinkCount++;
      }
      if (!displayOff) GraphicDriver::drawFrame(frameData); // write the frameData buffer into the LED's;
      else GraphicDriver::emptyFrame(); // clears all LEDs (but doesn't clear the frameData buffer)
  if (blinkSet>0) {
    if (blinkCount>=blinkSet*2) {
        blinkDisplay=false;
        blinkCount=0;
    }    
  }
}

//The IamAlive LED will blink two times short - followed bij a long time off
void IamAlive(){
  if (currentMillis - previous_aliveLEDMillis >= aliveLEDTime) {
    if (aliveLEDCounter==1){
      digitalWrite(LED_BUILTIN, aliveLEDState);
      aliveLEDTime=10;
      aliveLEDState = !aliveLEDState;            
    }
    else if (aliveLEDCounter==2){
      digitalWrite(LED_BUILTIN, aliveLEDState);
      aliveLEDTime=200;
      aliveLEDState = !aliveLEDState;            
    }
    else if (aliveLEDCounter==3){
      digitalWrite(LED_BUILTIN, aliveLEDState);
      aliveLEDTime=10;
      aliveLEDState = !aliveLEDState;            
    }
    else{
      digitalWrite(LED_BUILTIN, aliveLEDState);
      aliveLEDTime=ALIVE_LED_TIME;
      aliveLEDState = !aliveLEDState; 
      aliveLEDCounter=0;
    }
    aliveLEDCounter++;
    // save the last time the LED has blinked
    previous_aliveLEDMillis = currentMillis;
  }
}

void copy(uint8_t arraySource[]){ //Copy array to frameData Buffer
  for(int i=0; i<6; i++){
    frameData[i]=arraySource[i];
  }
}

void playSound (uint8_t sound){
  switch (sound) {
    case YOURTURN:
      AudioDriver::buzz(280, 80); //pin, freq, duration
      //    delay (150); // Delay can't used in event handler (causes crashes)
      delay(150);
      AudioDriver::buzz(250, 500);
      delay (1000);
      break; 
    
    case GOTIT:
      AudioDriver::buzz(250, 80); //pin, freq, duration
      delay(150);
      AudioDriver::buzz(250, 80); 
      delay (150);
      AudioDriver::buzz(250, 80); 
      delay (150);
      AudioDriver::buzz(200, 500);
      delay (1000);
      break;
    
    case BYEBYE:
      AudioDriver::buzz(200, 80); //pin, freq, duration
      delay (300);
      AudioDriver::buzz(190, 80); 
      delay (300);
      AudioDriver::buzz(180, 80); 
      delay (300);
      AudioDriver::buzz(170, 750);
      delay (1000);
      break;
    
    case PLIK:
      AudioDriver::buzz(600, 1); //pin, freq, duration    
      break;
    
    case PLOK:
      AudioDriver::buzz(2000, 1); //pin, freq, duration
      break;
  }
}

void playMelody(int melody){
  switch (melody) {
    case WELCOME:
      // iterate over the notes of the melody:
      for (int thisNote = 0; thisNote < 8; thisNote++) {
        AudioDriver::playNote(welcomeMelody[thisNote], welcomeNoteDurations[thisNote]);
      }
      break;
  }
}
