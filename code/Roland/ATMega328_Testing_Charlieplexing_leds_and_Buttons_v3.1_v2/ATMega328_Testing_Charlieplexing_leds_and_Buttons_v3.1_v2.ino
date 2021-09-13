/*
  Schematic; Snakegame.pro | v3.1
  Board; Arduino Pro or Pro Mini
  Demo for;
  - Charlieplexing with 30 LEDs
  - 4 buttons with debouncing
  - Piezobuzzer

  Ledschema;
  led1  led2  led3  led4  led5
  led6  led7  led8  led9  led10
  led11 led12 led13 led14 led15
  led16 led17 led18 led19 led20
  led21 led22 led23 led24 led25
  led26 led27 led28 led29 led30

  Demo sketch - use buttons to scroll
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

#define COLOMNS 5      // number of LED colomns
#define ROWS 6         // number of LED rows
#define AMOUNT_OF_LEDS COLOMNS*ROWS

//define directions
#define LEFT 1
#define DOWN 2
#define UP 3
#define RIGHT 4

#define B_PRESSED 0 // Button pressed active low
#define DEBOUNCETIME 10 // debouncetime 10ms

// Instantiate the Bounce objects for the buttons
Bounce debouncer_L = Bounce(); // Debounce object Button Left
Bounce debouncer_D = Bounce(); // Debounce object Button Down 
Bounce debouncer_U = Bounce(); // Debounce object Button Up 
Bounce debouncer_R = Bounce(); // Debounce object Button Right 

//Charlieplexing matrix
uint8_t ledmatrix[AMOUNT_OF_LEDS][2] = {
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

uint8_t frameData[ROWS]; //Led image buffer
int8_t positionX = 0; //positionX pointer in frameData
int8_t positionY = 0; //positionY pointer in frameData

void setup() {

  // initialize serial communication:
  Serial.begin(19200);
  Serial.println("Charlieplexing Demo sketch");
  Serial.println("");     
  Serial.println("press a button .....");   
  Serial.println("");  
  
  //tatatadaa welcome
  tone(BUZZER, 250, 80); //pin, freq, duration
  delay (150);
  tone(BUZZER, 250, 80); 
  delay (150);
  tone(BUZZER, 250, 80); 
  delay (150);
  tone(BUZZER, 200, 500); 
      
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

  uint8_t button = readButtons();
  if (button) {
    doSomething(button);
    updateFrameBuffer();
    }
  drawFrame();

}

void doSomething(uint8_t button){
  bool sema_once = false;
  switch (button) {
    case LEFT:    
      Serial.print("Button; ");
      Serial.println("Left");
      positionX++;
      sema_once=true; //set semahore for debugging purpose
      break;
    case DOWN:
      Serial.print("Button; ");    
      Serial.println("Down");
      positionY++;
      sema_once=true;       
      break;
    case UP:    
      Serial.print("Button; ");
      Serial.println("Up");
      positionY--;
      sema_once=true;        
      break;
    case RIGHT:    
      Serial.print("Button; ");
      Serial.println("Right");
      positionX--;
      sema_once=true; 
      break;
//     default:
//     Serial.println("no button pressed");     
  }
  //check row and colomn borders
  if (positionX>=COLOMNS) positionX=0;
  if (positionX<0) positionX=COLOMNS-1;
  if (positionY>=ROWS) positionY=0;
  if (positionY<0) positionY=ROWS-1;
  
  if (sema_once){
    Serial.print ("Colomn: ");
    Serial.println(positionX);
    Serial.print ("Row: ");
    Serial.println(positionY);    
    Serial.print ("Lednr: ");
    Serial.println((positionY*COLOMNS)+(COLOMNS-positionX));
    Serial.println();
    sema_once=false;     
  } 
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
  if ( debouncer_R.rose()) button=RIGHT;
  if ( debouncer_U.rose()) button=UP;
  if ( debouncer_D.rose()) button=DOWN;
  if ( debouncer_L.rose()) button=LEFT;
//  if (button) Serial.println ("Button pressed; ");
  return(button);
}

void updateFrameBuffer() {
  for(uint8_t i = 0; i < ROWS; i++) {
    frameData[i] = 0; //reset all leds
  }
  frameData[positionY] = 1 << positionX;
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
