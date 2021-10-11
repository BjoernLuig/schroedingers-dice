#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


// pins
#define GEIGER_COUNTER 2
#define YELLOW_BUTTON 3
#define BLUE_BUTTON 4
#define GREEN_BUTTON 5
#define RED_BUTTON 6
#define SIGNAL_LAMP 11
#define VOLTAGE A2


// settings
#define COUNTING_INTERVAL 20000 // 10 Seconds
#define CONVERSION_INDEX 151 // counts per minute /(micro Sv/h)
#define BACKGROUND 25 // backround counts per minutes
#define VOLTAGE_SCALE 0.00955 // 0.00735
#define MICROS_FAKTOR 4 // resolution for arduino nano


// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
byte arrowUp[] = {   // see to generate the bytecode https://maxpromer.github.io/LCD-Character-Creator/
  B00000,
  B00100,
  B01110,
  B10101,
  B00100,
  B00100,
  B00100,
  B00000
};
byte radioactiveUpperLeft[] = {
  B00010,
  B00110,
  B01111,
  B01111,
  B11111,
  B11111,
  B00000,
  B00000
};
byte radioactiveUpperCenter[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00100,
  B01110,
  B00100
};
byte radioactiveUpperRight[] = {
  B01000,
  B01100,
  B11110,
  B11110,
  B11111,
  B11111,
  B00000,
  B00000
};
byte radioactiveLowerCenter[] = {
  B00100,
  B01110,
  B01110,
  B11111,
  B11111,
  B11111,
  B00100,
  B00000
};


// button states
bool yellowButtonState = true; // false means pressed
bool blueButtonState = true;
bool greenButtonState = true;
bool redButtonState = true;


// modes
int mode = 0;
int nModes = 3;


// mode 0: random number
bool interrupted = false;
bool waitForRandomNumber = false;
int digit = 0;
int randomRangeDigits[] = {6, 0, 0}; // reversed decimal
int randomRange = 6;
int randomNumber = 1;
char randomNumberChars[4] = "001";


// mode 1: geiger counter
int count = 0;
unsigned long lastUpdate = 0;
float countsPerMinute = 0.0;
char countsPerMinuteChars[7];
float sievert = 0.0;
char sievertChars[6];


// mode 2: battery voltage
float batteryVoltage = 0;
char batteryVoltageChars[10];


// will be called on interrupt by particle detection
// do not write to the LCD in this function !!! (loop will break)
void interrupt() {
  randomNumber = (micros() / MICROS_FAKTOR) % randomRange;
  Serial.println("interrupt");
  interrupted = true;
  count++;
  digitalWrite(SIGNAL_LAMP, HIGH);
}


void setup() {
  Serial.begin(9600);
  Serial.println("starting");
  pinMode(GEIGER_COUNTER, INPUT);
  pinMode(RED_BUTTON, INPUT_PULLUP);
  pinMode(GREEN_BUTTON, INPUT_PULLUP);
  pinMode(BLUE_BUTTON, INPUT_PULLUP);
  pinMode(YELLOW_BUTTON, INPUT_PULLUP);
  pinMode(SIGNAL_LAMP, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(GEIGER_COUNTER), interrupt, FALLING);
  lcd.init();
  lcd.createChar(0, arrowUp);
  lcd.createChar(1, radioactiveUpperLeft);
  lcd.createChar(2, radioactiveUpperCenter);
  lcd.createChar(3, radioactiveUpperRight);
  lcd.createChar(4, radioactiveLowerCenter);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.write(byte(1));
  lcd.write(byte(2));
  lcd.write(byte(3));
  lcd.print(" R(006)=  001");
  lcd.setCursor(0, 1);
  lcd.print(" ");
  lcd.write(byte(4));
  lcd.print("              ");
}


void loop() {


  // yellow button
  if(yellowButtonState && ! digitalRead(YELLOW_BUTTON)){
    Serial.println("yellow button");
    mode = (mode + 1) % nModes;
    Serial.println(mode);
    if(mode == 0) {
      sprintf(randomNumberChars, "%03d", randomNumber);
      lcd.setCursor(4,0);
      lcd.print("R(");
      lcd.print((char) (randomRangeDigits[2] + 48));
      lcd.print((char) (randomRangeDigits[1] + 48));
      lcd.print((char) (randomRangeDigits[0] + 48));
      lcd.print(")=  ");
      lcd.print(randomNumberChars);
      lcd.setCursor(4,1);
      lcd.print("            ");
    } else if(mode == 1) {
      waitForRandomNumber = false;
      digit = 0;
      dtostrf(countsPerMinute, 6, 2, countsPerMinuteChars);
      dtostrf(sievert, 5, 1, sievertChars);
      lcd.setCursor(4,0);
      lcd.print("A=");
      lcd.print(countsPerMinuteChars);
      lcd.print("/min");
      lcd.setCursor(4,1);
      lcd.print("E=");
      lcd.print(sievertChars);
      lcd.print("\xE4Sv/h");
    } else if(mode == 2) {
      waitForRandomNumber = false;
      digit = 0;
      dtostrf(batteryVoltage, 9, 2, batteryVoltageChars);
      lcd.setCursor(4,0);
      lcd.print("U=");
      lcd.print(batteryVoltageChars);
      lcd.print("V");
      lcd.setCursor(4,1);
      lcd.print("            ");
     }
  }
  yellowButtonState = digitalRead(YELLOW_BUTTON);


  //blue button
  if(blueButtonState && ! digitalRead(BLUE_BUTTON)) {
    Serial.print("blue button");
    if(digit) {
      randomRangeDigits[digit - 1] = (randomRangeDigits[digit-1] + 1) % 10;
      randomRange = randomRangeDigits[0] + 10 * randomRangeDigits[1] + 100 * randomRangeDigits[2];
      lcd.setCursor(9 - digit, 0);
      lcd.print((char) (randomRangeDigits[digit - 1] + 48));
    }
    Serial.println((char) (randomRangeDigits[digit - 1] + 48));
  }
  blueButtonState = digitalRead(BLUE_BUTTON);


  // green button
  if(greenButtonState && ! digitalRead(GREEN_BUTTON)) {
    Serial.print("green button");
    if(mode == 0) {
      digit = (digit + 1) % 4;
    }
    if(digit) {
      lcd.setCursor(9 - digit, 1);
      lcd.write(0);
      lcd.print(" ");
    }
    else {
      lcd.setCursor(6, 1);
      lcd.print("   ");
    }
    Serial.println(digit);
  }
  greenButtonState = digitalRead(GREEN_BUTTON);


  // red button
  if(redButtonState && ! digitalRead(RED_BUTTON)) {
    Serial.println("red button");
    digitalWrite(SIGNAL_LAMP, LOW);
    if(mode == 0) {
      lcd.setCursor(13, 0);
      lcd.print(" ? ");
    }

    waitForRandomNumber = true;
  }
  redButtonState = digitalRead(RED_BUTTON);


  // update lcd
  if(mode == 0 && interrupted && waitForRandomNumber) { // update mode 1: dice
    sprintf(randomNumberChars, "%03d", randomNumber);
    lcd.setCursor(13, 0);
    lcd.print(randomNumberChars);
    interrupted = false;
    waitForRandomNumber = false;
  }else if(mode == 1 && ((millis() - lastUpdate) > COUNTING_INTERVAL)) { // update mode 1: geiger counter
    countsPerMinute = count * (60000.0 / COUNTING_INTERVAL) - BACKGROUND;
    sievert = countsPerMinute/CONVERSION_INDEX;
    dtostrf(countsPerMinute, 6, 2, countsPerMinuteChars);
    dtostrf(sievert, 5, 2, sievertChars);
    lcd.setCursor(6, 0);
    lcd.print(countsPerMinuteChars);
    lcd.setCursor(6, 1);
    lcd.print(sievertChars);
    lcd.print("\xE4Sv/h");
    lastUpdate = millis();
    count = 0;
  } else if(mode == 2) { // update mode 2: battery voltage
    batteryVoltage = analogRead(VOLTAGE) * VOLTAGE_SCALE;
    dtostrf(batteryVoltage, 9, 2, batteryVoltageChars);
    lcd.setCursor(6, 0);
    lcd.print(batteryVoltageChars);
  }


  // delay
  delay(100);
}
