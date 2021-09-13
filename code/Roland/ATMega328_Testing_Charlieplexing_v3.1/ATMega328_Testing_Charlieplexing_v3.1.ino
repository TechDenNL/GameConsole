/*
  Test Charlieplexing with 30 LEDs

=====================================================================================================
  Arduino-IDE settings;
  Board; Arduino Pro or Pro mini  
  Processor; ATMEGA328P (5V, 16MHz)
  
*/

// Charlyplexing pin defines
#define CHP_A A0 //Charleyplexing pin A - Arduino pin A0
#define CHP_B A1 //Charleyplexing pin B - Arduino pin A1
#define CHP_C A2 //Charleyplexing pin C - Arduino pin A2
#define CHP_D A3 //Charleyplexing pin D - Arduino pin A3
#define CHP_E A4 //Charleyplexing pin E - Arduino pin A4
#define CHP_F A5 //Charleyplexing pin E - Arduino pin A5 

#define COLOMNS 5      // number of LED colomns
#define ROWS 6         // number of LED rows
//#define LED_COUNT 30  // number of LED rows

#define PIN_CONFIG 0
#define PIN_STATE 1


/*
 * If you want to save bytes in SRAM, this could be a collection of bytes for each DDRx and PORTx
 * on the AVR chip (up to 8 bits or lines) (8 lines would allow for 56 LEDs)
 */
int matrix[COLOMNS*ROWS][2][ROWS] = {
   /*
    *  Each row in this matrix respresents the pin modes and pin states for a single LED
    */
   //              PIN_CONFIG                          PIN_STATE
   //CHP_A   CHP_B   CHP_C   CHP_D   CHP_E      CHP_A  CHP_B CHP_C CHP_D CHP_E
{{  OUTPUT, OUTPUT,  INPUT,  INPUT,  INPUT,  INPUT, },{ HIGH,  LOW,  LOW,  LOW,  LOW,  LOW, }}, //  01  CHP_A CHP_B
{{  OUTPUT,  INPUT, OUTPUT,  INPUT,  INPUT,  INPUT, },{ HIGH,  LOW,  LOW,  LOW,  LOW,  LOW, }}, //  02  CHP_A CHP_C
{{  OUTPUT,  INPUT,  INPUT, OUTPUT,  INPUT,  INPUT, },{ HIGH,  LOW,  LOW,  LOW,  LOW,  LOW, }}, //  03  CHP_A CHP_D
{{  OUTPUT,  INPUT,  INPUT,  INPUT, OUTPUT,  INPUT, },{ HIGH,  LOW,  LOW,  LOW,  LOW,  LOW, }}, //  04  CHP_A CHP_E
{{  OUTPUT,  INPUT,  INPUT,  INPUT,  INPUT, OUTPUT, },{ HIGH,  LOW,  LOW,  LOW,  LOW,  LOW, }}, //  05  CHP_A CHP_F
{{  OUTPUT, OUTPUT,  INPUT,  INPUT,  INPUT,  INPUT, },{  LOW, HIGH,  LOW,  LOW,  LOW,  LOW, }}, //  06  CHP_B CHP_A
{{   INPUT, OUTPUT, OUTPUT,  INPUT,  INPUT,  INPUT, },{  LOW, HIGH,  LOW,  LOW,  LOW,  LOW, }}, //  07  CHP_B CHP_C
{{   INPUT, OUTPUT,  INPUT, OUTPUT,  INPUT,  INPUT, },{  LOW, HIGH,  LOW,  LOW,  LOW,  LOW, }}, //  08  CHP_B CHP_D
{{   INPUT, OUTPUT,  INPUT,  INPUT, OUTPUT,  INPUT, },{  LOW, HIGH,  LOW,  LOW,  LOW,  LOW, }}, //  09  CHP_B CHP_E
{{   INPUT, OUTPUT,  INPUT,  INPUT,  INPUT, OUTPUT, },{  LOW, HIGH,  LOW,  LOW,  LOW,  LOW, }}, //  10  CHP_B CHP_F
{{  OUTPUT,  INPUT, OUTPUT,  INPUT,  INPUT,  INPUT, },{  LOW,  LOW, HIGH,  LOW,  LOW,  LOW, }}, //  11  CHP_C CHP_A
{{   INPUT, OUTPUT, OUTPUT,  INPUT,  INPUT,  INPUT, },{  LOW,  LOW, HIGH,  LOW,  LOW,  LOW, }}, //  12  CHP_C CHP_B
{{   INPUT,  INPUT, OUTPUT, OUTPUT,  INPUT,  INPUT, },{  LOW,  LOW, HIGH,  LOW,  LOW,  LOW, }}, //  13  CHP_C CHP_D
{{   INPUT,  INPUT, OUTPUT,  INPUT, OUTPUT,  INPUT, },{  LOW,  LOW, HIGH,  LOW,  LOW,  LOW, }}, //  14  CHP_C CHP_E
{{   INPUT,  INPUT, OUTPUT,  INPUT,  INPUT, OUTPUT, },{  LOW,  LOW, HIGH,  LOW,  LOW,  LOW, }}, //  15  CHP_C CHP_F
{{  OUTPUT,  INPUT,  INPUT, OUTPUT,  INPUT,  INPUT, },{  LOW,  LOW,  LOW, HIGH,  LOW,  LOW, }}, //  16  CHP_D CHP_A
{{   INPUT, OUTPUT,  INPUT, OUTPUT,  INPUT,  INPUT, },{  LOW,  LOW,  LOW, HIGH,  LOW,  LOW, }}, //  17  CHP_D CHP_B
{{   INPUT,  INPUT, OUTPUT, OUTPUT,  INPUT,  INPUT, },{  LOW,  LOW,  LOW, HIGH,  LOW,  LOW, }}, //  18  CHP_D CHP_C
{{   INPUT,  INPUT,  INPUT, OUTPUT, OUTPUT,  INPUT, },{  LOW,  LOW,  LOW, HIGH,  LOW,  LOW, }}, //  19  CHP_D CHP_E
{{   INPUT,  INPUT,  INPUT, OUTPUT,  INPUT, OUTPUT, },{  LOW,  LOW,  LOW, HIGH,  LOW,  LOW, }}, //  20  CHP_D CHP_F
{{  OUTPUT,  INPUT,  INPUT,  INPUT, OUTPUT,  INPUT, },{  LOW,  LOW,  LOW,  LOW, HIGH,  LOW, }}, //  21  CHP_E CHP_A
{{   INPUT, OUTPUT,  INPUT,  INPUT, OUTPUT,  INPUT, },{  LOW,  LOW,  LOW,  LOW, HIGH,  LOW, }}, //  22  CHP_E CHP_B
{{   INPUT,  INPUT, OUTPUT,  INPUT, OUTPUT,  INPUT, },{  LOW,  LOW,  LOW,  LOW, HIGH,  LOW, }}, //  23  CHP_E CHP_C
{{   INPUT,  INPUT,  INPUT, OUTPUT, OUTPUT,  INPUT, },{  LOW,  LOW,  LOW,  LOW, HIGH,  LOW, }}, //  24  CHP_E CHP_D
{{   INPUT,  INPUT,  INPUT,  INPUT, OUTPUT, OUTPUT, },{  LOW,  LOW,  LOW,  LOW, HIGH,  LOW, }}, //  25  CHP_E CHP_F
{{  OUTPUT,  INPUT,  INPUT,  INPUT,  INPUT, OUTPUT, },{  LOW,  LOW,  LOW,  LOW,  LOW, HIGH, }}, //  26  CHP_F CHP_A
{{   INPUT, OUTPUT,  INPUT,  INPUT,  INPUT, OUTPUT, },{  LOW,  LOW,  LOW,  LOW,  LOW, HIGH, }}, //  27  CHP_F CHP_B
{{   INPUT,  INPUT, OUTPUT,  INPUT,  INPUT, OUTPUT, },{  LOW,  LOW,  LOW,  LOW,  LOW, HIGH, }}, //  28  CHP_F CHP_C
{{   INPUT,  INPUT,  INPUT, OUTPUT,  INPUT, OUTPUT, },{  LOW,  LOW,  LOW,  LOW,  LOW, HIGH, }}, //  29  CHP_F CHP_D
{{   INPUT,  INPUT,  INPUT,  INPUT, OUTPUT, OUTPUT, },{  LOW,  LOW,  LOW,  LOW,  LOW, HIGH }}  //  30  CHP_F CHP_E

};

//LED control function (for one LED)
void turnOn( int led ) {
    // set all the pin modes
    pinMode( CHP_A, matrix[led][PIN_CONFIG][0] );
    pinMode( CHP_B, matrix[led][PIN_CONFIG][1] );
    pinMode( CHP_C, matrix[led][PIN_CONFIG][2] );
    pinMode( CHP_D, matrix[led][PIN_CONFIG][3] );
    pinMode( CHP_E, matrix[led][PIN_CONFIG][4] );
    pinMode( CHP_F, matrix[led][PIN_CONFIG][5] );
    // set all the pin states
    digitalWrite( CHP_A, matrix[led][PIN_STATE][0] );
    digitalWrite( CHP_B, matrix[led][PIN_STATE][1] );
    digitalWrite( CHP_C, matrix[led][PIN_STATE][2] );
    digitalWrite( CHP_D, matrix[led][PIN_STATE][3] );
    digitalWrite( CHP_E, matrix[led][PIN_STATE][4] );
    digitalWrite( CHP_F, matrix[led][PIN_STATE][5] );
}

void setup() {}

void loop() {
    for( int i = 0; i < COLOMNS*ROWS; i++ ) {
        turnOn( i );
        delay( 100  );
    }
    for( int i = 0; i < COLOMNS; i++ ) {
      for (int j = 0; j< ROWS; j++ ) {
      turnOn( i + j*COLOMNS );
      delay( 100  );
      }    
    }
}
