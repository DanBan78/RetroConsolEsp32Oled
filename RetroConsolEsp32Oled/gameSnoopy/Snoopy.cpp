#include "Snoopy.h"
#include "Snoopy_private.h"

void Game_Snoopy() {
	snoopyStr Snoopy;
  eggLineStr allEggLines[4];

  Snoopy.sesionScore = 0;
  GameLoop (Snoopy, allEggLines);
}

void GameLoop (snoopyStr& Snoopy, eggLineStr allEggLines[4]) {
  while (true) {
    myOLED.clearDisplay();
    SnoopyInit(Snoopy, allEggLines);
    DrawSnoopy(Snoopy.direction, SH110X_BLACK);

    AddNewEgg(allEggLines);
    myOLED.display();
    Snoopy.prevUpdateTime  = millis();

    while (Snoopy.lives!=0) {
      displaySound(120, 0, soundEnabled);
      if (ItsTimeForFrameUpdate(Snoopy)) {
        UpdateEggsOnScreen(Snoopy, allEggLines);
        for (int i = 0; i < 4; i++) CheckIfEggCatchOrNot(Snoopy, allEggLines[i]);
        if (Snoopy.lives == 0) break;
        AddNewEggOrUpdateSpeed(Snoopy, allEggLines);
      }
      SnoopyDisplayScore(Snoopy);
      CheckIfSnopyMoved(Snoopy);
      myOLED.display();
    }
  }
}

void AddNewEgg(eggLineStr allEggLines[4]) {
  orientation newEggOnLine = (orientation)random(0, 4);
  allEggLines[newEggOnLine].position |= 1;
}

void CheckIfSnopyMoved(snoopyStr& Snoopy) {
  orientation prevPos = Snoopy.direction;

  if (IsPressed(UpLeft)) Snoopy.direction = sLeftUp;
  if (IsPressed(DownLeft)) Snoopy.direction = sLeftDown;
  if (IsPressed(UpRight)) Snoopy.direction = sRightUp; 
  if (IsPressed(DownRight)) Snoopy.direction = sRightDown;
  
  DrawSnoopy(prevPos, SH110X_BLACK);
  DrawSnoopy(Snoopy.direction, SH110X_WHITE);
}

void ForceSnoopyDraw(orientation SnoopyPos, orientation Pos) {
  DrawSnoopy(SnoopyPos, SH110X_BLACK);
  DrawSnoopy(Pos, SH110X_WHITE);
}

void DrawSnoopy(orientation Pos, uint16_t color) {
    switch(Pos) {
    case sLeftDown:
      myOLED.drawBitmap(31, 13, SnoopyLD, 50, 50, color);
      break;
    case sLeftUp:
      myOLED.drawBitmap(31, 10, SnoopyLU, 50, 50, color);
      break;
    case sRightDown:
      myOLED.drawBitmap(43, 13, SnoopyRD, 50, 50, color);
      break;
    case sRightUp:
      myOLED.drawBitmap(43, 10, SnoopyRU, 50, 50, color);
      break;
    case sWaiting:
      myOLED.drawBitmap(37, 10, SnoopySit, 50, 50, color);
      break;
    case sResting:
      myOLED.drawBitmap(37, 10, SnoopyNap, 50, 50, color);
      break;
  }
}

void SnoopyDisplayScore(snoopyStr& Snoopy) {
  myOLED.fillRect(0, 0, 60,12, SH110X_BLACK);
  myOLED.setTextColor(SH110X_WHITE);
  myOLED.setTextSize(1);
  myOLED.setCursor(0,0);
  myOLED.print(F("Eggs: "));
  myOLED.print(Snoopy.score);
}

void UpdateEggsOnScreen(snoopyStr& Snoopy, eggLineStr allEggLines[4]) {
  for (int i = 0; i < 4; i++) {
    DrawEggs(Snoopy, allEggLines[i], 0);
    allEggLines[i].position = allEggLines[i].position << 1;
    DrawEggs(Snoopy, allEggLines[i], 1);
  }
}

void DrawEggs(snoopyStr& Snoopy, eggLineStr& eggLine, uint16_t color){
  for (int i=0; i<6; i++) {
    if ((eggLine.position & (1 << i)) != 0) {
      int x=0+i*7; 
      int y=18+i;
      switch(eggLine.direction) {
        case sLeftUp:
          myOLED.drawCircle(x, y, 2, color);
        break;
        case sLeftDown:
          myOLED.drawCircle(x, y+20, 2, color);
        break;
        case sRightUp:
          myOLED.drawCircle(127-x, y, 2, color);
        break;
        case sRightDown:
          myOLED.drawCircle(127-x, y+20, 2, color);
        break;
      }
    }
  }
}

bool ItsTimeForFrameUpdate(snoopyStr& Snoopy) {
  if (millis()- Snoopy.prevUpdateTime > (Snoopy.frameUpdateRate)) {
    Snoopy.prevUpdateTime  = millis();
    return true;
  }
  return false;
}

void SnoopyInit(snoopyStr& Snoopy, eggLineStr allEggLines[4]) {
  myOLED.clearDisplay();
  myOLED.setTextColor(SH110X_WHITE);
  myOLED.setTextSize(1);
  myOLED.setCursor(10, 1);
  //draw lives circles
  myOLED.drawCircle(2, 58, 2, SH110X_WHITE);
  myOLED.drawCircle(10, 58, 2, SH110X_WHITE);
  myOLED.drawCircle(18, 58, 2, SH110X_WHITE);
  //draw lines
  myOLED.drawLine(0,21, 30, 26, SH110X_WHITE);
  myOLED.drawLine(0,22, 30, 27, SH110X_WHITE);
  myOLED.drawLine(0,21+20, 30, 26+20, SH110X_WHITE);
  myOLED.drawLine(0,22+20, 30, 27+20, SH110X_WHITE);
  myOLED.drawLine(127,21, 123-30, 26, SH110X_WHITE);
  myOLED.drawLine(127,22, 123-30, 27, SH110X_WHITE);
  myOLED.drawLine(127,21+20,123- 30, 26+20, SH110X_WHITE);
  myOLED.drawLine(127,22+20,123- 30, 27+20, SH110X_WHITE);
  DrawSnoopy(Snoopy.direction, SH110X_WHITE);
  // init values
  Snoopy.lives = 3;
  Snoopy.score = 0;
  Snoopy.direction   = sWaiting;
  Snoopy.frameUpdateRate = Snoopy.initFrameUpdateRate;
  allEggLines[sLeftUp].position    = 0; allEggLines[sLeftUp].direction    = sLeftUp;
  allEggLines[sLeftDown].position  = 0; allEggLines[sLeftDown].direction  = sLeftDown;
  allEggLines[sRightUp].position   = 0; allEggLines[sRightUp].direction   = sRightUp;
  allEggLines[sRightDown].position = 0; allEggLines[sRightDown].direction = sRightDown;
  myOLED.display();
}

void CheckIfEggCatchOrNot(snoopyStr& Snoopy, eggLineStr& eggLine) {
  if ((eggLine.position & 64) == 64) {
    if (Snoopy.direction == eggLine.direction) {
      Snoopy.score ++;
      if(soundEnabled) MyTune(TON_POINT_FREQ, TON_POINT_CZAS);
    } else {
      Snoopy.lives --;
      DisplayBrokenEgg(Snoopy, eggLine.direction);
      if (Snoopy.lives == 0) {
        myOLED.clearDisplay();
        myOLED.setTextColor(SH110X_WHITE);
        myOLED.setTextSize(1);
        myOLED.setCursor(10, 1);
        myOLED.println(F("Koniec gry!"));
        myOLED.setCursor(10, 20);
        myOLED.print(F("Wynik: "));
        myOLED.print(Snoopy.score);
        myOLED.setCursor(10, 40);
        myOLED.display();
        delay(DELAY250MS);
        WaitforButton();
      }
    }
  }
}

void DisplayBrokenEgg(snoopyStr& Snoopy, orientation pos) {
  int x;
  int y = 44; 
  int z = 36;

  if (pos == sRightUp || pos == sRightDown) {
    x = 75;
  } else x = 30;

  if (pos == sLeftUp || pos == sRightUp) {
    myOLED.drawCircle(x + 5, z, 2, SH110X_WHITE);
    myOLED.display();
    delay(DELAY250MS);
  }
  myOLED.drawCircle(x + 5, z, 2, SH110X_BLACK);
  DrawSnoopy(Snoopy.direction, SH110X_BLACK);
  Snoopy.direction = sWaiting;
  DrawSnoopy(Snoopy.direction, SH110X_WHITE);
  myOLED.drawBitmap(x, y, BrokenEgg_bmp, 23, 16, SH110X_WHITE);
  myOLED.display();
  if(soundEnabled) {
    MyTune(TON_ERROR_FREQ, TON_ERROR_CZAS);
  } else delay(TON_ERROR_CZAS);
  
  myOLED.drawBitmap(x, y, BrokenEgg_bmp, 23, 16, SH110X_BLACK);
  myOLED.drawCircle(2 + Snoopy.lives * 8, 58, 2, SH110X_BLACK);
}

void AddNewEggOrUpdateSpeed(snoopyStr& Snoopy, eggLineStr allEggLines[4]) {
  bool eggAdded = false;
  uint8_t totalEggsPos = 0;
  uint8_t eggsOnLinesCount = 0;
  orientation randomLineForNewEgg;

  for (int i = 0; i < 4; i++) {
    eggsOnLinesCount += __builtin_popcount(allEggLines[i].position);
    totalEggsPos |= allEggLines[i].position;
  }
  if ((totalEggsPos > 0) && soundEnabled) MyTune(TON_EGG_FREQ, TON_EGG_CZAS);
  if (Snoopy.score < 7) {
    if (eggsOnLinesCount == 0) AddNewEgg(allEggLines);
    eggAdded = true;
  } else if (Snoopy.score < 20) {
    if (eggsOnLinesCount < 2) {
      if (!(totalEggsPos > 0 && totalEggsPos < 8)) {
        AddNewEgg(allEggLines);
        eggAdded = true;
      }
    }
  } else if ((Snoopy.score > 19) && (eggsOnLinesCount < 3)) {
    while (!eggAdded) {
      randomLineForNewEgg = (orientation)random(0, 4);
      if (allEggLines[randomLineForNewEgg].position > 4 || allEggLines[randomLineForNewEgg].position == 0) {
        allEggLines[randomLineForNewEgg].position |= 1;
        eggAdded = true;
      }
    }
  }
  if (Snoopy.score > 30) Snoopy.frameUpdateRate = Snoopy.initFrameUpdateRate - Snoopy.score/3*2;
  if (eggAdded) {
    for (int i = 0; i < 4; i++) {
      DrawEggs(Snoopy, allEggLines[i], 1);
    }
    myOLED.display();
  }
  return;
}

const GameInfo GameInfo_Snoopy = {
  "Snoopy",
  "klasyczne lapanie jajek",
  Game_Snoopy
};