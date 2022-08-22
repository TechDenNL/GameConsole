/*
  Schematic; GameConsole v3.31
  Board; Arduino Uno 
  Programmer; AVRISP mkII
  
  Name; GameConsole shooter game

  Functions;
  Buttons Left & Right to control the gun
  Button Up = fire button
  Button Down is start new game
  The playtime is determined by PLAYTIME
  If hitscore >=50% then Won, else Loose
  IamAlive LED - will blink two time short - followed bij a long time off
*/

#include "Graphic_driver.h"
#include "Audio_driver.h"
#include "Led_matrix.h"
#include "Sprites.h"

#define PLAYTIME 30 //playtime in sec

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

int gunPosX = 2; // gun X position - initial in the middle
int bulletPosX = gunPosX; // bullet X position 
int bulletPosY = EDGE_DOWN; // bullet Y position - always started at bottom row
int birdPosX = -1; // bird position - initial out of screen

bool play=0; //play semaphore 
bool fire=0; // fire signal
bool blinkDisplay=false;
uint8_t blinkCount = 0;
bool displayOff = false;
int playtimeCounter=0;
int birdDir=1;

#define WONSCORE 50 //50%
int scoreCounter=0;
int fireCounter=0;
bool countOnce=false;
bool birdFlies=false;

#define ALIVE_LED LED_BUILTIN   // LED_BUILTIN (arduino pin13) as alive-blink | chip pin19 (SCK) 
#define ALIVE_LED_TIME 2000 //aliveLED blink time
//bool aliveLEDState= HIGH; //for active LOW LED
bool aliveLEDState= LOW; //for active HIGH Led
uint8_t aliveLEDCounter=0;
unsigned long aliveLEDTime=ALIVE_LED_TIME; //long time off

unsigned long currentMillis;
// store last times
unsigned long previousBulletMillis = 0; 
unsigned long previousBirdMillis = 0; 
unsigned long previousPlayMillis = 0;
unsigned long previousBirdWaitMillis = 0;
unsigned long previousGunMillis = 0;
unsigned long previousBlinkMillis = 0; 
unsigned long previous_aliveLEDMillis = 0;

unsigned long birdSpeed = 100;    // speed of bird (mS)
unsigned long bulletSpeed = 40;    // speed of bullet (mS)
unsigned long birdWait = 100;    // bird wait time (mS)

#define BIRDSPEEDMIN 80
#define BIRDSPEEDMAX 400
#define BIRDWAITMIN 500
#define BIRDWAITMAX 3000
#define GUNSPEED 150    // speed of gun (mS)

#define BLINKTIME 150    // Display blinktime (mS)
uint8_t blinkTime = BLINKTIME;

#define MAXBLINK 5  //5 times on and off
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

void setup() {

  // initialize serial communication:
  Serial.begin(19200);
  Serial.println("GameConsole shooter game");
  Serial.println("");     
  Serial.print("press DOWN to play game for ");   
  Serial.print(PLAYTIME);  
  Serial.print(" sec");    
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

  //If sprites are executed in the Loop, they will disrupt the loop sequence
  //Therefore they should be copied into frameData buffer
  copy(downArrow); //set the sprite "downArrow" in frameData buffer
  blinkDisplay=true;
  blinkTime=500;
  blinkSet=0; //means; constant blinking
  playSound(YOURTURN); 
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
  if (currentMillis - previousGunMillis >= GUNSPEED) {
    previousGunMillis = currentMillis;    
    if (button && !fire) {  //!fire - to prevent erroneous counting due to contact-bouching
      doSomething(button);
     }
  }
  if (play) {
    if (currentMillis - previousPlayMillis >= 1000) {
      previousPlayMillis = currentMillis;
      playtimeCounter++;
      if (playtimeCounter>=PLAYTIME) {
        play=false;
        Serial.println ("GAME OVER; ");
        Serial.print ("HIT; ");
        Serial.println (scoreCounter);
        Serial.print ("MISSED; ");
        Serial.println (fireCounter-scoreCounter);
        Serial.println ("");        
        playtimeCounter=0;
        int birdPosX = -1; // bird position - initial out of screen
        if (fireCounter>0){
          if ((fireCounter-scoreCounter)<=fireCounter*WONSCORE/100) {
            playSound(GOTIT); 
            GraphicDriver::drawSprite(smile, 2000); // show smile sprite 2000mS
          }
          else {
            playSound(BYEBYE);
            GraphicDriver::drawSprite(sad, 2000); // show sad sprite 2000mS
          }          
        }
//        GraphicDriver::emptyFrame(); // doesn't clears the actual frameData buffer!
          emptyFrame(); // clears the frameData buffer
          playSound(YOURTURN);
          copy(downArrow); //set downArrow in frameData buffer
          blinkDisplay=true;
      }
    }

    if ((currentMillis - previousBirdWaitMillis >= birdWait) || birdFlies) {
      birdFlies=true;
      if (currentMillis - previousBirdMillis >= birdSpeed) {
        previousBirdMillis = currentMillis;
        birdPosX += birdDir;
        //check colomn borders
        if ((birdPosX > EDGE_LEFT)||(birdPosX < EDGE_RIGHT)){
          birdDir = -1;
          if (birdPosX < EDGE_RIGHT) birdDir=1;
          birdSpeed = random(BIRDSPEEDMIN, BIRDSPEEDMAX);
          birdWait = random(BIRDWAITMIN, BIRDWAITMAX);
          previousBirdWaitMillis = currentMillis;
          birdFlies=false;
        }
        updateFrameBuffer();
      }
  }
  
    if (fire){
      if (currentMillis - previousBulletMillis >= bulletSpeed) {
        previousBulletMillis = currentMillis;
        bulletPosY +=GO_UP;
        //check colomn borders
        if (bulletPosY < EDGE_UP) {
          bulletPosY=EDGE_DOWN; 
          fire=false;
          countOnce=true;
        }
        updateFrameBuffer();
      }
    }
    
    if ((birdPosX==bulletPosX) && (bulletPosY==EDGE_UP)&& countOnce) {
      scoreCounter++;
      Serial.print ("Score; ");
      Serial.println (scoreCounter);
      countOnce=false;
      birdPosX=COLOMNS;
      if (random(0,2))birdPosX=-1;
      updateFrameBuffer();
      birdWait = random(BIRDWAITMIN, BIRDWAITMAX);
      previousBirdWaitMillis = millis();
      playSound(PLOK);
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
  if(!digitalRead(BUTTON_U)) button = UP;
  if(!digitalRead(BUTTON_D)) button = DOWN;    
  return(button);
}

void doSomething(uint8_t button){
  //define your button actions here
  switch (button) {
    case LEFT:    
      Serial.print("Button; ");
      Serial.println("Left");
      gunPosX+=GO_LEFT;
      break;

    case RIGHT:    
      Serial.print("Button; ");
      Serial.println("Right");
      gunPosX+=GO_RIGHT;
      break;

    case DOWN:
      Serial.print("Button; ");    
      Serial.println("Down");
      //start playing
      play=true;
      scoreCounter=0;
      fireCounter=0;
//      birdWait = random(BIRDWAITMIN, BIRDWAITMAX);
      birdWait=1200;
      previousBirdWaitMillis = currentMillis;
      gunPosX=2;
      bulletPosY=EDGE_DOWN;
      fire=false;
      blinkDisplay=false;       
      break;
      
    case UP:    
      Serial.print("Button; ");
      Serial.println("Up");
      //start bullet
      fire = true;
      fireCounter++;
      bulletPosX=gunPosX;
      break;
      
  }
  updateFrameBuffer();
}

void updateFrameBuffer() {
//  GraphicDriver::emptyFrame();
    emptyFrame(); // clears the frameData buffer  
  //=====================================
  // >>define your game logic here<<
  
  // in this game:
  //1 - check borders
  if (gunPosX > EDGE_LEFT) gunPosX=EDGE_LEFT;
  if (gunPosX < EDGE_RIGHT) gunPosX=EDGE_RIGHT;

  //2 - update frameData
  frameData[ROWS-1] = 1 << gunPosX;
//  if (birdPosX <= EDGE_LEFT && birdPosX >=EDGE_RIGHT) frameData[0] = 1 << birdPosX;
  if (birdPosX < COLOMNS && birdPosX >=0) frameData[0] = 1 << birdPosX;
  if (fire) frameData[bulletPosY] = 1 << bulletPosX;
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
