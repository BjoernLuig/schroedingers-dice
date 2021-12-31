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
#define COUNTING_INTERVAL 30000 // counting every 30 seconds
#define CONVERSION_INDEX 151    // (counts per minute)/(micro Sv/h)
#define BACKGROUND 25           // backround counts per minutes
#define VOLTAGE_SCALE 0.00851   // (battery voltage)/(sensor voltage) (might not be very accurate)
#define MICROS_FAKTOR 4         // micros() counts in steps of 4 on an arduino nano

// LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2);
byte radioactiveUpperLeft[] = { // see https://maxpromer.github.io/LCD-Character-Creator/ to generate the bytecode
    B00000,
    B00100,
    B01110,
    B01110,
    B11111,
    B11111,
    B00000,
    B00000};
byte radioactiveUpperCenter[] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00100,
    B01110,
    B00100};
byte radioactiveUpperRight[] = {
    B00000,
    B00100,
    B01110,
    B01110,
    B11111,
    B11111,
    B00000,
    B00000};
byte radioactiveLowerCenter[] = {
    B00100,
    B01110,
    B01110,
    B11111,
    B11111,
    B00100,
    B00000,
    B00000};
byte batteryLeft[] = {
    B00011,
    B00010,
    B00010,
    B00010,
    B00010,
    B00010,
    B00010,
    B00011};
byte batteryCenterFull[] = {
    B11111,
    B00000,
    B11110,
    B11110,
    B11110,
    B11110,
    B00000,
    B11111};
byte batteryCenterEmpty[] = {
    B11111,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B11111};
byte batteryRight[] = {
    B10000,
    B10000,
    B11100,
    B11100,
    B11100,
    B11100,
    B10000,
    B10000};

// button states
bool yellowButtonState = true; // true means not pressed, false means pressed
bool blueButtonState = true;
bool greenButtonState = true;
bool redButtonState = true;

// modes (0: random number, 1: geiger counter, 2: battery voltage)
short mode = 0;
short nModes = 3;

// mode 0: random number
bool interrupted = false;
bool waitForRandomNumber = false;
short digit = 0;
short randomRangeDigits[] = {6, 0, 0}; // reversed decimal
short randomRange = 6;
short randomNumber = 1;
char randomNumberChars[4];

// mode 1: geiger counter
short count = 0;
unsigned long lastUpdate = 0;
float countsPerMinute = 0.0;
char countsPerMinuteChars[7];
float sievert = 0.0;
char sievertChars[6];

// mode 2: battery voltage
float batteryVoltage = 0;
char batteryVoltageChars[10];
short batterySymbolState;

// will be called on interrupt by particle detection
// do not write to the LCD in this function!!! (loop will break)
void interrupt()
{
  randomNumber = (micros() / MICROS_FAKTOR) % randomRange + 1; // get new random number using division with remainder
  Serial.println("interrupt");
  interrupted = true;
  count++; // increase counts per minutes
  digitalWrite(SIGNAL_LAMP, HIGH);
}

void setup()
{

  // serial
  Serial.begin(9600);
  Serial.println("starting");

  // pins
  pinMode(GEIGER_COUNTER, INPUT);
  pinMode(RED_BUTTON, INPUT_PULLUP);
  pinMode(GREEN_BUTTON, INPUT_PULLUP);
  pinMode(BLUE_BUTTON, INPUT_PULLUP);
  pinMode(YELLOW_BUTTON, INPUT_PULLUP);
  pinMode(SIGNAL_LAMP, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(GEIGER_COUNTER), interrupt, FALLING);

  // start LCD display
  lcd.init();
  lcd.createChar(0, radioactiveUpperLeft);
  lcd.createChar(1, radioactiveUpperCenter);
  lcd.createChar(2, radioactiveUpperRight);
  lcd.createChar(3, radioactiveLowerCenter);
  lcd.createChar(4, batteryLeft);
  lcd.createChar(5, batteryCenterFull);
  lcd.createChar(6, batteryCenterEmpty);
  lcd.createChar(7, batteryRight);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.write(byte(0));
  lcd.write(byte(1));
  lcd.write(byte(2));
  lcd.print(" R(006)=   ? ");
  lcd.setCursor(0, 1);
  lcd.print(" ");
  lcd.write(byte(3));
  lcd.print("              ");
}

void loop()
{

  // yellow button / change mode
  if (yellowButtonState && !digitalRead(YELLOW_BUTTON)) // is pressed but wasn't before
  {
    Serial.println("yellow button");
    mode = (mode + 1) % nModes;
    Serial.println(mode);
    if (mode == 0) // random number
    {
      sprintf(randomNumberChars, "%03d", randomNumber);
      lcd.setCursor(4, 0);
      lcd.print("R(");
      lcd.print((char)(randomRangeDigits[2] + 48)); // digit as char
      lcd.print((char)(randomRangeDigits[1] + 48)); // digit as char
      lcd.print((char)(randomRangeDigits[0] + 48)); // digit as char
      lcd.print(")=  ");
      lcd.print(randomNumberChars);
      lcd.setCursor(4, 1);
      lcd.print("            ");
    }
    else if (mode == 1) // geiger counter
    {
      waitForRandomNumber = false;
      digit = 0;
      dtostrf(countsPerMinute, 6, 1, countsPerMinuteChars);
      dtostrf(sievert, 5, 2, sievertChars);
      lcd.setCursor(4, 0);
      lcd.print("A=");
      lcd.print(countsPerMinuteChars);
      lcd.print("/min");
      lcd.setCursor(4, 1);
      lcd.print("E=");
      lcd.print(sievertChars);
      lcd.print("\xE4Sv/h");
    }
    else if (mode == 2) // battery voltage
    {
      waitForRandomNumber = false;
      digit = 0;
      dtostrf(batteryVoltage, 9, 2, batteryVoltageChars);
      lcd.setCursor(4, 0);
      lcd.print("U=");
      lcd.print(batteryVoltageChars);
      lcd.print("V");
      lcd.setCursor(4, 1);
      lcd.print("      ");
      lcd.write(byte(4));
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(6));
      lcd.write(byte(6));
      lcd.write(byte(7));
    }
  }
  yellowButtonState = digitalRead(YELLOW_BUTTON);

  // blue button / change selected digit of randomNumberRange
  if (blueButtonState && !digitalRead(BLUE_BUTTON)) // is pressed but wasn't before
  {
    Serial.print("blue button");
    if (digit && (mode == 0))
    {
      randomRangeDigits[digit - 1] = (randomRangeDigits[digit - 1] + 1) % 10;
      randomRange = randomRangeDigits[0] + 10 * randomRangeDigits[1] + 100 * randomRangeDigits[2];
      lcd.setCursor(9 - digit, 0);
      lcd.print((char)(randomRangeDigits[digit - 1] + 48)); // digit as char
    }
    Serial.println((char)(randomRangeDigits[digit - 1] + 48)); // digit as char
  }
  blueButtonState = digitalRead(BLUE_BUTTON);

  // green button / select a digit of randomNumberRange
  if (greenButtonState && !digitalRead(GREEN_BUTTON))
  {
    Serial.print("green button");
    if (mode == 0)
    {
      digit = (digit + 1) % 4;
      if (digit)
      {
        lcd.setCursor(9 - digit, 1);
        lcd.print("^"); // print cursor
        lcd.print(" "); // overwrite old cursor
      }
      else
      {
        lcd.setCursor(6, 1);
        lcd.print("   "); // overwrite old cursor
      }
    }
    Serial.println(digit);
  }
  greenButtonState = digitalRead(GREEN_BUTTON);

  // red button / get new random number
  if (redButtonState && !digitalRead(RED_BUTTON)) // is pressed but wasn't before
  {
    Serial.println("red button");
    digitalWrite(SIGNAL_LAMP, LOW);
    if (mode == 0)
    {
      lcd.setCursor(13, 0);
      lcd.print(" ? ");
      delay(300); // for better visual feedback
    }

    waitForRandomNumber = true; // new number will now be generated by "radioactive interrupt"
  }
  redButtonState = digitalRead(RED_BUTTON);

  // update lcd
  if (mode == 0 && interrupted && waitForRandomNumber)
  { // update mode 0: random number
    sprintf(randomNumberChars, "%03d", randomNumber);
    lcd.setCursor(13, 0);
    lcd.print(randomNumberChars);
    interrupted = false;
    waitForRandomNumber = false;
  }
  else if (mode == 1 && ((millis() - lastUpdate) > COUNTING_INTERVAL))
  { // update mode 1: geiger counter
    countsPerMinute = count * (60000.0 / COUNTING_INTERVAL) - BACKGROUND;
    sievert = countsPerMinute / CONVERSION_INDEX;
    dtostrf(countsPerMinute, 6, 1, countsPerMinuteChars);
    dtostrf(sievert, 5, 2, sievertChars);
    lcd.setCursor(6, 0);
    lcd.print(countsPerMinuteChars);
    lcd.print("/min");
    lcd.setCursor(6, 1);
    lcd.print(sievertChars);
    lcd.print("\xE4Sv/h");
    lastUpdate = millis();
    count = 0;
  }
  else if (mode == 2)
  { // update mode 2: battery voltage
    batteryVoltage = analogRead(VOLTAGE) * VOLTAGE_SCALE;
    dtostrf(batteryVoltage, 9, 2, batteryVoltageChars);
    batterySymbolState = map(batteryVoltage, 4.5, 6.1, 0, 4);
    lcd.setCursor(6, 0);
    lcd.print(batteryVoltageChars);
    lcd.print("V");
    lcd.setCursor(11, 1);
    for (short i = 0; i < 4; i++)
    {
      if (i <= batterySymbolState)
      {
        lcd.write(byte(5));
      }
      else
      {
        lcd.write(byte(6));
      }
    }
  }

  // delay
  delay(100);
}
