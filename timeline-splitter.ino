#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define GEIGER_COUNTER 2
#define YELLOW_BUTTON 3
#define BLUE_BUTTON 4
#define GREEN_BUTTON 5
#define RED_BUTTON 6
#define SIGNAL_LAMP 11
#define VOLTAGE A2

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
byte arrowDown[] = {
  B00000,
  B00100,
  B00100,
  B00100,
  B10101,
  B01110,
  B00100,
  B00000
};
byte arrowLeft[] = {
  B00000,
  B00100,
  B00010,
  B11111,
  B00010,
  B00100,
  B00000,
  B00000
};
byte arrowRight[] = {
  B00000,
  B00100,
  B01000,
  B11111,
  B01000,
  B00100,
  B00000,
  B00000
};
byte radioactiveUpperLeft[] = {
  B00000,
  B00000,
  B00010,
  B00110,
  B01111,
  B01111,
  B11111,
  B11111
};
byte radioactiveUpperCenter[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B01110
};
byte radioactiveUpperRight[] = {
  B00000,
  B00000,
  B01000,
  B01100,
  B11110,
  B11110,
  B11111,
  B11111
};
byte radioactiveLowerCenter[] = {
  B01110,
  B00000,
  B00100,
  B01110,
  B01110,
  B11111,
  B11111,
  B11111
};

bool yellowButtonState = true; // false means pressed
bool blueButtonState = true;
bool greenButtonState = true;
bool redButtonState = true;

short mode = 0;
short nModes = 3;
short digit = 0;
short randomRange = 6;
short randomRangeDigits[] = {6, 0, 0};
short randomNumber = 0;
char randomNumberChars[4] = "  1";

void interrupt() {
  Serial.println("interrupt");
  counting = false;
  digitalWrite(SIGNAL_LAMP, HIGH);
}

void setup() {
  Serial.begin(9600);
  Serial.println("starting");
  lcd.init();
  lcd.createChar(0, arrowUp);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Starting");
  pinMode(GEIGER_COUNTER, INPUT);
  pinMode(RED_BUTTON, INPUT_PULLUP);
  pinMode(GREEN_BUTTON, INPUT_PULLUP);
  pinMode(BLUE_BUTTON, INPUT_PULLUP);
  pinMode(YELLOW_BUTTON, INPUT_PULLUP);
  pinMode(SIGNAL_LAMP, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(GEIGER_COUNTER), interrupt, FALLING);
  delay(300);
  lcd.print(".");
  delay(300);
  lcd.print(".");
  delay(300);
  lcd.print(".");
  delay(300);
  lcd.setCursor(0, 0);
  lcd.print("Random(006)=???");
}

void loop() {

  // yellow button
  if(yellowButtonState && ! digitalRead(YELLOW_BUTTON)){
    Serial.println("yellow button");
    digitalWrite(SIGNAL_LAMP, LOW);
    counting = true;
    redButtonState = false;
    yellowButtonState = false;
    // TODO
  }
  else {
    yellowButtonState = digitalRead(YELLOW_BUTTON);
  }

  //blue button
  if(blueButtonState && ! digitalRead(BLUE_BUTTON)) {
    Serial.println("blue button");
    if(digit) {
      randomRangeDigits[digit - 1] = (randomRangeDigits[digit-1] + 1) % 10;
      randomRange = randomRangeDigits[0] + 10 * randomRangeDigits[1] + 100 * randomRangeDigits[2];
      lcd.setCursor(10 - digit, 0);
      lcd.print((char) (randomRangeDigits[digit - 1] + 48));
    }
    blueButtonState = false;
  }
  else {
    blueButtonState = digitalRead(BLUE_BUTTON);
  }

  // green button
  if(greenButtonState && ! digitalRead(GREEN_BUTTON)) {
    Serial.println("green button");
    digit = (digit + 1) % 4;
    if(digit) {
      lcd.setCursor(10 - digit, 1);
      lcd.write(0);
      lcd.print(" ");
    }
    else {
      lcd.setCursor(7, 1);
      lcd.print("   ");
    }
    greenButtonState = false;
  }
  else {
    greenButtonState = digitalRead(GREEN_BUTTON);
  }

  // red button
  if(redButtonState && ! digitalRead(RED_BUTTON)) {
    Serial.println("red button");
    digitalWrite(SIGNAL_LAMP, LOW);
    counting = true;
    redButtonState = false;
  }
  else {
    redButtonState = digitalRead(RED_BUTTON);
  }

  // TODO
  //Serial.println(analogRead(VOLTAGE));
  //adjustabe speed, if americanium

  // counting
  delay(counterDelay);
  if(counting) {
    randomNumber = randomNumber % randomRange + 1;
    sprintf(randomNumberChars, "%03d", randomNumber);
    lcd.setCursor(12, 0);
    lcd.print(randomNumberChars);
  }
}
