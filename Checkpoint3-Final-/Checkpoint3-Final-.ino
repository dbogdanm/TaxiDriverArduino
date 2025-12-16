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

const int trigPin = 3;          
const int lcdBacklightPin = 6;
const int echoPin = 5;
const int buzzerPin = 13;

const int rs = 9, en = 8, d4 = 7, d5 = 4, d6 = A3, d7 = A2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);

byte iconPlay[] = { B00000000, B00000000, B00100100, B01111110, B01111110, B00100100, B00000000, B00000000 };
byte iconSettings[] = { B00011000, B00100100, B01011010, B10111101, B10111101, B01011010, B00100100, B00011000 };
byte iconCup[] = { B00000000, B11111110, B01111100, B00111000, B00010000, B00010000, B00111000, B00000000 };
byte iconInfo[] = { B00011000, B00011000, B00000000, B00011000, B00011000, B00011000, B00011000, B00011000 };

enum State { INTRO, MENU, SETTINGS, INFO_SCROLL, GAME, ENTER_NAME, GAME_OVER };
State currentState = INTRO;

String menuItems[] = { "Start Game", "Settings", "High Scores", "About", "How to Play" };
int menuIndex = 0;
const int menuLen = 5;
bool buttonPressed = false;
unsigned long lastDebounceTime = 0;

int xPos = 3; 
int cameraOffset = 0; 

const int minThreshold = 200;
const int maxThreshold = 800;
unsigned long lastMovedCar = 0;
const int carMoveInterval = 150;

const int MAX_CARS = 10; 
int enemyRows[MAX_CARS];
int enemyCols[MAX_CARS];

unsigned long lastMapUpdate = 0;   

unsigned long lastEnemyUpdate = 0; 

int mapSpeedDelay = 0;             

bool hasPassenger = false;
int objectiveDistance = 0;
int objectiveCol = 0; 
int money = 0;

int timeLeft = 60;
unsigned long lastTimerTick = 0;
const int timeBonus = 10;
const int missPenaltyTime = 5; 
const int missPenaltyMoney = 20;

unsigned long lastEngineTick = 0;

int matrixBrightness = 8; 
int lcdBrightnessLevel = 5; 
bool soundEnabled = true;
int settingsPage = 0;

struct HighScoreEntry 
{
  int score;
  char name[4];
};
HighScoreEntry topScores[3];
int newHighIndex = -1;
char newName[] = "AAA";
int nameCharIndex = 0;





void setup() 
{
  Serial.begin(9600);
  pinMode(swPin, INPUT_PULLUP);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(lcdBacklightPin, OUTPUT);

  lc.shutdown(0, false);
  lcd.begin(16, 2);

  loadSettings();
  loadHighScores();
  applySettings();
  showIntro();
}




void loop() 
{
  handleInputDebounce();
  switch (currentState) {
    case MENU: runMenu(); break;
    case SETTINGS: runSettings(); break;
    case INFO_SCROLL: runInfoScroll(); break;
    case GAME: runGame(); break;
    case ENTER_NAME: runEnterName(); break;
    case GAME_OVER: 
      if (buttonPressed) {
        buttonPressed = false;
        currentState = MENU;
        updateMenuLCD();
        delay(500);
      }
      break;
  }
}




void loadSettings() 
{
  matrixBrightness = EEPROM.read(100);
  if (matrixBrightness > 15) matrixBrightness = 8;
  lcdBrightnessLevel = EEPROM.read(101);
  if (lcdBrightnessLevel > 9) lcdBrightnessLevel = 5;
  byte snd = EEPROM.read(102);
  soundEnabled = (snd != 0); 
}




void saveSettings() 
{
  EEPROM.update(100, matrixBrightness);
  EEPROM.update(101, lcdBrightnessLevel);
  EEPROM.update(102, soundEnabled ? 1 : 0);
}




void loadHighScores() 
{
  int addr = 0;
  for (int i = 0; i < 3; i++) 
  {
    EEPROM.get(addr, topScores[i]);
    if (topScores[i].score < 0) 
    { 
      topScores[i].score = 0; strcpy(topScores[i].name, "AAA");
    }
    addr += sizeof(HighScoreEntry);
  }
}




void saveHighScores() 
{
  int addr = 0;
  for (int i = 0; i < 3; i++) 
  {
    EEPROM.put(addr, topScores[i]);
    addr += sizeof(HighScoreEntry);
  }
}




void resetHighScores() 
{
  for (int i = 0; i < 3; i++) 
  {
    topScores[i].score = 0; strcpy(topScores[i].name, "AAA");
  }

  saveHighScores();
}




void applySettings() 
{
  lc.setIntensity(0, matrixBrightness);
  int pwmVal = map(lcdBrightnessLevel, 0, 9, 50, 255);
  analogWrite(lcdBacklightPin, pwmVal);
}




void playSound(int freq, int dur) 
{
  if (soundEnabled) tone(buzzerPin, freq, dur);
}




void playStartGameSoundtrack() 
{
  if (!soundEnabled) return;
  tone(buzzerPin, NOTE_C4, 150); delay(180);
  tone(buzzerPin, NOTE_E4, 150); delay(180);
  tone(buzzerPin, NOTE_G4, 150); delay(180);
  tone(buzzerPin, NOTE_C5, 300); delay(350);
  tone(buzzerPin, NOTE_E4, 150); delay(180);
  tone(buzzerPin, NOTE_G4, 300); delay(350);
}




void drawIcon(byte* icon) 
{
  for (int i = 0; i < 8; i++) lc.setRow(0, i, icon[i]);
}





void handleInputDebounce() 
{
  if (digitalRead(swPin) == LOW && (millis() - lastDebounceTime > 200)) 
  {
    buttonPressed = true; lastDebounceTime = millis();
  }
}




void updateMenuLCD() 
{
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print(">"); lcd.print(menuItems[menuIndex]);
  lcd.setCursor(0, 1);
  if (menuIndex < menuLen - 1) lcd.print(menuItems[menuIndex + 1]);

  lc.clearDisplay(0);
  if (menuIndex == 0) drawIcon(iconPlay);
  else if (menuIndex == 1) drawIcon(iconSettings);
  else if (menuIndex == 2) drawIcon(iconCup);
  else drawIcon(iconInfo);
}




void runMenu() 
{
  int yVal = analogRead(yPin);
  if (yVal < minThreshold && menuIndex > 0) 
  {
    menuIndex--; updateMenuLCD(); playSound(400, 20); delay(200);
  }

  if (yVal > maxThreshold && menuIndex < menuLen - 1) 
  {
    menuIndex++; updateMenuLCD(); playSound(400, 20); delay(200);
  }

  if (buttonPressed) 
  {
    buttonPressed = false;
    if (menuIndex == 0) 
    { 

      for(int i = 0; i < MAX_CARS; i++) 
      {
        enemyRows[i] = -2 - (i * 3); 
        enemyCols[i] = random(2, 14); 
      }

      money = 0; timeLeft = 60;
      hasPassenger = false; objectiveDistance = 30; 
      objectiveCol = 0; 
      xPos = 3; 
      lastTimerTick = millis();
      lastMapUpdate = millis();
      lastEnemyUpdate = millis();

      lcd.clear(); lcd.print("GET READY!");
      playStartGameSoundtrack();

      currentState = GAME;
      lcd.clear();
    }

    else if (menuIndex == 1) 
    { 

      currentState = SETTINGS; settingsPage = 0; updateSettingsLCD();
    }

    else if (menuIndex == 2) 
    { 

      lcd.clear(); lcd.print("Top 3 Scores:"); delay(1000);

      for(int i=0; i<3; i++) 
      {
        lcd.clear(); lcd.print(i+1); lcd.print(". "); lcd.print(topScores[i].name);
        lcd.print(" $"); lcd.print(topScores[i].score); delay(2000);
      }
      updateMenuLCD();
    }

    else if (menuIndex == 3) 
    { 

      currentState = INFO_SCROLL;
      lcd.clear();
      lcd.print("Taxi Driver"); 
      lcd.setCursor(0,1);
      lcd.print("v1.1 Final"); 
    }

    else if (menuIndex == 4) 
    { 

      currentState = INFO_SCROLL;
      lcd.clear(); lcd.print("Accel: Sensor"); lcd.setCursor(0,1); lcd.print("Steer: Joystick");
    }
  }
}




void updateSettingsLCD() 
{
  lcd.clear();
  if (settingsPage == 0) 
  {
    lcd.print("Matrix Bright:"); lcd.setCursor(0,1);
    lcd.print("< "); lcd.print(matrixBrightness); lcd.print(" >");
    for(int i=0; i<8; i++) lc.setRow(0, i, B11111111);
  } 
  else if (settingsPage == 1) 
  {
    lc.clearDisplay(0); 
    lcd.print("LCD Brightness:"); lcd.setCursor(0,1);
    lcd.print("< "); lcd.print(lcdBrightnessLevel); lcd.print(" >");
  } 
  else if (settingsPage == 2) 
  {
    lcd.print("Sound:"); lcd.setCursor(0,1);
    lcd.print(soundEnabled ? "< ON >" : "< OFF >");
  } 
  else if (settingsPage == 3) 
  {
    lcd.print("Reset Scores?"); lcd.setCursor(0,1); lcd.print("Press Button");
  }
}




void runSettings() 
{
  int xVal = analogRead(xPin);
  bool changed = false;

  if (settingsPage == 0) 
  { 
    if (xVal < minThreshold && matrixBrightness > 0) { matrixBrightness--; changed = true; delay(200); }
    if (xVal > maxThreshold && matrixBrightness < 15) { matrixBrightness++; changed = true; delay(200); }
    if(changed) { lc.setIntensity(0, matrixBrightness); updateSettingsLCD(); }
  }
  else if (settingsPage == 1) 
  {
    if (xVal < minThreshold && lcdBrightnessLevel > 0) { lcdBrightnessLevel--; changed = true; delay(200); }
    if (xVal > maxThreshold && lcdBrightnessLevel < 9) { lcdBrightnessLevel++; changed = true; delay(200); }
    if(changed) { applySettings(); updateSettingsLCD(); }
  }
  else if (settingsPage == 2) 
  {
    if (xVal < minThreshold || xVal > maxThreshold) 
    {
      soundEnabled = !soundEnabled; delay(300); updateSettingsLCD();
    }
  }

  if (buttonPressed) 
  {
    buttonPressed = false;
    if (settingsPage == 3) 
    {
      resetHighScores(); lcd.clear(); lcd.print("Scores Reset!"); delay(1000);
      saveSettings(); currentState = MENU; updateMenuLCD();
      lc.clearDisplay(0);
    } else 
    {
      settingsPage++;
      if (settingsPage > 3) 
      {
        saveSettings(); lc.clearDisplay(0); currentState = MENU; updateMenuLCD();
      } else 
      {
        updateSettingsLCD(); delay(200);
      }
    }
  }
}




void runInfoScroll() 
{
  if (buttonPressed) { buttonPressed = false; currentState = MENU; updateMenuLCD(); }
}




void updateGameLCD(int distSensor) 
{

  lcd.setCursor(0, 0);
  lcd.print("T:"); 
  if(timeLeft<10) lcd.print("0"); 
  lcd.print(timeLeft);
  lcd.print(" $"); lcd.print(money);

  if(money < 10) lcd.print("   ");
  else if(money < 100) lcd.print("  ");
  else lcd.print(" ");

  bool dangerRight = false;
  bool dangerLeft = false;

  if (xPos <= 7) 
  {
    for(int i=0; i<MAX_CARS; i++) 
    {

      if (enemyCols[i] == 8 && enemyRows[i] >= 3 && enemyRows[i] <= 9) 
      {
        dangerRight = true;
      }
    }
  }

  if (xPos >= 8) 
  {
    for(int i=0; i<MAX_CARS; i++) 
    {

      if (enemyCols[i] == 7 && enemyRows[i] >= 3 && enemyRows[i] <= 9) 
      {
        dangerLeft = true;
      }
    }
  }

  lcd.setCursor(13, 0);
  if (dangerRight) 
  {
    lcd.print(" !>"); 

  } 
  else if (dangerLeft) 
  {
    lcd.print("<! "); 

  } 
  else 
  {
    lcd.print("   "); 

  }

  lcd.setCursor(0, 1);
  if (objectiveDistance > 0) 
  {
    lcd.print("Dst:"); lcd.print(objectiveDistance); lcd.print("m ");
    if (objectiveCol == 0) 
    { 
      if (xPos < 8) lcd.print("<-HERE"); 
      else lcd.print("<<-GO"); 

    } 
    else 
    { 
      if (xPos >= 8) lcd.print("HERE->");
      else lcd.print("GO->>");
    }
  } 
  else 
  {
    lcd.print("STOP TO PICKUP! ");
  }
}




void updateMatrixImage() 
{
  lc.clearDisplay(0);
  int cameraOffset = (xPos >= 8) ? 8 : 0;
  bool crash = false;

  for(int i=0; i<MAX_CARS; i++) 
  {
     if (xPos == enemyCols[i] && enemyRows[i] >= 6 && enemyRows[i] <= 8) 
     {
       crash = true;
     }
  }

  if (xPos < 2 || xPos > 13) 
  {
    crash = true;
  }

  if (crash) 
  {
    noTone(buzzerPin);
    playSound(200, 500); lcd.clear(); lcd.print("CRASH!"); delay(1000);
    checkHighScore(); return;
  }

  for (int row = 0; row < 8; row++) 
  {
    byte displayRow = 0;

    if (objectiveDistance < 8) 
    {
      int visualRow = 6 - objectiveDistance;
      if (row == visualRow) 
      {
        if (objectiveCol == 0 && cameraOffset == 0) displayRow |= B10000000;
        if (objectiveCol == 15 && cameraOffset == 8) displayRow |= B00000001;
      }
    }

    if (cameraOffset == 0) displayRow |= B01000000; 

    else displayRow |= B00000010; 

    for(int i=0; i<MAX_CARS; i++) 
    {
      if (row == enemyRows[i] || row == enemyRows[i] - 1) 
      { 
        if (enemyCols[i] >= cameraOffset && enemyCols[i] < cameraOffset + 8) 
        {
          int displayCol = enemyCols[i] - cameraOffset;
          displayRow |= (1 << (7 - displayCol));
        }
      }
    }

    if (row == 6 || row == 7) 
    {
      if (xPos >= cameraOffset && xPos < cameraOffset + 8) 
      {
        int displayCol = xPos - cameraOffset;
        displayRow |= (1 << (7 - displayCol));
      }
    }

    lc.setRow(0, row, displayRow);
  }
}




void runGame() {

  if (millis() - lastTimerTick >= 1000) 
  {
    timeLeft--; lastTimerTick = millis();
    if (timeLeft <= 0) 
    {
      noTone(buzzerPin);
      playSound(200, 1000); lcd.clear(); lcd.print("TIME'S UP!"); delay(1000);
      checkHighScore(); return;
    }
  }

  long dur, dist;
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10); digitalWrite(trigPin, LOW);
  dur = pulseIn(echoPin, HIGH); dist = dur * 0.034 / 2;

  if (dist < 5) mapSpeedDelay = 100;        

  else if (dist < 15) mapSpeedDelay = 250;  

  else if (dist < 25) mapSpeedDelay = 500;  

  else mapSpeedDelay = 9999;                

  updateGameLCD(dist);

  if (soundEnabled && mapSpeedDelay < 2000) 
  {
    int targetFreq = map(mapSpeedDelay, 100, 1000, 500, 100);
    if (targetFreq < 100) targetFreq = 100; if (targetFreq > 500) targetFreq = 500;
    int tickInterval = mapSpeedDelay / 2;
    if (tickInterval < 50) tickInterval = 50;

    if (millis() - lastEngineTick > tickInterval) 
    {
      tone(buzzerPin, targetFreq, tickInterval / 2); lastEngineTick = millis();
    }
  } 
  else 
  {
    noTone(buzzerPin);
  }

  bool playerIsStopped = (mapSpeedDelay > 2000);
  int trafficSpeedDelay; 

  if (playerIsStopped) 
  {
     trafficSpeedDelay = 400; 

  } 
  else 
  {

     trafficSpeedDelay = map(mapSpeedDelay, 100, 1000, 150, 450); 
  }

  if (millis() - lastEnemyUpdate > trafficSpeedDelay) 
  {
    lastEnemyUpdate = millis();

    if (playerIsStopped) 
    {

      for(int i=0; i<MAX_CARS; i++) 
      {
        enemyRows[i]--; 

        if (enemyRows[i] < -8) 
        {
           enemyRows[i] = -2; 

           enemyCols[i] = random(2, 14);
        }
      }
    } 
    else 
    {

      for(int i=0; i<MAX_CARS; i++) 
      {
        enemyRows[i]++; 

        if (enemyRows[i] > 9) 
        { 
          enemyRows[i] = random(-4, -1); 

          enemyCols[i] = random(2, 14); 
        }
      }
    }
    updateMatrixImage();
  }

  if (digitalRead(swPin) == LOW) 
  { 

    if (millis() - lastMapUpdate > 600) 
    {

      if (objectiveDistance < 99) objectiveDistance++;
      playSound(800, 100); lastMapUpdate = millis(); updateMatrixImage();
    }
  } 

  else 
  { 

    if (!playerIsStopped && millis() - lastMapUpdate > mapSpeedDelay) 
    {
      lastMapUpdate = millis();

      if (objectiveDistance > 0) objectiveDistance--;
      else if (objectiveDistance == 0) 
      {

         playSound(NOTE_G4, 150); lcd.setCursor(0, 1); lcd.print("MISSED!");
         timeLeft -= missPenaltyTime; money -= missPenaltyMoney; if(money<0) money=0;
         objectiveDistance = random(20, 50);
         objectiveCol = (random(0,2)==0) ? 0 : 15;
         delay(1000);
      }
      updateMatrixImage();
    }

    if (objectiveDistance == 0 && playerIsStopped) 
    {
      bool properStop = false;
      if (objectiveCol == 0 && xPos == 2) properStop = true;
      if (objectiveCol == 15 && xPos == 13) properStop = true;

      if (properStop) 
      {
        if(soundEnabled) { tone(buzzerPin, NOTE_G5, 100); delay(100); tone(buzzerPin, NOTE_C5, 100); }
        if (!hasPassenger) 
        {
          hasPassenger = true; timeLeft += 2;
          objectiveDistance = random(40, 85);
          objectiveCol = (random(0,2)==0) ? 0 : 15;
        } 
        else 
        {
          hasPassenger = false; money += 50; timeLeft += timeBonus;
          objectiveDistance = random(20, 50);
          objectiveCol = (random(0,2)==0) ? 0 : 15;
        }
        delay(1000);
      }
    }

    if (millis() - lastMovedCar > carMoveInterval) 
    {
      int xVal = analogRead(xPin);
      bool moved = false;
      if (xVal < minThreshold && xPos > 2) { xPos--; moved = true; }
      if (xVal > maxThreshold && xPos < 13) { xPos++; moved = true; }
      if (moved) 
      {
        lastMovedCar = millis();
        updateMatrixImage();
      }
    }
  }
}





void checkHighScore() 
{
  newHighIndex = -1;
  for (int i = 0; i < 3; i++) 
  {
    if (money > topScores[i].score) { newHighIndex = i; break; }
  }

  if (newHighIndex != -1) 
  {
    strcpy(newName, "AAA"); nameCharIndex = 0;
    currentState = ENTER_NAME; lcd.clear(); lcd.print("NEW HIGH SCORE!"); delay(2000);
  } 
  else 
  {
    currentState = GAME_OVER; lcd.clear(); lcd.print("GAME OVER");
    lcd.setCursor(0,1); lcd.print("Score: "); lcd.print(money);
  }
}




void runEnterName() 
{
  lcd.setCursor(0, 0); lcd.print("Enter Name:");
  lcd.setCursor(0, 1); lcd.print(newName);
  lcd.setCursor(nameCharIndex, 1); lcd.cursor(); 

  int yVal = analogRead(yPin); int xVal = analogRead(xPin);
  if (millis() - lastDebounceTime > 200) 
  {
    bool moved = false;
    if (yVal < minThreshold) { newName[nameCharIndex]--; if (newName[nameCharIndex] < 'A') newName[nameCharIndex] = 'Z'; moved = true; }
    if (yVal > maxThreshold) { newName[nameCharIndex]++; if (newName[nameCharIndex] > 'Z') newName[nameCharIndex] = 'A'; moved = true; }
    if (xVal > maxThreshold && nameCharIndex < 2) { nameCharIndex++; moved = true; }
    if (xVal < minThreshold && nameCharIndex > 0) { nameCharIndex--; moved = true; }
    if (moved) { playSound(1000, 10); lastDebounceTime = millis(); }
  }

  if (buttonPressed) 
  {
    buttonPressed = false; lcd.noCursor(); saveNewHighScore();
    currentState = GAME_OVER; lcd.clear(); lcd.print("Saved!"); delay(1500);
  }
}




void saveNewHighScore() 
{
  for (int i = 2; i > newHighIndex; i--) topScores[i] = topScores[i-1];
  topScores[newHighIndex].score = money; strcpy(topScores[newHighIndex].name, newName);
  saveHighScores();
}




void showIntro() 
{
  lcd.clear(); lcd.print("Taxi Driver 1.0"); 
  playSound(NOTE_C5, 100); delay(150); playSound(NOTE_E4, 100);
  delay(2000); currentState = MENU; updateMenuLCD();
}