#include "HackMe.h"
#include "HackMe_private.h"

namespace HackMeGame {

void Game_HackMe() {
  HackMeStruct HackMeGame;
  btPressedCode ButtonCode;

  WelcomeHackMeScreen();
  WaitforButton();
  initGameParameters(HackMeGame, Timer1Sec);
  HackMeGame.newGame = true;

  while(true) {
    if (Timer1Sec.timeOut) timeEndedGameOver(HackMeGame, Timer1Sec);
    if (HackMeGame.newGame) {
      myOLED.clearDisplay();  
      GenerateRandomCodeMatrix(HackMeGame);

      DisplayMixedCodeAndStartCountingDown(HackMeGame, Timer1Sec);
      initGameParameters(HackMeGame, Timer1Sec);
    } else GameRestart(HackMeGame);
    while(true) {
      hdisplaySoundInfo();
      if (FirstDigitIsWrong(HackMeGame,Timer1Sec)) break;
      if (SecondDigitIsWrong(HackMeGame,Timer1Sec)) break;
      if (ThirdDigitIsWrong(HackMeGame,Timer1Sec)) break;
      if (FourthDigitIsWrong(HackMeGame,Timer1Sec)) break;
      CodeResolved();   
      HackMeGame.newGame = true;
      break;
    }
  }
}

void CodeResolved() {
  for (int i=0; i<3; i++) {
    myOLED.fillRect(10, 10, 74, 44, SH110X_BLACK);
    myOLED.setCursor(25, 20);
    myOLED.println(F("Szyfr!"));
    myOLED.setCursor(25, 35);
    myOLED.println(F("zlamany!"));
    myOLED.display();
    if (soundEnabled) {
      tone(BUZZER_PIN, 1000, 300);
      delay(DELAY500MS);
      noTone(BUZZER_PIN);
    } else delay(DELAY500MS);
    myOLED.fillRect(10, 10, 74, 44, SH110X_BLACK);
    myOLED.display();
    delay(DELAY500MS);
  }
  myOLED.clearDisplay();
  myOLED.display();
}

void GameRestart(HackMeStruct& HackMeGame) {
  myOLED.clearDisplay();
  displayMatrix(HackMeGame);
  selectFirstRowCol(HackMeGame);
  mixedCodeDisplay(HackMeGame);
  for (int i=0; i<CodeWidth; i++) (HackMeGame.CodeSignsCodesMixed[i].matched = false);
  myOLED.display();
}

void timeEndedGameOver(HackMeStruct& HackMeGame, timerStruct& Timer1Sec){
  Timer1Sec.timeOut = false;
  Timer1Sec.currTime = timeForCodeBreaking;
  displayBombExplosion();
  displayResolution(HackMeGame);
  myOLED.clearDisplay();
  myOLED.display();
}

void initGameParameters(HackMeStruct& HackMeGame, timerStruct& Timer1Sec) {
  HackMeGame.newGame = false;
  HackMeGame.SelectedRow = 0;
  HackMeGame.SelectedCol = 0;
  HackMeGame.newGame = false;

  Timer1Sec.currTime = timeForCodeBreaking;
  Timer1Sec.prevTimeCheck = millis();
  Timer1Sec.timeOut = false;
}

void DisplayMixedCodeAndStartCountingDown(HackMeStruct& HackMeGame, timerStruct& Timer1Sec) {
    selectFirstRowCol(HackMeGame);
    GenerateCode(HackMeGame, CodeWidth); // wygenerowana tablica czas wygenerowac code
    mixCode(HackMeGame, CodeWidth);
    mixedCodeDisplay(HackMeGame);
    delay(100);
    while (ReadButton(nullptr, Timer1Sec) == DownRight);
    Timer1Sec.currTime = timeForCodeBreaking;
    Timer1Sec.timeOut = false;
}

void randomMatrix(HackMeStruct& HackMeGame) {
  for (uint8_t row=0; row< ArrayMaxRowColIndex+1; row++) {
    for (uint8_t col=0; col< ArrayMaxRowColIndex+1; col++) {
      HackMeGame.GameMatrix[row][col]= random(0,12);
    }
  }
}

void displayMatrix(HackMeStruct& HackMeGame) {
  myOLED.fillRect(0,0, 80, 64, SH110X_BLACK);
  myOLED.setTextColor(SH110X_WHITE);
  myOLED.setCursor(0, 56);
  myOLED.setTextSize(1);
  for (uint8_t row=0; row< ArrayMaxRowColIndex+1; row++) {
    for (uint8_t col=0; col< ArrayMaxRowColIndex+1; col++) {
  myOLED.setCursor(4+14*col, 11*row); // x: kolumna, y: wiersz (lepsze wyśrodkowanie)
      myOLED.print(CodeChars[HackMeGame.GameMatrix[row][col]]);
    }
  }
}

void displayBombExplosion() {
  myOLED.clearDisplay();
  myOLED.setTextSize(1);
  myOLED.drawBitmap(32, -1, WYBUCH, 64, 64, SH110X_WHITE);
  for (uint8_t i=0; i < 3; i++){
    if (soundEnabled) tone(BUZZER_PIN, 600, DELAY100MS);
    myOLED.fillRect(0, 0, 127, 63, SH110X_INVERSE);
    myOLED.display();
    delay(DELAY100MS);
    noTone(BUZZER_PIN);
    myOLED.fillRect(0, 0, 127, 63, SH110X_INVERSE);
    myOLED.display();
    delay(DELAY100MS);
  }
  if (soundEnabled) tone(BUZZER_PIN, 300, DELAY500MS);
  delay(DELAY500MS);
  noTone(BUZZER_PIN);
}

void SelectRow_y(HackMeStruct& HackMeGame, uint8_t row) {
  myOLED.fillRect(1, 11*HackMeGame.SelectedRow-1, 79, 9, SH110X_INVERSE);
  myOLED.fillRect(1, 11*row-1, 79, 9, SH110X_INVERSE);
}

void SelectCol_x(HackMeStruct& HackMeGame, uint8_t col) {
  myOLED.fillRect(1+(HackMeGame.SelectedCol)*14, 0, 9, 63, SH110X_INVERSE);
  myOLED.fillRect(1+14*col, 0, 9, 63, SH110X_INVERSE);
}

void GenerateCode(HackMeStruct& HackMeGame, uint8_t ileZnakow) {
  uint8_t indeks = 0;
  for (uint8_t znakIndex=0; znakIndex < ileZnakow; znakIndex++) {
    bool done = false;
    switch(znakIndex){
      case 0:
        HackMeGame.CodeGenerated_xy[0][1] = random(0, 6); //30
        HackMeGame.CodeGenerated_xy[0][0] = 0;
        HackMeGame.CodeSignsCodes[0] = HackMeGame.GameMatrix[HackMeGame.CodeGenerated_xy[0][0]][HackMeGame.CodeGenerated_xy[0][1]];
        break;
      case 1:
        HackMeGame.CodeGenerated_xy[1][1] = HackMeGame.CodeGenerated_xy[0][1];
        HackMeGame.CodeGenerated_xy[1][0] = random(1,6);
        HackMeGame.CodeSignsCodes[1] = HackMeGame.GameMatrix[HackMeGame.CodeGenerated_xy[1][0]][HackMeGame.CodeGenerated_xy[1][1]];
        break;
      case 2:
        HackMeGame.CodeGenerated_xy[2][0] = HackMeGame.CodeGenerated_xy[1][0];
        while(done==false){
          indeks = random(0,6);
          if (indeks != HackMeGame.CodeGenerated_xy[1][1]) done=true;
        }
        HackMeGame.CodeGenerated_xy[2][1] = indeks;
        HackMeGame.CodeSignsCodes[2] = HackMeGame.GameMatrix[HackMeGame.CodeGenerated_xy[2][0]][HackMeGame.CodeGenerated_xy[2][1]];
        break;
      case 3:
        while(done==false){
          indeks = random(0,6);
          if (indeks != HackMeGame.CodeGenerated_xy[2][0]) done=true;
        }
        HackMeGame.CodeGenerated_xy[3][1] = HackMeGame.CodeGenerated_xy[2][1];
        HackMeGame.CodeGenerated_xy[3][0] = indeks;
        HackMeGame.CodeSignsCodes[3] = HackMeGame.GameMatrix[HackMeGame.CodeGenerated_xy[3][0]][HackMeGame.CodeGenerated_xy[3][1]];
        break;
      default:
        break;
    }
  }
}

void mixCode(HackMeStruct& HackMeGame,uint8_t ileZnakow) {
  int digits[4] = {1, 2, 3, 4};
  // Tasowanie tablicy (algorytm Fishera-Yatesa)
  for (int i = 3; i > 0; i--) {
    int j = random(0, i + 1);
    int temp = digits[i];
    digits[i] = digits[j];
    digits[j] = temp;
  }

  for(int i=0; i<CodeWidth; i++) {
    int k = digits[i]-1;
    HackMeGame.CodeSignsCodesMixed[i].CodeSignsCodesIndex = HackMeGame.CodeSignsCodes[k]; // tablica przechowujaca indeksy znakow kodu w losowej kolejnosci
    HackMeGame.CodeSignsCodesMixed[i].matched = false;
  }
}

void mixedCodeDisplay (HackMeStruct& HackMeGame){
  myOLED.fillRect(88, 0, 40, 40, SH110X_BLACK);
  for (int i=0; i<CodeWidth; i++) {
    myOLED.setTextSize(1);
    myOLED.setCursor(93,11*i);
    int k = HackMeGame.CodeSignsCodesMixed[i].CodeSignsCodesIndex;
    myOLED.print(CodeChars[k]);
    myOLED.display();
  }
}

// Funkcja wyboru pierwszej cyfry kodu: użytkownik wybiera kolumnę w pierwszym wierszu
bool FirstDigitIsWrong(HackMeStruct& HackMeGame, timerStruct& Timer1Sec) {
  int prevCol = HackMeGame.SelectedCol; // zawsze ta sama kolumna
  int prevRow = HackMeGame.SelectedRow;
  while(ReadButton(Timer_Interrupt, Timer1Sec) != NONE);

  while(!Timer1Sec.timeOut) {
    int selectedCol = HackMeGame.SelectedCol;
    btPressedCode button;
    while (ReadButton(Timer_Interrupt, Timer1Sec) == NONE) {
      ReadButton(Timer_Interrupt, Timer1Sec);
      if (checkForTimeoutAndDisplayCurrTime(Timer1Sec)) return true;
    }
    button = ReadButton(Timer_Interrupt, Timer1Sec);
    while (ReadButton(Timer_Interrupt, Timer1Sec) != NONE); // czekaj na puszczenie przycisku

    switch (button) {
      case UpLeft:
        if (HackMeGame.SelectedCol == 0) break;
        HackMeGame.SelectedCol--;
        break;
      case DownLeft:
        if (HackMeGame.SelectedCol == ArrayMaxRowColIndex) break;
        HackMeGame.SelectedCol++;
        break;
      case DownRight:
        if (DigitIsWrong(HackMeGame)) {
          return true;
        } else return false;
        break;
      default:
        break;
    }
    if (selectedCol != HackMeGame.SelectedCol) {
      SelectCol_x(HackMeGame, selectedCol);
    }
    myOLED.display();
    while (ReadButton(Timer_Interrupt, Timer1Sec) != NONE);
  }
  return true;
}

// Funkcja sprawdzająca drugi znak: user wybiera wiersz, kolumna ustalona
bool SecondDigitIsWrong(HackMeStruct& HackMeGame, timerStruct& Timer1Sec) {
  int prevCol = HackMeGame.SelectedCol; // zawsze ta sama kolumna
  int prevRow = HackMeGame.SelectedRow;
  while(!Timer1Sec.timeOut) {
    int selectedRow = HackMeGame.SelectedRow;
    btPressedCode button;
    while (ReadButton(Timer_Interrupt, Timer1Sec) == NONE) {
      ReadButton(Timer_Interrupt, Timer1Sec);
      if (checkForTimeoutAndDisplayCurrTime(Timer1Sec)) return true;
    }
    button = ReadButton(Timer_Interrupt, Timer1Sec);;
    switch (button){
      case DownLeft:
        if (HackMeGame.SelectedRow == ArrayMaxRowColIndex) break;
        HackMeGame.SelectedRow++;
        break;
      case UpLeft:
        if (HackMeGame.SelectedRow == 0) break;
        HackMeGame.SelectedRow--;
        break;
      case DownRight:
        if (HackMeGame.SelectedRow == prevRow)  continue; // ignoruj próbę wyboru tej samej pozycji
        if (DigitIsWrong(HackMeGame)) {
          return true;
        } else return false;
        break;
      default:
        break;
    }
    if (selectedRow != HackMeGame.SelectedRow) {
      SelectRow_y(HackMeGame, selectedRow);
    }
    myOLED.display();
    while(ReadButton(Timer_Interrupt, Timer1Sec) != NONE);
  }
  return true;
}

 // Funkcja wyboru trzeciego znaku: user wybiera kolumnę, nie może wybrać tej samej co poprzednio
bool ThirdDigitIsWrong(HackMeStruct& HackMeGame, timerStruct& Timer1Sec) {
  int prevCol = HackMeGame.SelectedCol; // zawsze ta sama kolumna
  int prevRow = HackMeGame.SelectedRow;
  while(!Timer1Sec.timeOut) {
    int selectedCol = HackMeGame.SelectedCol;
    btPressedCode button;
    while (ReadButton(Timer_Interrupt, Timer1Sec) == NONE) {
      ReadButton(Timer_Interrupt, Timer1Sec);
      if (checkForTimeoutAndDisplayCurrTime(Timer1Sec)) return true;
    }
    button = ReadButton(Timer_Interrupt, Timer1Sec);
    switch (button){
      case UpLeft:
        if (HackMeGame.SelectedCol == 0) break;
        HackMeGame.SelectedCol--;
        break;
      case DownLeft:
        if (HackMeGame.SelectedCol == ArrayMaxRowColIndex) break;
        HackMeGame.SelectedCol++;
        break;
      case DownRight:
        if (HackMeGame.SelectedCol == prevCol) continue; // ignoruj próbę wyboru tej samej pozycji
        if (DigitIsWrong(HackMeGame)) {
          return true;
        } else return false;
        break;
      default:
        break;
    }
    if (selectedCol != HackMeGame.SelectedCol) SelectCol_x(HackMeGame, selectedCol);
    myOLED.display();
    while(ReadButton(Timer_Interrupt, Timer1Sec) != NONE);
  }
  return true;
}

// Funkcja wyboru czwartego znaku: user wybiera wiersz, nie może wybrać tego samego co poprzednio
bool FourthDigitIsWrong(HackMeStruct& HackMeGame, timerStruct& Timer1Sec) {
  int prevCol = HackMeGame.SelectedCol; // zawsze ta sama kolumna
  int prevRow = HackMeGame.SelectedRow;
  while(!Timer1Sec.timeOut) {
    int selectedRow = HackMeGame.SelectedRow;

    btPressedCode button;
    while (ReadButton(Timer_Interrupt, Timer1Sec) == NONE) {
      ReadButton(Timer_Interrupt, Timer1Sec);
      if (checkForTimeoutAndDisplayCurrTime(Timer1Sec)) return true;
    }
    button = ReadButton(Timer_Interrupt, Timer1Sec);
    switch (button){
      case UpLeft:
        if (HackMeGame.SelectedRow == 0) break;
        HackMeGame.SelectedRow--;
        break;
      case DownLeft:
        if (HackMeGame.SelectedRow == ArrayMaxRowColIndex) break;
        HackMeGame.SelectedRow++;
        break;
      case DownRight:
        if (HackMeGame.SelectedRow == prevRow) continue; // ignoruj próbę wyboru tej samej pozycji
        if (DigitIsWrong(HackMeGame)) {
          return true;
        } else return false;
        break;
      default:
        break;
    }
    if (selectedRow != HackMeGame.SelectedRow) {
      SelectRow_y(HackMeGame, selectedRow);
    }
    myOLED.display();
    while(ReadButton(Timer_Interrupt, Timer1Sec) != NONE);
  }
  return true;
}

bool DigitIsWrong(HackMeStruct& HackMeGame){
  for (int i=0; i<CodeWidth; i++) {
    if (HackMeGame.CodeSignsCodesMixed[i].matched == false) {// jeśli ten znak jeszcze nie został dopasowany
      if (HackMeGame.CodeSignsCodesMixed[i].CodeSignsCodesIndex == HackMeGame.GameMatrix[HackMeGame.SelectedRow][HackMeGame.SelectedCol]) {
        // Dopasowanie znalezione
        HackMeGame.CodeSignsCodesMixed[i].matched = true;
        myOLED.setTextSize(1);
        myOLED.setCursor(93,11*(i));
        myOLED.fillRect(93,11*(i), 8, 8, SH110X_BLACK);
        myOLED.display();
        if (soundEnabled) {
          tone(BUZZER_PIN, TON_KONIECGRY_CZAS, DELAY100MS);
          delay(DELAY100MS);
          noTone(BUZZER_PIN);
        } else delay(DELAY100MS);
        return false; // poprawny wybór
      }
    }
  }
  errorSound();
  return true; // brak dopasowania
}

void displayResolution(HackMeStruct& HackMeGame) {
  myOLED.clearDisplay();
  displayMatrix(HackMeGame);
  for (int i=0; i < 6; i++) {
    myOLED.setCursor(115, 11*i);
    myOLED.print(CodeString[i]);
  }

  for (int znakIndex=0; znakIndex < CodeWidth; znakIndex++) {
    int row = HackMeGame.CodeGenerated_xy[znakIndex][0];
    int col = HackMeGame.CodeGenerated_xy[znakIndex][1];
    for (int blink=0; blink<3; blink++) {
      // Inwersja (podświetlenie)
      myOLED.setTextColor(SH110X_BLACK, SH110X_WHITE);
      myOLED.setCursor(3+14*col, 11*row);
      myOLED.print(CodeChars[HackMeGame.GameMatrix[row][col]]);
      myOLED.setTextColor(SH110X_WHITE, SH110X_BLACK);
      myOLED.setCursor(98, 11+11*znakIndex);
      myOLED.print(CodeChars[HackMeGame.GameMatrix[row][col]]);
      myOLED.display();
      delay(300);
      // Wyłączenie podświetlenia (czarny znak na czarnym tle w tablicy)
      myOLED.setTextColor(SH110X_WHITE, SH110X_BLACK);
      myOLED.setCursor(3+14*col, 11*row);
      myOLED.print(CodeChars[HackMeGame.GameMatrix[row][col]]);
      myOLED.setTextColor(SH110X_WHITE, SH110X_BLACK);
      myOLED.setCursor(98, 11+11*znakIndex);
      myOLED.print(CodeChars[HackMeGame.GameMatrix[row][col]]);
      myOLED.display();
      delay(200);
    }
    // Po miganiu zostaw inwersję w tablicy i białe z boku
    myOLED.setTextColor(SH110X_BLACK, SH110X_WHITE);
    myOLED.setCursor(3+14*col, 11*row);
    myOLED.print(CodeChars[HackMeGame.GameMatrix[row][col]]);
    myOLED.setTextColor(SH110X_WHITE, SH110X_BLACK);
    myOLED.setCursor(98, 11+11*znakIndex);
    myOLED.print(CodeChars[HackMeGame.GameMatrix[row][col]]);
    myOLED.setTextColor(SH110X_WHITE);
    myOLED.display();
    delay(800);
  }
  myOLED.display();
  delay(DELAY500MS);
  HackMeGame.newGame = true;
  while (ReadButton(nullptr, Timer1Sec) == NONE);
  myOLED.clearDisplay();
  myOLED.setCursor(10, 25);
  myOLED.println(F("Czas na nowa gre!"));
  myOLED.display();
  delay(DELAY1000MS);
}

void errorSound() {
  if (!soundEnabled) return;
  for (int j=0; j<3; j++) {
    tone(BUZZER_PIN, TON_RAMKA_FREQ, DELAY250MS);
    delay(DELAY50MS);
    noTone(BUZZER_PIN);
    delay(DELAY50MS);
  }
}

void WelcomeHackMeScreen() {
  myOLED.clearDisplay();
  myOLED.setTextColor(SH110X_WHITE);
  myOLED.setTextSize(2);
  myOLED.setCursor(2, 1);
  myOLED.println("Disarm it!");
  myOLED.setTextSize(1);
  myOLED.setCursor(0, 20);
  myOLED.println("o > select row/col");
  myOLED.setCursor(0, 31);

  myOLED.println("o > select row/col");
  myOLED.setCursor(45, 44);
  myOLED.println("      --- > o");
  myOLED.setCursor(45, 55);

  myOLED.println("    Enter > o");
  myOLED.display();
  delay(DELAY1000MS);
} 

void GenerateRandomCodeMatrix(HackMeStruct& HackMeGame) {
  uint8_t randomLoopCount = random(5,20);
  for (int i=0; i<randomLoopCount; i++) {
    myOLED.clearDisplay();
    randomMatrix(HackMeGame);
    displayMatrix(HackMeGame);
    myOLED.display();
  }
  while (ReadButton(nullptr, Timer1Sec) != NONE);
  while (ReadButton(nullptr, Timer1Sec) == NONE) {
    myOLED.clearDisplay();
    randomMatrix(HackMeGame);
    displayMatrix(HackMeGame);
    myOLED.display();
  }
}

void hdisplaySoundInfo() {
  if (!soundEnabled) {
    myOLED.setTextColor(SH110X_BLACK);
    myOLED.setCursor(88, 52);
    myOLED.print("!");
  } else {
    myOLED.setTextColor(SH110X_WHITE);
    myOLED.setCursor(88, 52);
    myOLED.print("!");
  }
  myOLED.setTextColor(SH110X_WHITE);
  myOLED.display();
}

void Timer_Interrupt(timerStruct& t) {
  if (millis()-t.prevTimeCheck > (t.interval)) {
    t.prevTimeCheck  = millis();
    if (t.currTime > 0) {
      t.currTime--;
      if (t.currTime == 0) t.timeOut = true;
    }
  }
}

bool checkForTimeoutAndDisplayCurrTime(timerStruct& Timer1Sec) {
  int lineStart = 55;
  int lineEnd = lineStart - Timer1Sec.currTime;
  myOLED.drawLine(120, lineStart, 120, lineStart-timeForCodeBreaking, SH110X_BLACK);

  myOLED.fillRect(118, lineEnd, 6, 6, SH110X_BLACK);
  myOLED.drawLine(120, lineStart, 120, lineEnd, SH110X_WHITE);
  myOLED.setTextColor(SH110X_WHITE);
  myOLED.setCursor(116, 0);
  myOLED.fillRect(110, 0, 20, 20, SH110X_BLACK);
  myOLED.print(Timer1Sec.currTime);
  myOLED.setCursor(116, lineEnd);
  if (Timer1Sec.currTime > 4) {
    myOLED.setCursor(118, lineEnd);
  } else myOLED.setCursor(112+Timer1Sec.currTime, lineEnd);
  myOLED.print("*");
  myOLED.drawBitmap(104, 48, BOMB, 17, 15, SH110X_WHITE);
  myOLED.display();
  if (Timer1Sec.timeOut) {
    return true;
  }
  return false;
}

void selectFirstRowCol(HackMeStruct& HackMeGame) {
  myOLED.fillRect(1,-1, 79, 9, SH110X_INVERSE);
  myOLED.fillRect(1,0, 9, 63, SH110X_INVERSE);
  HackMeGame.SelectedRow = 0;
  HackMeGame.SelectedCol = 0;
}

} // namespace HackMeGame

const GameInfo GameInfo_HackMe = {
  "Disarm the bomb",
  "Rozbroj bombe wbijajac kod",
  HackMeGame::Game_HackMe,
  -1  // HackMe nie zapisuje highscore
};