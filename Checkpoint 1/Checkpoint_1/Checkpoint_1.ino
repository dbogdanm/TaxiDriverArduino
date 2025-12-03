#include "LedControl.h"
#include <LiquidCrystal.h>

#define NOTE_C4 262
#define NOTE_E4 330
#define NOTE_G4 392
#define NOTE_C5 523
#define NOTE_G5 784

const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;
const int xPin = A0;
const int yPin = A1;
const int swPin = 2;
const int trigPin = 6;
const int echoPin = 5;
const int buzzerPin = 13;

const int rs = 9, en = 8, d4 = 7, d5 = 4, d6 = 3, d7 = A2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);

enum State
{
  INTRO,
  MENU,
  GAME
};
State currentState = INTRO;

String menuItems[] = { "Start Game", "Settings", "High Score" };
int menuIndex = 0;
bool buttonPressed = false;
unsigned long lastDebounceTime = 0;

byte xPos = 3;
const int minThreshold = 200;
const int maxThreshold = 800;
unsigned long lastMovedCar = 0;
const int carMoveInterval = 150;

int enemyRow = -1;
int enemyCol = 3;
unsigned long lastEnemyMoveForward = 0;
const int enemyConstantSpeed = 400;

unsigned long lastMapUpdate = 0;
int mapSpeedDelay = 0;

byte worldMap[] = { B01000010, B01000010, B01000010, B01000010,
                    B01000010, B01000010, B01000010, B01000010 };

void setup()
{
  Serial.begin(9600);
  lc.shutdown(0, false);
  lc.setIntensity(0, 2);
  lc.clearDisplay(0);

  pinMode(swPin, INPUT_PULLUP);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  lcd.begin(16, 2);
  showIntro();
}

void loop()
{
  handleInput();
  switch (currentState) 
  {
    case MENU:
      runMenu();
      break;
    case GAME:
      runGame();
      break;
  }
}

void singStartMelody()
{
  int melody[] = { NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, NOTE_G5 };
  int dur[] = { 100, 100, 100, 200, 400 };
  for (int i = 0; i < 5; i++) {
    tone(buzzerPin, melody[i], dur[i]);
    delay(dur[i] * 1.3);
    noTone(buzzerPin);
  }
}

void showIntro()
{
  lcd.clear();
  lcd.print("Taxi Driver");
  tone(buzzerPin, NOTE_G4, 100);
  delay(2000);
  lcd.clear();
  currentState = MENU;
  updateMenuLCD();
}

void runMenu()
{
  int yVal = analogRead(yPin);
  if (yVal < minThreshold && menuIndex > 0) {
    menuIndex--;
    updateMenuLCD();
    tone(buzzerPin, 400, 20);
    delay(200);
  }
  if (yVal > maxThreshold && menuIndex < 2) {
    menuIndex++;
    updateMenuLCD();
    tone(buzzerPin, 400, 20);
    delay(200);
  }

  if (buttonPressed) {
    buttonPressed = false;
    if (menuIndex == 0) {
      singStartMelody();
      enemyRow = -5;
      enemyCol = 3;
      currentState = GAME;
      lcd.clear();
    }
  }
}

void updateMenuLCD()
{
  lcd.setCursor(0, 0);
  lcd.print(">");
  lcd.print(menuItems[menuIndex]);
  lcd.print("   ");
  if (menuIndex < 2) {
    lcd.setCursor(1, 1);
    lcd.print(menuItems[menuIndex + 1]);
    lcd.print("   ");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
}

void runGame()
{

  long dur, dist;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  dur = pulseIn(echoPin, HIGH);
  dist = dur * 0.034 / 2;

  lcd.setCursor(0, 0);
  lcd.print("Speed: ");
  if (dist < 30)
    lcd.print((30 - dist) * 3);
  else
    lcd.print("0 ");
  lcd.print("km/h  ");

  if (dist < 5)
    mapSpeedDelay = 100;
  else if (dist < 15)
    mapSpeedDelay = 250;
  else if (dist < 25)
    mapSpeedDelay = 500;
  else
    mapSpeedDelay = 9999;

  if (millis() - lastEnemyMoveForward > enemyConstantSpeed) 
  {
    enemyRow--;
    if (enemyRow < -8) {
      enemyRow = -2;
      enemyCol = random(2, 6);
    }
    lastEnemyMoveForward = millis();
    updateMatrixImage();
  }

  if (mapSpeedDelay < 2000 && millis() - lastMapUpdate > mapSpeedDelay) 
  {
    enemyRow++;

    tone(buzzerPin, 60, 15);

    if (enemyRow > 9) 
    {
      enemyRow = -8;
      enemyCol = random(2, 6);
    }

    lastMapUpdate = millis();
    updateMatrixImage();
  }

  if (millis() - lastMovedCar > carMoveInterval) 
  {
    int xVal = analogRead(xPin);
    if (xVal < minThreshold && xPos > 2)
      xPos--;
    if (xVal > maxThreshold && xPos < 5)
      xPos++;
    lastMovedCar = millis();
    updateMatrixImage();
  }
}

void updateMatrixImage()
{
  lc.clearDisplay(0);

  bool crash = false;
  if (xPos == enemyCol) {

    if (enemyRow >= 6 && enemyRow <= 8) 
    {
      crash = true;
    }
  }

  if (crash) 
  {
    tone(buzzerPin, 200, 50);
  }

  for (int row = 0; row < 8; row++) 
  {

    byte currentRow = B01000010;

    if (row == enemyRow || row == enemyRow - 1) 
    {
      currentRow = currentRow | (1 << (7 - enemyCol));
    }

    if (row == 6 || row == 7) 
    {
      if (crash) 
      {

        currentRow = currentRow ^ (1 << (7 - xPos));
      }
       else 
      {
        currentRow = currentRow | (1 << (7 - xPos));
      }
    }

    lc.setRow(0, row, currentRow);
  }
}

void handleInput()
{
  if (digitalRead(swPin) == LOW && (millis() - lastDebounceTime > 200)) 
  {
    buttonPressed = true;
    lastDebounceTime = millis();
  }
}