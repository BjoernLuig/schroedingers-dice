/*
**************ARDUINO SKETCH FOR GEIGER COUNTER******************
Author: RH Electronics www.radiohobbystore.com / www.mygeiger.org
*****************************************************************
This sketch can be used with DIY Geiger Counter board. It allow
to receive CPM data during minute. You can modify the sketch
for your needs.
*****************************************************************
* LCD Connection:
* LCD RS pin to digital pin 9
* LCD Enable pin to digital pin 8
* LCD D4 pin to digital pin 7
* LCD D5 pin to digital pin 6
* LCD D6 pin to digital pin 5
* LCD D7 pin to digital pin 3
* LCD R/W pin to ground
* 10K resistor:
* ends to +5V and ground
* wiper to LCD VO pin (pin 3)
****************************************************************/
// include the library code:
#include <LiquidCrystal.h>
#include <SPI.h>
#define LOG_PERIOD 15000  //Logging period in milliseconds, recommended value 15000-60000.
#define MAX_PERIOD 60000  //Maximum logging period without modifying this sketch
#define PERIOD 60000.0 // (60 sec) one minute measure period
volatile unsigned long CNT; // variable for counting interrupts from dosimeter
unsigned long counts;     //variable for GM Tube events
unsigned long cpm;        //variable for CPM
unsigned int multiplier;  //variable for calculation CPM in this sketch
unsigned long previousMillis;  //variable for time measurement
unsigned long dispPeriod; // variable for measuring time
unsigned long CPM; // variable for measuring CPM
// initialize the library with the numbers of the interface pins

LiquidCrystal lcd(9, 8, 7, 6, 5, 3);
void setup() { // setup
lcd.begin(16, 2);
CNT = 0;
CPM = 0;
dispPeriod = 0;
lcd.setCursor(0,0);
lcd.print(" RH Electronics ");
lcd.setCursor(0,1);
lcd.print(" Geiger Counter ");
delay(2000);
cleanDisplay();
attachInterrupt(0,GetEvent,FALLING); // Event on pin 2
}
void loop() {
lcd.setCursor(0,0); // print text and CNT on the LCD
lcd.print("CPM:");
lcd.setCursor(0,1);
lcd.print("CNT:");
lcd.setCursor(5,1);
lcd.print(CNT);
if (millis() >=dispPeriod + PERIOD) { // If one minute is over
cleanDisplay(); // Clear LCD
// Do something about accumulated CNT events....
lcd.setCursor(5, 0);
CPM = CNT;
lcd.print(CPM); //Display CPM
CNT = 0;
dispPeriod = millis();
}
}
void GetEvent(){ // Get Event from Device
CNT++;
}
void cleanDisplay (){ // Clear LCD routine
lcd.clear();
lcd.setCursor(0,0);
lcd.setCursor(0,0);
}
