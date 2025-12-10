//luminozitate LCD, matrice
//rezolvat problema cand accelerez si apare STOP, trag cu totul clientul dupa mine(functionalitate de ratat pieton)
//functionalitate de dat cu spatele

#include "LedControl.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>

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
  GAME,
  GAME_OVER
};


State currentState = INTRO;

String menuItems[] = { "Start Game", "High Score", "Reset Score" };
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

bool hasPassenger = false;
int objectiveDistance = 0;
int objectiveCol = 0;
int money = 0;
int highScore = 0;

int timeLeft = 60;

unsigned long lastTimerTick = 0;
const int timeBonus = 10;





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

  EEPROM.get(0, highScore);
  if (highScore < 0) highScore = 0;

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
    case GAME_OVER:
      if (buttonPressed)
      {
        buttonPressed = false;
        currentState = MENU;
        updateMenuLCD();
        delay(500);
      }
      break;
  }
}







void singStartMelody()
{
  int melody[] = { NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, NOTE_G5 };
  int dur[] = { 100, 100, 100, 200, 400 };
  for (int i = 0; i < 5; i++)
  {
    tone(buzzerPin, melody[i], dur[i]);
    delay(dur[i] * 1.3);
    noTone(buzzerPin);
  }
}







void singMoneyMelody()
{
  tone(buzzerPin, NOTE_G5, 100);
  delay(100);
  tone(buzzerPin, NOTE_C5, 100);
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
  if (yVal < minThreshold && menuIndex > 0)
  {
    menuIndex--;
    updateMenuLCD();
    tone(buzzerPin, 400, 20);
    delay(200);
  }
  if (yVal > maxThreshold && menuIndex < 2)
  {
    menuIndex++;
    updateMenuLCD();
    tone(buzzerPin, 400, 20);
    delay(200);
  }

  if (buttonPressed)
  {
    buttonPressed = false;

    if (menuIndex == 0)
    {
      singStartMelody();

      enemyRow = -5;
      enemyCol = 3;
      money = 0;
      timeLeft = 60;

      hasPassenger = false;
      objectiveDistance = 30;
      objectiveCol = 0;
      lastTimerTick = millis();

      currentState = GAME;
      lcd.clear();
    }
    else if (menuIndex == 1)
    {
      lcd.clear();
      lcd.print("High Score:");
      lcd.setCursor(0, 1);
      lcd.print(highScore);
      lcd.print(" $");
      delay(2000);
      updateMenuLCD();
    }
    else if (menuIndex == 2)
    {
      highScore = 0;
      EEPROM.put(0, highScore);
      lcd.clear();
      lcd.print("Score Reset!");
      delay(1500);
      updateMenuLCD();
    }
  }
}







void updateMenuLCD()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(">");
  lcd.print(menuItems[menuIndex]);

  lcd.setCursor(0, 1);
  if (menuIndex < 2)
  {
    lcd.print(menuItems[menuIndex + 1]);
  }
}








void updateGameLCD(int distSensor)
{
  lcd.setCursor(0, 0);
  lcd.print("T:");
  if (timeLeft < 10) lcd.print("0");
  lcd.print(timeLeft);

  lcd.print(" $");
  lcd.print(money);

  lcd.print("    ");

  lcd.setCursor(0, 1);

  if (hasPassenger) lcd.print("Drop:");
  else lcd.print("Get :");

  if (objectiveDistance > 0)
  {
    if (objectiveCol == 0)
    {
      lcd.print("<-");
      lcd.print(objectiveDistance);
      lcd.print("m  ");
    }
    else
    {
      if (objectiveDistance < 10) lcd.print(" ");
      lcd.print(objectiveDistance);
      lcd.print("m->");
      lcd.print("  ");
    }
  }
  else
  {
    lcd.print(" STOP! ");
  }
}








void runGame()
{
  if (millis() - lastTimerTick >= 1000)
  {
    timeLeft--;
    lastTimerTick = millis();

    if (timeLeft < 10 && timeLeft > 0)
    {
      tone(buzzerPin, 1000, 50);
    }

    if (timeLeft <= 0)
    {
      tone(buzzerPin, 200, 1000);
      lcd.clear();
      lcd.print("TIME'S UP!");
      lcd.setCursor(0, 1);
      lcd.print("Score: "); lcd.print(money);
      delay(3000);
      currentState = GAME_OVER;
      return;
    }
  }




  long dur, dist;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  dur = pulseIn(echoPin, HIGH);
  dist = dur * 0.034 / 2;

  if (dist < 5) mapSpeedDelay = 100;
  else if (dist < 15) mapSpeedDelay = 250;
  else if (dist < 25) mapSpeedDelay = 500;
  else mapSpeedDelay = 9999;

  updateGameLCD(dist);

  int dynamicSpeed = enemyConstantSpeed - (money * 2);

  if (dynamicSpeed < 100) dynamicSpeed = 100;

  if (millis() - lastEnemyMoveForward > dynamicSpeed)
  {
    enemyRow--;
    if (enemyRow < -8)
    {
      enemyRow = -2;
      enemyCol = random(2, 6);
    }
    lastEnemyMoveForward = millis();
    updateMatrixImage();
  }

  if (mapSpeedDelay < 2000 && millis() - lastMapUpdate > mapSpeedDelay)
  {
    enemyRow++;
    if (objectiveDistance > 0) objectiveDistance--;

    tone(buzzerPin, 60, 15);

    if (enemyRow > 9)
    {
      enemyRow = -8;
      enemyCol = random(2, 6);
    }
    lastMapUpdate = millis();
    updateMatrixImage();
  }

  if (objectiveDistance == 0 && mapSpeedDelay > 2000)
  {
    bool properStop = false;

    if (objectiveCol == 0 && xPos == 2) properStop = true;
    if (objectiveCol == 7 && xPos == 5) properStop = true;

    if (properStop)
    {
      singMoneyMelody();

      if (!hasPassenger)
      {
        hasPassenger = true;
        objectiveDistance = random(40, 85);
        objectiveCol = (random(0, 2) == 0) ? 0 : 7;
        timeLeft += 2;
      }
      else
      {
        hasPassenger = false;
        money += 50;
        timeLeft += timeBonus;

        objectiveDistance = random(20, 50);
        objectiveCol = (random(0, 2) == 0) ? 0 : 7;

        if (money > highScore)
        {
          highScore = money;
          EEPROM.put(0, highScore);
        }
      }
      delay(1000);
    }
  }

  if (millis() - lastMovedCar > carMoveInterval)
  {
    int xVal = analogRead(xPin);
    if (xVal < minThreshold && xPos > 2) xPos--;
    if (xVal > maxThreshold && xPos < 5) xPos++;
    lastMovedCar = millis();
    updateMatrixImage();
  }
}







void updateMatrixImage()
{
  lc.clearDisplay(0);

  bool crash = false;
  if (xPos == enemyCol && enemyRow >= 6 && enemyRow <= 8)
  {
    crash = true;
  }

  if (crash)
  {
    tone(buzzerPin, 200, 500);
    lcd.clear();
    lcd.print("CRASH! Game Over");
    lcd.setCursor(0, 1);
    lcd.print("Score: "); lcd.print(money);
    delay(2000);
    currentState = GAME_OVER;
    buttonPressed = false;
    return;
  }

  for (int row = 0; row < 8; row++)
  {
    byte currentRow = B01000010;

    if (row == enemyRow || row == enemyRow - 1)
    {
      if (enemyCol >= 0 && enemyCol <= 7)
        currentRow = currentRow | (1 << (7 - enemyCol));
    }

    if (row == 6 || row == 7)
    {
      currentRow = currentRow | (1 << (7 - xPos));
    }

    if (objectiveDistance < 8)
    {
      int visualRow = 6 - objectiveDistance;

      if (row == visualRow)
      {
        if (objectiveCol == 0) currentRow |= B10000000;
        if (objectiveCol == 7) currentRow |= B00000001;
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
