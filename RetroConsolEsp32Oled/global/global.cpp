#include "global.h"
#include "gameCymbergaj/Cymbergaj.cpp"
#include "gameSlalom/Slalom.cpp"
#include "gameDino/Dino.cpp"
#include "gameWilk/Wilk.cpp"
#include "gameSpaceShooter/SpaceShooter.cpp"
#include "gameHackMe/HackMe.cpp"

Adafruit_SH1106G myOLED = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


bool soundEnabled = false;

bool Debug = true;
bool sounddone = false;
bool pauseGame = false;

int GameSpeed = 0;
int GameSelected = 1;

int score = 0;
int SessionScore = 0;

timerStruct Timer1Sec = {false, DELAY1000MS, 0, 0};

void displayVar(int i) {
	//myOLED.clearDisplay();
  myOLED.setTextColor(SH110X_WHITE);
  myOLED.fillRect(10, 35, 26, 26, SH110X_BLACK);
  myOLED.setTextSize(2);
  myOLED.setCursor(10,35);
  myOLED.print(i);
  //myOLED.display();
}

btPressedCode ReadButton(void (*callback)(timerStruct&), timerStruct& t) {
  btPressedCode ButtonCode = NONE;
  if  (digitalRead(BTN_1) == LOW) ButtonCode = DownLeft;
  if  (digitalRead(BTN_0) == LOW) ButtonCode = UpLeft;
  if  (digitalRead(BTN_3) == LOW) ButtonCode = DownRight;
  if  (digitalRead(BTN_4) == LOW) ButtonCode = UpRight;

  if (callback) callback(t); //update timera
  delay(10); // debounce

  return ButtonCode;
}

bool IsPressed(btPressedCode button){
   delay(10); // debounce
   switch(button) {
     case DownLeft:
       if (digitalRead(BTN_1) == LOW) return true;
       break;
     case UpLeft:
       if (digitalRead(BTN_0) == LOW) return true;
       break;
     case DownRight:
       if (digitalRead(BTN_3) == LOW) return true;
       break;
     case UpRight:
       if (digitalRead(BTN_4) == LOW) return true;
       break;
   }
  return false;
}

void WaitforButton() {
  btPressedCode btn = ReadButton(nullptr, Timer1Sec);
  while (btn == NONE) {
    btn = ReadButton(nullptr, Timer1Sec);
  } // wait for button press
  
}

void WelcomeScreen() {
  myOLED.setTextColor(SH110X_WHITE);
  myOLED.clearDisplay();
  myOLED.display();
  myOLED.setTextSize(2);
  myOLED.setCursor(10,20);
  myOLED.print("ArduGame");
  myOLED.setTextSize(1);
  myOLED.setCursor(60,50);
  myOLED.print("dla Filipa");
  myOLED.display();
}

int GameSelectMenu() {

  int GameSelected = 0;
  displayMenu(GameSelected, totalGamesNo);
  btPressedCode btn = NONE;

  while(btn != DownRight) {
    btn = ReadButton(nullptr, Timer1Sec);
    while (btn == NONE) {btn = ReadButton(nullptr, Timer1Sec);}
    if (btn == DownLeft && GameSelected < (totalGamesNo - 1)) {
      GameSelected++;
    }
    if (btn == UpLeft && GameSelected > 0) {
      GameSelected--;
    }
    if (btn == UpRight) {
      soundEnabled = !soundEnabled;
    }
    while (ReadButton(nullptr, Timer1Sec) != NONE); // wait for button release
    displayMenu(GameSelected, totalGamesNo);
  }
  return GameSelected;
}


void displayMenu(int MenuStartRow, int totalGamesNo){ 
  //Serial.println("displayMenu");

  if (MenuStartRow > totalGamesNo) return;

  myOLED.clearDisplay();
  myOLED.setTextColor(SH110X_WHITE);
  myOLED.setTextSize(1);

  if (totalGamesNo==0) return;
  if (MenuStartRow < 0) MenuStartRow = 0;
  if (MenuStartRow > totalGamesNo) MenuStartRow = totalGamesNo;
  myOLED.setCursor(1,0);
  myOLED.print(">>");

  for (int i=MenuStartRow; i<MenuStartRow+maxMenuRowsOnScreen && i< totalGamesNo; i++){
    myOLED.setCursor(20,(i-MenuStartRow)*12);
    myOLED.print(String(i+1)+". ");
    myOLED.print(allGames[i]->name);
  }
  displaySound(soundEnabled);
  //displayVar(MenuStartRow);
  myOLED.display();
}

bool CheckIfTimePassed(unsigned long& LastTimeCheckOneSec, unsigned long interval) {
  if (millis()-LastTimeCheckOneSec > interval) {
      LastTimeCheckOneSec  = millis();
      return true;
  }
  return false;
}

void MyTune(int freq, int duration_ms) {
  tone(BUZZER_PIN, freq, duration_ms);
  delay(duration_ms);
  noTone(BUZZER_PIN);
}
void displaySound(bool sound){
  myOLED.setTextSize(1);
  //myOLED.setCursor(116,1);
  if (sound) {
  myOLED.drawBitmap(120, 0, Glosnik, Glosnik_x_y, Glosnik_x_y, SH110X_WHITE);
  //myOLED.print("!!");
  } else {
    myOLED.fillRect(120, 0, Glosnik_x_y, Glosnik_x_y, SH110X_BLACK);
  }
}
void displaySound2(bool sound){
  myOLED.setTextSize(1);
  //myOLED.setCursor(116,1);
  if (sound) {
  myOLED.drawBitmap(120, 14, Glosnik, Glosnik_x_y, Glosnik_x_y, SH110X_WHITE);
  //myOLED.print("!!");
  } else {
    myOLED.fillRect(120, 14, Glosnik_x_y, Glosnik_x_y, SH110X_BLACK);
  }
}
void CheckIfResetHighscores(){
  if (IsPressed(UpLeft) && IsPressed(DownRight) && IsPressed(UpRight) && IsPressed(DownLeft)) {
    int reset = 0;
    EEPROM.put(WolfRecord, reset);
    EEPROM.put(SlalomRecord, reset);
    EEPROM.put(DinoRecord, reset);
    EEPROM.commit();
  }
 }

 void DisplayHighscores(){
  myOLED.clearDisplay();
  myOLED.setTextSize(1);

  myOLED.setCursor(5,1);
  myOLED.print("Highscores");

  myOLED.setCursor(5,15);
  myOLED.print("DinoRecord: ");
  int highscore;
  EEPROM.get(DinoRecord, highscore);
  myOLED.print(highscore);
  myOLED.setCursor(5,27);
  myOLED.print("WolfRecord: ");
  EEPROM.get(WolfRecord, highscore);
  myOLED.print(highscore);
  myOLED.setCursor(5,39);
  myOLED.print("SlalomRecord: ");
  EEPROM.get(SlalomRecord, highscore);
  myOLED.print(highscore);
  myOLED.display();
  delay(1000);
 }

const GameInfo HelpInfo = {
  "Pomoc/info",
  "opis obslugi konsoli",
  DisplayHelpInfo
};

void DisplayHelpInfo() {
  myOLED.clearDisplay();
  myOLED.setTextSize(1);
  //myOLED.setFont(&TomThumb);
  for (int i=0; i<4; i++) {
    char buffer[64];
    const char* ptr = (const char*)pgm_read_ptr(&(teksty[i]));
    strcpy_P(buffer, ptr);
    myOLED.setCursor(0, 2+i*12);
    myOLED.print(buffer);
  }
  myOLED.display();
  delay(1000);
  WaitforButton();
  myOLED.clearDisplay();
  for (int i=4; i<9; i++) {
    char buffer[120];
    const char* ptr = (const char*)pgm_read_ptr(&(teksty[i]));
    strcpy_P(buffer, ptr);
    myOLED.setCursor(0, 2+i*12-48);
    myOLED.print(buffer);
  }
  myOLED.display();
  WaitforButton();
  delay(300);
  myOLED.clearDisplay();
   for (int i=4; i<9; i++) {
    char buffer[120];
    const char* ptr = (const char*)pgm_read_ptr(&(teksty[i]));
    strcpy_P(buffer, ptr);
    myOLED.setCursor(0, 2+i*12-48);
    myOLED.print(buffer);
  }
  myOLED.display();
  WaitforButton();
  delay(500);
  myOLED.clearDisplay();
   for (int i=9; i<14; i++) {
    char buffer[120];
    const char* ptr = (const char*)pgm_read_ptr(&(teksty[i]));
    strcpy_P(buffer, ptr);
    myOLED.setCursor(0, 2+i*12-108);
    myOLED.print(buffer);
  }
  myOLED.display();
  WaitforButton();
  delay(300);

 }

void SerialPrintFreeRam() {
  Serial.print("Wolna pamięć RAM: ");
  Serial.println(ESP.getFreeHeap());      // ilość wolnej pamięci heap (RAM)
  Serial.print("Największy możliwy blok do alokacji: ");
  Serial.println(ESP.getMaxAllocHeap());  // największy możliwy blok do alokacji
  Serial.println("----");
 }