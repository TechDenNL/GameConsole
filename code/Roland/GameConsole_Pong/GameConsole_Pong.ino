/*
  Schematic; GameConsole v3.31
  Board; Arduino Uno 
  Programmer; AVRISP mkII
  
  Name; GameConsole pong game

  Functions;
  Buttons Left & Right to control the racket
  Button Up = play ball
  Button Down - Demo mode
  IamAlive LED - will blink two time short - followed bij a long time off
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

#define NILL COLOMNS+1 //outside screen

//int ballPosX = EDGE_RIGHT; // ball X position
int ballPosX = NILL; // ball X position
int ballPosY = EDGE_UP ; // ball Y position
int ballDirX = GO_LEFT;
int ballDirY = GO_UP;

bool blinkDisplay=false;
uint8_t blinkCount = 0;
bool displayOff = false;
#define RACKETINITPOS 2
int racketPosX = RACKETINITPOS;

//int racketWidth = 2; // width of racket is 2 leds

#define ALIVE_LED LED_BUILTIN   // LED_BUILTIN (arduino pin13) as alive-blink | chip pin19 (SCK) 
#define ALIVE_LED_TIME 2000 //aliveLED blink time
//bool aliveLEDState= HIGH; //for active LOW LED
bool aliveLEDState= LOW; //for active HIGH Led
uint8_t aliveLEDCounter=0;
unsigned long aliveLEDTime=ALIVE_LED_TIME; //long time off

unsigned long currentMillis;
// store last times
unsigned long previousBallMillis = 0; 
unsigned long previousRacketMillis = 0;
unsigned long previousBlinkMillis = 0;  
unsigned long previous_aliveLEDMillis = 0;

#define BALLSPEED 150    // speed of ball (mS)
//#define BALLSPEED 200    // speed of ball (mS) | the lower the value the faster the speed
#define RACKETSPEED 150    // speed of racket (mS)

#define BLINKTIME 150    // Display blinktime (mS)
uint8_t blinkTime = BLINKTIME;

#define MAXBLINK 5
uint8_t blinkSet = MAXBLINK;

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

bool hit_right= false;
bool hit_left = false;
bool you_lost = false;
bool play = false; //play semaphore 
bool setOnce = false;
bool play_onetime=true;
bool demo=false;

void setup() {
  // initialize serial communication:
  Serial.begin(19200);
  Serial.println("GameConsole pong");
  Serial.println("");     
  Serial.print("press UP to play new ball");   
  Serial.println("");
  
  pinMode(ALIVE_LED, OUTPUT); 

  // Setup the the buttons with an internal pull-up
  pinMode(BUTTON_L,INPUT_PULLUP);
  pinMode(BUTTON_D,INPUT_PULLUP);
  pinMode(BUTTON_U,INPUT_PULLUP);
  pinMode(BUTTON_R,INPUT_PULLUP);
  
  playMelody(WELCOME);  // Hence; the LED Display will stop during playing a melody
  //When Sprites are executed in Loop the will disturb the loopsequence
  GraphicDriver::drawSprite(smile, 1000); // show smile 1000mS
  GraphicDriver::drawSprite(wink, 500); 
  GraphicDriver::drawSprite(smile, 1000); 

  updateFrameBuffer(); //initial led on
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
  uint8_t r;
  uint8_t button = readButtons();
  if (button) {
    doSomething(button);
    if (play) updateFrameBuffer();    
  }

  if (you_lost && !blinkDisplay && setOnce) {  //when the blinking is over ...
      blinkDisplay=false;
      blinkCount=0;
      setOnce=false;
      you_lost=false;
      ballPosX = NILL; // hide ball
      GraphicDriver::drawSprite(sad, 2000);  
      playSound(BYEBYE);
    }
  if (you_lost && !blinkDisplay && !setOnce) {  //set blinking
    play = false;
    blinkDisplay=true;
    setOnce=true;
    racketPosX = RACKETINITPOS;
  }
 
  if (play) {  
    if (currentMillis - previousBallMillis >= BALLSPEED) {
      previousBallMillis = currentMillis;
      if (hit_left) {
        ballDirY = GO_UP;
        ballPosX = random (racketPosX, racketPosX+2); // ball X position (effect ball :-) )
        play_onetime=true;
        if (ballDirX == GO_LEFT) {
          ballDirX = GO_LEFT;        
          r=random(0,2);
          if (r){
            ballDirX = 0;
          }
        }
        if (ballDirX == GO_RIGHT || !ballDirX) {
          ballDirX = 0;        
          r=random(0,2);
          if (r){
            ballDirX = GO_LEFT;
            r=random(0,2);
            if (r){
              ballDirX = GO_RIGHT;
            }
          }
        }
        hit_left = false;        
      }
      
      if (hit_right) {
        ballDirY = GO_UP;
        play_onetime=true;
        ballPosX = random (racketPosX, racketPosX+2); // ball X position (effect ball :-) )
        if (ballDirX == GO_RIGHT) {
          ballDirX = GO_RIGHT;        
          r=random(0,2);
          if (r){
            ballDirX = 0;
          }
        }
        if (ballDirX == GO_LEFT || !ballDirX) {
          ballDirX = 0;        
          r=random(0,2);
          if (r){
            ballDirX = GO_RIGHT;
            r=random(0,2);
            if (r){
              ballDirX = GO_LEFT;
            }
          }
        }
        hit_right = false;           
      }
      
      ballPosX += ballDirX;
      //check colomn borders
      if ((ballPosX >= EDGE_LEFT)||(ballPosX <= EDGE_RIGHT)){
        ballDirX = GO_RIGHT;
        if (ballPosX <= EDGE_RIGHT) ballDirX = GO_LEFT;
        playSound (PLOK);
      }
      ballPosY += ballDirY;
      //check colomn borders
      if ((ballPosY >= EDGE_DOWN)||(ballPosY <= EDGE_UP)){
        ballDirY = GO_UP;
        if (ballPosY<=EDGE_UP){
          ballDirY = GO_DOWN;
          playSound (PLOK); 
        }     
      }
      if (demo) racketPosX=ballPosX;
      updateFrameBuffer();
    }
  }
  if (!you_lost) {
    if (currentMillis - previousRacketMillis >= RACKETSPEED) {
      previousRacketMillis = currentMillis;
      if (button==RIGHT) racketPosX+=GO_RIGHT;
      if (button==LEFT) racketPosX+=GO_LEFT;
      if (racketPosX >= EDGE_LEFT-1) racketPosX=EDGE_LEFT-1;
      if (racketPosX <= EDGE_RIGHT) racketPosX=EDGE_RIGHT;
      updateFrameBuffer();  
    }      
  }
}
//======================Loop ends=================

//====================FUNCTIONS===================
// read button status
uint8_t readButtons(){
  uint8_t button=0;
  if(!digitalRead(BUTTON_L)) button = LEFT;
  else if(!digitalRead(BUTTON_R)) button = RIGHT;
  if(!digitalRead(BUTTON_U)) button = UP;
  if(!digitalRead(BUTTON_D)) button = DOWN;  
  return(button);
}

void doSomething(uint8_t button){
  bool sema_once = false;
  switch (button) {
/* 
    case LEFT:    
      racketPosX+=GO_LEFT;
      break;    
    case RIGHT:    
      racketPosX+=GO_RIGHT;
      break;
*/
    case DOWN:
        //Demo
      if (!play || !demo){
        demo=true;
        play=true;
        you_lost=false;
        ballPosX = random (racketPosX, racketPosX+2); // ball X position
        ballPosY = EDGE_DOWN ; // ball Y position
        sema_once=true;          
      }
      break;

    case UP:    
      //start bullet
      if (!play || demo){
        demo=false;
        play=true;
        you_lost=false;
        ballPosX = random (racketPosX, racketPosX+2); // ball X position
        ballPosY = EDGE_DOWN ; // ball Y position
        sema_once=true;          
      }
      break;
   
  }
}

void updateFrameBuffer() {
    emptyFrame(); // clears the frameData buffer  
  //=====================================
  // >>define your game logic here<<

  // in this game:
  // 1 - Update frameData
  frameData[ROWS-1] = 3 << racketPosX;  //two width
  if (ballPosY == ROWS-1){
    byte temp = 1 << ballPosX;
    frameData[ROWS-1] = frameData[ROWS-1] | temp; //if ball and racket has the same row, combine their positions
    if (ballPosX == racketPosX){
      hit_right = true;
      if (play_onetime)playSound (PLIK);
    }
    else if (ballPosX == racketPosX+1){
      hit_left = true;
      if (play_onetime)playSound (PLIK);
    }
    else if (play){
      you_lost = true;
      playSound (PLOK);
    }
  }
  else frameData[ballPosY] = 1 << ballPosX;

//  Serial.println("frameData updated");
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
      play_onetime=false;    
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
