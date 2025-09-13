#include "HackMe.h"
#include "HackMe_private.h"


void Game_HackMe() {
  HackMeStruct HackMeGame;

  Timer1Sec.currTime = TimerInits;
  Timer1Sec.prevTimeCheck = millis();
  delay(DELAY100MS);

  WelcomeHackMeScreen();
  while (ReadButton(nullptr, Timer1Sec) == NONE) ;
  while (ReadButton(nullptr, Timer1Sec) != NONE) ;

  while(1) {
    btPressedCode ButtonCode;
    HackMeGame.gameRestart = false;
    HackMeGame.gameEnded = false;
    if (Timer1Sec.timeOut) {
      Timer1Sec.timeOut = false;
      Timer1Sec.currTime = TimerInits;
      displayBombExplosion();
      displayResolution(HackMeGame);
    }

    while (ReadButton(nullptr, Timer1Sec) != DownRight)  GenerateRandomScreen(HackMeGame);
    selectFirstRowCol(HackMeGame);
    GenerateCode(HackMeGame, codewidth); // wygenerowana tablica czas wygenerowac code
    mixCode(HackMeGame, codewidth);
    mixedCodeDisplay(HackMeGame);
    delay(100);
    while (ReadButton(nullptr, Timer1Sec) == DownRight);
    Timer1Sec.currTime = TimerInits;
    Timer1Sec.timeOut = false;

    while (!HackMeGame.gameEnded) {
      displaySound();
      while(1==1) {
        displaySound();
        if (HackMeGame.gameRestart) {
          myOLED.clearDisplay();
          displayMatrix(HackMeGame);
          selectFirstRowCol(HackMeGame);

          mixedCodeDisplay(HackMeGame);
          for (int i=0; i<codewidth; i++) (HackMeGame.CodeSignsCodesMixed[i].matched = false);
          myOLED.display();
          HackMeGame.gameRestart = false;
        }
        if (!checkedFirstNo(HackMeGame,Timer1Sec)) {checkIfEndOrRestartGame(HackMeGame,Timer1Sec); break;}
        delay(DELAY250MS);
        if (!checkedSecondNo(HackMeGame,Timer1Sec)) {checkIfEndOrRestartGame(HackMeGame,Timer1Sec); break;}
        delay(DELAY250MS);
        if (!checkedThirdNo(HackMeGame,Timer1Sec)) {checkIfEndOrRestartGame(HackMeGame,Timer1Sec); break;}
        delay(DELAY250MS);
        if (!checkedFourthNo(HackMeGame,Timer1Sec)) {checkIfEndOrRestartGame(HackMeGame,Timer1Sec); break;}

        Timer1Sec.currTime = TimerInits;
        for (int i=0; i<codewidth; i++) {
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
        HackMeGame.gameEnded = true;
        break;
      }
    }
  }
}


///$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
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
        myOLED.setCursor(3+14*col, 11*row); // x: kolumna, y: wiersz
        myOLED.print(CodeChars[HackMeGame.GameMatrix[row][col]]);
      }
    }
  }

  void displayBombExplosion() {
    myOLED.clearDisplay();
    myOLED.drawBitmap(32, -1, WYBUCH, 64, 64, SH110X_WHITE);
    for (uint8_t i=0; i<codewidth; i++){
      if (soundEnabled) tone(BUZZER_PIN, 600, 100);
      myOLED.fillRect(0, 0, 127, 63, SH110X_INVERSE);
      myOLED.display();
      delay(DELAY100MS);
      noTone(BUZZER_PIN);
      myOLED.fillRect(0, 0, 127, 63, SH110X_INVERSE);
      myOLED.display();
      delay(DELAY100MS);
    }
    if (soundEnabled) tone(BUZZER_PIN, 300, 500);
    delay(DELAY500MS);
    noTone(BUZZER_PIN);
    myOLED.clearDisplay();
    myOLED.setCursor(10, 25);
    myOLED.setTextSize(1);
    myOLED.println(F("Czas na nowa gre!"));
    myOLED.display();
    delay(4*DELAY500MS);
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
            indeks=random(0,6);
            if (indeks != HackMeGame.CodeGenerated_xy[1][1]) done=true;
          }
          HackMeGame.CodeGenerated_xy[2][1] = indeks;
          HackMeGame.CodeSignsCodes[2] = HackMeGame.GameMatrix[HackMeGame.CodeGenerated_xy[2][0]][HackMeGame.CodeGenerated_xy[2][1]];
          break;
        case 3:
          while(done==false){
            indeks=random(0,6);
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

    for(int i=0; i<codewidth; i++) {
      int k = digits[i]-1;
      HackMeGame.CodeSignsCodesMixed[i].CodeSignsCodesIndex = HackMeGame.CodeSignsCodes[k]; // tablica przechowujaca indeksy znakow kodu w losowej kolejnosci
      HackMeGame.CodeSignsCodesMixed[i].matched = false;
    }
  }

  void mixedCodeDisplay (HackMeStruct& HackMeGame){
    myOLED.fillRect(88, 0, 40, 40, SH110X_BLACK);
    for (int i=0; i<codewidth; i++) {
      myOLED.setTextSize(1);
      myOLED.setCursor(93,11*i);
      int k = HackMeGame.CodeSignsCodesMixed[i].CodeSignsCodesIndex;
      myOLED.print(CodeChars[k]);
      myOLED.display();
    }
  }

  // Funkcja wyboru pierwszej cyfry kodu: użytkownik wybiera kolumnę w pierwszym wierszu
bool checkedFirstNo(HackMeStruct& HackMeGame, timerStruct& Timer1Sec) {
  HackMeGame.SelectedRow = 0;
  HackMeGame.SelectedCol = 0;
  int prevCol = HackMeGame.SelectedCol; // zawsze ta sama kolumna
  int prevRow = HackMeGame.SelectedRow;
  while(ReadButton(Timer_Interrupt, Timer1Sec) != NONE);

  while(!Timer1Sec.timeOut) {
    int selectedCol = HackMeGame.SelectedCol;
    btPressedCode button;
    while (ReadButton(Timer_Interrupt, Timer1Sec) == NONE) {
      ReadButton(Timer_Interrupt, Timer1Sec);; // czekaj na wciśnięcie przycisku
      if (itsTimeoutAndDisplayCurrTime(Timer1Sec)) return false;
    }

    button = ReadButton(Timer_Interrupt, Timer1Sec);;
    while (ReadButton(Timer_Interrupt, Timer1Sec) != NONE); // czekaj na puszczenie przycisku

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
        if (!checkCode(HackMeGame)) {
          errorSound();
          return false;
        } else return true;
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
  return false;   
}

// Funkcja sprawdzająca drugi znak: user wybiera wiersz, kolumna ustalona
bool checkedSecondNo(HackMeStruct& HackMeGame, timerStruct& Timer1Sec) {
  int prevCol = HackMeGame.SelectedCol; // zawsze ta sama kolumna
  int prevRow = HackMeGame.SelectedRow;
  while(true) {
    int selectedRow = HackMeGame.SelectedRow;
    btPressedCode button;
    while (ReadButton(Timer_Interrupt, Timer1Sec) == NONE) {
      ReadButton(Timer_Interrupt, Timer1Sec);; // czekaj na wciśnięcie przycisku
      if (itsTimeoutAndDisplayCurrTime(Timer1Sec)) return false;
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
        if (!checkCode(HackMeGame)) {
          errorSound();
          return false;
        } else return true;
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
}

 // Funkcja wyboru trzeciego znaku: user wybiera kolumnę, nie może wybrać tej samej co poprzednio
bool checkedThirdNo(HackMeStruct& HackMeGame, timerStruct& Timer1Sec) {
  int prevCol = HackMeGame.SelectedCol; // zawsze ta sama kolumna
  int prevRow = HackMeGame.SelectedRow;
  while(true) {
    int selectedCol = HackMeGame.SelectedCol;
    btPressedCode button;
    while (ReadButton(Timer_Interrupt, Timer1Sec) == NONE) {
      ReadButton(Timer_Interrupt, Timer1Sec);; // czekaj na wciśnięcie przycisku
      if (itsTimeoutAndDisplayCurrTime(Timer1Sec)) return false;
    }
    button = ReadButton(Timer_Interrupt, Timer1Sec);;
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
        if (!checkCode(HackMeGame)) {
          errorSound();
          return false;
        } else return true;
        break;
      default:
        break;
    }
    if (selectedCol != HackMeGame.SelectedCol) SelectCol_x(HackMeGame, selectedCol);
    myOLED.display();
    while(ReadButton(Timer_Interrupt, Timer1Sec) != NONE);
  }
}

// Funkcja wyboru czwartego znaku: user wybiera wiersz, nie może wybrać tego samego co poprzednio
bool checkedFourthNo(HackMeStruct& HackMeGame, timerStruct& Timer1Sec) {
  int prevCol = HackMeGame.SelectedCol; // zawsze ta sama kolumna
  int prevRow = HackMeGame.SelectedRow;
  while(true) {
    int selectedRow = HackMeGame.SelectedRow;

    btPressedCode button;
    while (ReadButton(Timer_Interrupt, Timer1Sec) == NONE) {
      ReadButton(Timer_Interrupt, Timer1Sec);; // czekaj na wciśnięcie przycisku
      if (itsTimeoutAndDisplayCurrTime(Timer1Sec)) return false;
    }
    button = ReadButton(Timer_Interrupt, Timer1Sec);;
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
        if (!checkCode(HackMeGame)) {
          errorSound();
          return false;
        } else return true;
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
}

bool checkCode(HackMeStruct& HackMeGame){
  for (int i=0; i<codewidth; i++) {
    if (HackMeGame.CodeSignsCodesMixed[i].matched == false) // jeśli ten znak jeszcze nie został dopasowany
      if (HackMeGame.CodeSignsCodesMixed[i].CodeSignsCodesIndex == HackMeGame.GameMatrix[HackMeGame.SelectedRow][HackMeGame.SelectedCol]) {
        // Dopasowanie znalezione
        HackMeGame.CodeSignsCodesMixed[i].matched = true;
        myOLED.setTextSize(1);
        myOLED.setCursor(93,11*(i));
        //myOLED.print("X");
        myOLED.fillRect(93,11*(i), 8, 8, SH110X_BLACK);
        myOLED.display();
        if (soundEnabled) {
          tone(BUZZER_PIN, TON_KONIECGRY_CZAS, DELAY100MS);
          delay(DELAY100MS);
          noTone(BUZZER_PIN);
        } else delay(DELAY100MS);
        return true; // poprawny wybór
      }
    }
  errorSound();
  return false; // brak dopasowania
}

void displayResolution(HackMeStruct& HackMeGame) {
  myOLED.clearDisplay();
  displayMatrix(HackMeGame);
  for (int znakIndex=0; znakIndex < codewidth; znakIndex++) {
    int row = HackMeGame.CodeGenerated_xy[znakIndex][0];
    int col = HackMeGame.CodeGenerated_xy[znakIndex][1];
    myOLED.setCursor(98,11+11*znakIndex);
    //myOLED.print(row); myOLED.print(','); myOLED.print(col);
    //myOLED.print(".");
    myOLED.print(CodeChars[HackMeGame.GameMatrix[row][col]]);
  }
  for (int i=0; i < 6; i++) {
    myOLED.setCursor(120, 11*i);
    myOLED.print(CodeString[i]);
  }
  myOLED.display();
  delay(DELAY500MS);
  HackMeGame.gameEnded = true;
  while (ReadButton(nullptr, Timer1Sec) == NONE);
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
  myOLED.setTextSize(1);

  myOLED.setCursor(0, 10);
  myOLED.println(F("Hack Me If You Can!"));
  myOLED.setCursor(20, 30);
  myOLED.println(F("Press any key.."));
  myOLED.display();
} 

void GenerateRandomScreen(HackMeStruct& HackMeGame) {
  myOLED.clearDisplay();
  randomMatrix(HackMeGame);
  displayMatrix(HackMeGame);
  myOLED.display();
}

void displaySound() {
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



bool itsTimeoutAndDisplayCurrTime(timerStruct& Timer1Sec) {
  int lineStart = 55;
  int lineEnd = lineStart - Timer1Sec.currTime;
  myOLED.drawLine(120, lineStart, 120, lineStart-TimerInits, SH110X_BLACK);

  // Zamaluj poprzednią gwiazdkę (czarny prostokąt 6x8 px)
  myOLED.fillRect(118, lineEnd, 6, 6, SH110X_BLACK);
  // Rysuj białą linię od góry do aktualnej pozycji
  myOLED.drawLine(120, lineStart, 120, lineEnd, SH110X_WHITE);
  // Rysuj gwiazdkę na końcu białej linii
  myOLED.setTextColor(SH110X_WHITE);
  myOLED.setCursor(116, 0);
  myOLED.fillRect(110, 0, 20, 20, SH110X_BLACK);
  myOLED.print(Timer1Sec.currTime);
  myOLED.setCursor(116, lineEnd);
  if (Timer1Sec.currTime > 4) {
    myOLED.setCursor(118, lineEnd);
  } else myOLED.setCursor(112+Timer1Sec.currTime, lineEnd);
  myOLED.print("*");
  myOLED.display();
  if (Timer1Sec.timeOut) {
    return true;
  }
  return false;
}

void selectFirstRowCol(HackMeStruct& HackMeGame) {
  myOLED.fillRect(1,-1, 79, 9, SH110X_INVERSE);
  myOLED.fillRect(1,0, 9, 63, SH110X_INVERSE);
  myOLED.drawBitmap(104, 48, BOMB, 17, 15, SH110X_WHITE);
  HackMeGame.SelectedRow = 0;
  HackMeGame.SelectedCol = 0;
}
void checkIfEndOrRestartGame(HackMeStruct& HackMeGame, timerStruct& Timer1Sec) {
  if (Timer1Sec.timeOut) {
    HackMeGame.gameEnded = true;
  } else {
    HackMeGame.gameRestart = true;
  }
}

const GameInfo GameInfo_HackMe = {
  "HackMe",
  "Rozbroj bombe wbijajac kod",
  Game_HackMe
};