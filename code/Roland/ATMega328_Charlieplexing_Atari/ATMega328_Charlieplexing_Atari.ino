/*
  Schematic; Snakegame.pro | v3.1
  Board; Arduino Pro or Pro Mini
  Charlieplexing Atari solo pong with ability to win 

  Ledschema;
  led1  led2  led3  led4  led5
  led6  led7  led8  led9  led10
  led11 led12 led13 led14 led15
  led16 led17 led18 led19 led20
  led21 led22 led23 led24 led25
  led26 led27 led28 led29 led30

  Pong - use buttons to control
*/
#include <Bounce2.h>

// Buttons & buzzer pin defines
#define BUTTON_L 8 // Button Left pin - Arduino pin D8
#define BUTTON_D 7 // Button Down pin - Arduino pin D7
#define BUTTON_U 6 // Button Up pin - Arduino pin D6
#define BUTTON_R 5 // Button Right pin - Arduino pin D5
#define BUZZER 9 // Piezospeaker - Arduino pin D9

// Charlyplexing pin defines
#define CHP_A A0 //Charlieplexing pin A - Arduino pin A0
#define CHP_B A1 //Charlieplexing pin B - Arduino pin A1
#define CHP_C A2 //Charlieplexing pin C - Arduino pin A2
#define CHP_D A3 //Charlieplexing pin D - Arduino pin A3
#define CHP_E A4 //Charlieplexing pin E - Arduino pin A4
#define CHP_F A5 //Charlieplexing pin E - Arduino pin A5

#define COLOMNS 5 // number of LED colomns
#define ROWS 6    // number of LED rows
#define AMOUNT_OF_LEDS COLOMNS*ROWS

//define directions
#define LEFT 1
#define DOWN 2
#define UP 3
#define RIGHT 4

#define GO_LEFT 1
#define GO_RIGHT -1
#define GO_UP -1
#define GO_DOWN 1
#define EDGE_LEFT COLOMNS-1
#define EDGE_RIGHT 0
#define EDGE_UP 0
#define EDGE_DOWN ROWS-1

#define B_PRESSED 0 // Button pressed active low
#define DEBOUNCETIME 10 // debouncetime 10ms

// Instantiate the Bounce objects for the buttons
Bounce debouncer_L = Bounce(); // Debounce object Button Left
Bounce debouncer_D = Bounce(); // Debounce object Button Down 
Bounce debouncer_U = Bounce(); // Debounce object Button Up 
Bounce debouncer_R = Bounce(); // Debounce object Button Right 

//Charlieplexing matrix
byte ledmatrix[AMOUNT_OF_LEDS][2] = {
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

byte frameData[ROWS]; //Led image buffer

//int ballPosX = EDGE_RIGHT; // ball X position
//int ballPosY = EDGE_UP ; // ball Y position
int ballPosX = -1; // ball X position
int ballPosY = -1 ; // ball Y position
int ballDirX = GO_LEFT;
int ballDirY = GO_UP;
int prefballDirX=0;

bool blinkDisplay=false;

int racketPosX = 1;
#define NILL COLOMNS+1

byte target [3][COLOMNS] = {
  {0,1,2,3,4},
  {NILL,1,NILL,3,NILL},  
  {0,1,2,3,4},
  };

/*
byte target [3][COLOMNS] = {
  {0,1,2,3,4},
  {NILL,NILL,NILL,NILL,NILL},
  {NILL,NILL,NILL,NILL,NILL},
  };
*/
//int racketWidth = 2; // width of racket is 2 leds

// store last times
unsigned long previousBallMillis = 0; 
unsigned long previousRacketMillis = 0;
unsigned long previousBlinkMillis = 0;  

#define BALLSPEED 150    // speed of ball (mS) | the lower the value the faster the speed
//#define BALLSPEED 250    // speed of ball (mS) | the lower the value the faster the speed
#define RACKETSPEED 150    // speed of racket (mS)
#define BLINKTIME 100    // Display blinktime (mS)
#define MAXBLINK 10
#define WELCOME 1 // welcome sound
#define YOULOST 2 // you lost sound
#define YOUWON 3 // you lost sound
#define PLOK 4 // PLOK sound
#define PLIK 5 // PLIK sound

bool hit_right= false;
bool hit_left = false;
bool you_lost = false;
bool you_won = false;
bool play = false; //play semaphore 
bool displayOff = false;
uint8_t blinkcount = 0;
bool play_onetime=true;

void setup() {

  // initialize serial communication:
  Serial.begin(19200);
  Serial.println("Charlieplexing pong");
  Serial.println("");     
  Serial.print("press UP to play new ball");   
  Serial.println("");  
  
  //tatatadaa welcome
  playSound(WELCOME);   
  // Setup the the buttons with an internal pull-up and debounce:
  pinMode(BUTTON_L,INPUT_PULLUP);
  debouncer_L.attach(BUTTON_L);
  debouncer_L.interval(DEBOUNCETIME); // interval in ms
  
  // Setup the the buttons with an internal pull-up and debounce:
  pinMode(BUTTON_D,INPUT_PULLUP);
  debouncer_D.attach(BUTTON_D);
  debouncer_D.interval(DEBOUNCETIME); // interval in ms

    // Setup the the buttons with an internal pull-up and debounce:
  pinMode(BUTTON_U,INPUT_PULLUP);
  debouncer_U.attach(BUTTON_U);
  debouncer_U.interval(DEBOUNCETIME); // interval in ms

    // Setup the the buttons with an internal pull-up and debounce:
  pinMode(BUTTON_R,INPUT_PULLUP);
  debouncer_R.attach(BUTTON_R);
  debouncer_R.interval(DEBOUNCETIME); // interval in ms
  updateFrameBuffer(); //initial led on
  drawFrame();
}

void loop() {
  uint8_t r;
  unsigned long currentMillis = millis();
  uint8_t button = readButtons();
  if (button) {
    doSomething(button);
    if (play) updateFrameBuffer();    
  }
  
  if (you_lost) {
    play = false;
    blinkDisplay=true;
    if (blinkcount>=MAXBLINK) {
      blinkDisplay=false;
      blinkcount=0;
      you_lost=false;
      ballPosX = NILL; // hide ball
      updateFrameBuffer();      
      playSound(YOULOST);
      initTarget();
    }
  }

  if (you_won) {
    play = false;
    blinkDisplay=true;
    if (blinkcount>=MAXBLINK) {
      blinkDisplay=false;
      blinkcount=0;
      you_won=false;
//      ballPosX = NILL; // hide ball
//      updateFrameBuffer();      
      playSound(YOUWON);
      initTarget();
    }
  }

  
  if (play) {  
    if (currentMillis - previousBallMillis >= BALLSPEED) {
      previousBallMillis = currentMillis;
    
      if (hit_left) {
         play_onetime=true;
 //       playSound (PLIK);
        if (ballDirX == GO_LEFT) {
          ballDirY = GO_UP;
        }
        if (ballDirX == GO_RIGHT || !ballDirX) {
          ballDirY = GO_UP;
          if (prefballDirX==GO_LEFT) ballDirX=0;  // 
          else ballDirX=GO_LEFT;
          prefballDirX=ballDirX;
        }
        hit_left = false;        
      }
      if (hit_right) {
        play_onetime=true;
//        playSound (PLIK);
        if (ballDirX == GO_RIGHT) {
          ballDirY = GO_UP;
         
        }
        if (ballDirX == GO_LEFT || !ballDirX) {
          ballDirY = GO_UP;
          if (prefballDirX==GO_RIGHT) ballDirX=0;  // 
          else ballDirX=GO_RIGHT;
          prefballDirX=ballDirX;
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
//      if (racketPosX >= EDGE_LEFT) racketPosX=EDGE_LEFT;
//      if (racketPosX <= EDGE_RIGHT-1) racketPosX=EDGE_RIGHT-1;
//      Serial.println(racketPosX);       
      updateFrameBuffer();  
    }      
  }
  
  if (blinkDisplay){
      if (currentMillis - previousBlinkMillis >= BLINKTIME) {
      previousBlinkMillis = currentMillis;
        displayOff = !displayOff;
        blinkcount++;
      }
      if (!displayOff) drawFrame();
  }
  else drawFrame();
}

uint8_t readButtons(){
  // Update the Bounce instances
  debouncer_L.update();
  debouncer_D.update();
  debouncer_U.update();
  debouncer_R.update();

  // Get the updated button states :
  int button_L = debouncer_L.read();
  int button_D = debouncer_D.read();
  int button_U = debouncer_U.read();
  int button_R = debouncer_R.read();

  uint8_t button=0;
//  if ( debouncer_R.rose()) button=RIGHT
//  if ( debouncer_L.rose()) button=LEFT;
  if ( debouncer_U.rose()) button=UP;
  if ( debouncer_D.rose()) button=DOWN;

  if(!digitalRead(BUTTON_L)) button = LEFT;
  else if(!digitalRead(BUTTON_R)) button = RIGHT;
  
  return(button);
}

void doSomething(uint8_t button){
  bool sema_once = false;
  switch (button) {
/* 
    case LEFT:    
//      Serial.print("Button; ");
//      Serial.println("Left");
      racketPosX+=GO_LEFT;
      sema_once=true; 
      break;    
    case RIGHT:    
//      Serial.print("Button; ");
//      Serial.println("Right");
      racketPosX+=GO_RIGHT;
      sema_once=true; 
      break;
*/
    case DOWN:
//      Serial.print("Button; ");    
//      Serial.println("Down");
        //action
      sema_once=true;    
      break;
    case UP:    
//      Serial.print("Button; ");
//      Serial.println("Up");
      //start bullet
      if (!play){
        play=true;
        you_lost=false;
        you_won=false;
        ballPosX = random (racketPosX, racketPosX+2); // ball X position
        ballPosY = EDGE_DOWN ; // ball Y position
        sema_once=true; 
         
      }
      
      break;
      
//     default:
//     Serial.println("no button pressed");     
  }
  //check colomn borders
//  if (racketPosX >= EDGE_LEFT-1) racketPosX=EDGE_LEFT-1;
//  if (racketPosX <= EDGE_RIGHT) racketPosX=EDGE_RIGHT;

/*
  if (sema_once){
    Serial.print ("racketPosX: ");
    Serial.println(racketPosX);
    Serial.println();
    sema_once=false;     
  } 
*/
}

void updateFrameBuffer() {
  byte tempData=0;
  for(uint8_t i = 0; i < ROWS; i++) {
    frameData[i] = 0; //reset all leds
/*
    Serial.print(counter);
    Serial.print(" ");
    Serial.print(i);
    Serial.print(" ");
    Serial.println(frameData[i],BIN);
*/
  }

      
//  if (racketPosX==-1) frameData[ROWS-1] = 1;
//  else frameData[ROWS-1] = 3 << racketPosX;
   frameData[ROWS-1] = 3 << racketPosX;
  if (ballPosY == ROWS-1){
    tempData = 1 << ballPosX;
    frameData[ROWS-1] = frameData[ROWS-1] | tempData; //if ball and racket has the same row, combine their positions
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
  bool hitTarget=false;
  you_won=true;
  for (uint8_t y=0; y<3; y++) {
    for (uint8_t x=0; x<COLOMNS; x++) {
      tempData = 1<< target[y][x];
      frameData[y] = frameData[y] | tempData;
    
      if (ballPosY == y){
        tempData = 1 << ballPosX;
        frameData[y] = frameData[y] | tempData; //if ball and targets has the same row, combine their positions
        if ((ballPosX == x) && (target[y][x]!=NILL)) {
          target[y][x]=NILL;
          hitTarget=true;
        }
      }
      if (target[y][x] != NILL) you_won=false;     
    }
  }
/*  
//For complex ball movements
  if (hitTarget){
    ballDirY=-1*ballDirY;
//    ballDirX=-1*ballDirX;
    playSound (PLOK); 
  }
*/
  

//  Serial.println("frameData updated");
}

void drawFrame() {
  for(uint8_t y = 0; y < ROWS; y++) {
    uint8_t rowData = frameData[y];
    for(uint8_t x = 0; x < COLOMNS ; x++) {
      //calculate led_matrixaddr from colomn & row nr
      uint8_t ledNumber=(y*COLOMNS+(COLOMNS-x))-1; //calculate led_matrixaddr from colomn & row nr
/*
      Serial.print(y);
      Serial.print(" ");
      Serial.print(x);
      Serial.print(" ");
      Serial.print(ledNumber);
      Serial.print(" ");
      Serial.println((rowData >> x) & 1, BIN);
*/
      // check on
      if((rowData >> x) & 1) {
        DDRC = ledmatrix[ledNumber][0];
        PORTC = ledmatrix[ledNumber][1];
        delayMicroseconds(500); // to shine leds brighter
      } else {
          DDRC = 0;
          PORTC = 0;
          }
      }
    }
    DDRC = 0;
    PORTC = 0;
}

void playSound (uint8_t sound){
  switch (sound) {
    case  WELCOME:
    //tatatadaa welcome
    tone(BUZZER, 250, 80); //pin, freq, duration
    delay (150);
    tone(BUZZER, 250, 80); 
    delay (150);
    tone(BUZZER, 250, 80); 
    delay (150);
    tone(BUZZER, 200, 500);
    break;
  
  case YOULOST:
    tone(BUZZER, 200, 80); //pin, freq, duration
    delay (300);
    tone(BUZZER, 190, 80); 
    delay (300);
    tone(BUZZER, 180, 80); 
    delay (300);
    tone(BUZZER, 170, 750);
    delay (1000);
    break;

  case YOUWON:
    //tatatadaa 
    tone(BUZZER, 250, 80); //pin, freq, duration
    delay (150);
    tone(BUZZER, 250, 80); 
    delay (150);
    tone(BUZZER, 250, 80); 
    delay (150);
    tone(BUZZER, 200, 500);
    break;
    
  case PLOK:
    tone(BUZZER, 600, 1); //pin, freq, duration    
    break;
    
  case PLIK:
    tone(BUZZER, 1500, 1); //pin, freq, duration
    play_onetime=false;
    break;
  }
}

void initTarget (){
  for (uint8_t y=0; y<3; y++) {
    for (uint8_t x=0; x<COLOMNS; x++) {
      target[y][x] = x;
    }     
  }
  target[1][0]=NILL; these leds can't be hit
  target[1][2]=NILL;
  target[1][4]=NILL;  
}
