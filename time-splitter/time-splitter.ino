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
byte arrowUp[] = {   // https://maxpromer.github.io/LCD-Character-Creator/
  B00000,
  B00100,
  B01110,
  B10101,
  B00100,
  B00100,
  B00100,
  B00000
};

bool yellowButtonState = true; // false means pressed
bool blueButtonState = true;
bool greenButtonState = true;
bool redButtonState = true;

bool counting = false;
short counterDelay = 20;
//char counterDelayChars[] = "100";
//short counterRange = 1000;
short digit = 0;
short randomRange = 6;
short randomRangeDigits[] = {6, 0, 0};
short randomNumber = 0;
char randomNumberChars[3] = "  1";

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
    Serial.println(digit);
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
