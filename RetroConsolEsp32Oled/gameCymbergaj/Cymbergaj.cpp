#include "Cymbergaj.h"
#include "Cymbergaj_private.h"

//  --- Globalne zmienne gry ---
cymGameStruct CymGame;
cymMenuStruct Menu;

// Główna pętla
void Game_Cymbergaj(){
  // Inicjalizacja zmiennych gry
  cymBallStruct       Ball;
  cymPlayerStruct     LeftPlayer;
  cymPlayerStruct     RightPlayer;

  ShowUserMenu(Menu, CymGame);
  CymInitParams(CymGame, Ball, LeftPlayer, RightPlayer);

  while(true) {
    DrawGameScreen(CymGame, Ball, LeftPlayer, RightPlayer);
    if (itsTimeForBallPosUpdate(Ball)) updateBallPos(CymGame, Ball, LeftPlayer, RightPlayer);
    if (itsTimeForPadlePosUpdate(CymGame)) {
      PadlePosPlayerUpdate(CymGame, LeftPlayer, RightPlayer);
      PadlePosAiUpdate(CymGame, Ball, LeftPlayer, RightPlayer);
    }
    if (itsGameOver(CymGame, LeftPlayer, RightPlayer)) {
      DisplayGameSummary(CymGame, LeftPlayer, RightPlayer);
      return;
    }
  }
}

void CymInitParams(cymGameStruct& CymGame, cymBallStruct& Ball, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer){
  Ball.x = HalfFieldX;
  Ball.y = HalfFieldY;
  Ball.isHeading = toRight;
  Ball.dY = 0;

  LeftPlayer.y = HalfFieldY;
  RightPlayer.y = HalfFieldY;
  LeftPlayer.score = 0;
  RightPlayer.score = 0;

  CymGame.padleHalfWidth = 11;
  CymGame.padleDeltaMoveY = 3;
  CymGame.updateScreen = true;

  switch (CymGame.ballSpeed) {
    case Wolno:
      Ball.updateRateInit = 80;
      break;
    case Srednio:
      Ball.updateRateInit = 25;
      break;
    case Szybko:
      Ball.updateRateInit = 20;
      break;
  }
  Ball.updateRate = Ball.updateRateInit;
}

void PadlePosAiUpdate(cymGameStruct& CymGame, cymBallStruct& Ball,  cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer) {
  int deltaMoveY=0;
  if (millis()-CymGame.aiPadlePosPrevUpdateTime > (CymGame.aiPadlePosUpdateRate)) {
    CymGame.aiPadlePosPrevUpdateTime  = millis();

    switch(CymGame.aiLevel){
      case 1: //amator
        deltaMoveY=2+random(-3,3);
        break;
      case 2: // normalny
        deltaMoveY=4*random(0,2);
        break;
      default: // geniusz
        deltaMoveY=2;
        break;
    }
    if (CymGame.playersNo != 2) {
      if (RightPlayer.y > Ball.y) {
        RightPlayer.y = RightPlayer.y - deltaMoveY;
      } else {
        RightPlayer.y = RightPlayer.y + deltaMoveY;
      }
      CymGame.updateScreen = true;
    }
    if (CymGame.playersNo == 0){
      if (LeftPlayer.y > Ball.y) {
        LeftPlayer.y = LeftPlayer.y - deltaMoveY;
      } else {
        LeftPlayer.y = LeftPlayer.y + deltaMoveY;
      }
      CymGame.updateScreen = true;
    }
  }
}

void PadlePosPlayerUpdate(cymGameStruct& CymGame, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer) {
  int PadleMinY = 0;
  int PadleMaxY = FieldYmax + 4;

  if (CymGame.playersNo != 0) {
    if ( IsPressed(UpLeft)) {
      LeftPlayer.y = LeftPlayer.y-CymGame.padleDeltaMoveY;
      if (LeftPlayer.y < PadleMinY) LeftPlayer.y = PadleMinY;
      CymGame.updateScreen = true;
    }
    if ( IsPressed(DownLeft)) {
      LeftPlayer.y = LeftPlayer.y+CymGame.padleDeltaMoveY;
      if ( LeftPlayer.y > PadleMaxY ) LeftPlayer.y = PadleMaxY;
      CymGame.updateScreen = true;
    }
  }

  if (CymGame.playersNo == 2) {
      if (IsPressed(UpRight)) {
      RightPlayer.y = RightPlayer.y-CymGame.padleDeltaMoveY;
      if (RightPlayer.y < PadleMinY) RightPlayer.y = PadleMinY;
      CymGame.updateScreen = true;
    }
    if (IsPressed(DownRight)) {
      RightPlayer.y = RightPlayer.y + CymGame.padleDeltaMoveY;
      if (RightPlayer.y > PadleMaxY) RightPlayer.y = PadleMaxY;
      CymGame.updateScreen = true;
    }
  }
}

void UpdateBallPosAndCheckForBandOrGoal(cymGameStruct& CymGame, cymBallStruct& Ball, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer) {
  updateBallXpos(Ball);
  updateBallYpos(Ball, LeftPlayer, RightPlayer);
  CheckIfBallHitBand(Ball);
  if (Ball.x <= 3) {
    CheckIfLeftPlayerLostGoal(CymGame, Ball, LeftPlayer, RightPlayer);
    if ( Ball.isHeading == toLeft ) Ball.isHeading = (direction)random(1,3);
    if ( Ball.isHeading == toLeftUp ) Ball.isHeading = toRightUp;
    if ( Ball.isHeading == toLeftDown ) Ball.isHeading = toRightDown;
    Ball.x = 5; 
  }
  if (Ball.x >= FieldXmax-4) {
    CheckIfRightPlayerLostGoal(CymGame, Ball, LeftPlayer, RightPlayer);
    if ( Ball.isHeading == toRight ) Ball.isHeading = (direction)random(4,6);
    if ( Ball.isHeading == toRightUp ) Ball.isHeading = toLeftUp;
    if ( Ball.isHeading == toRightDown ) Ball.isHeading = toLeftDown;
    Ball.x = FieldXmax-6;
  }
}

int updateBallDyAfterPadleHit(int BallX, int BallY, int LeftPadleY, int RightPadleY) {
  int padleHitDist;
  if (BallX < HalfFieldX) {
    padleHitDist = abs(BallY - LeftPadleY);
  } else {
    padleHitDist = abs(BallY - RightPadleY);
  }
  if (padleHitDist < 2) {return 1;
  } else if (padleHitDist < 6) {return 2;
  } else if  (padleHitDist < 9) {return 3;
  } else return 5;
}


void CheckIfLeftPlayerLostGoal(cymGameStruct& CymGame, cymBallStruct& Ball, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer) {
  if (Ball.y < LeftPlayer.y - CymGame.padleHalfWidth-2 || Ball.y > LeftPlayer.y + CymGame.padleHalfWidth+2) {
    RightPlayer.score++;
    CymGame.updateScreen = true;
    HandleLostGoal(CymGame, Ball, LeftPlayer, RightPlayer);
  } else {
    if (soundEnabled) MyTune(TON_RAMKA_FREQ,30);
    Ball.updateRate = Ball.updateRate - CymGame.speedInc;
    Ball.dY = updateBallDyAfterPadleHit(Ball.x, Ball.y, LeftPlayer.y, RightPlayer.y);
  }
}

void CheckIfRightPlayerLostGoal(cymGameStruct& CymGame, cymBallStruct& Ball, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer) {
  if (Ball.y < RightPlayer.y - CymGame.padleHalfWidth-2 || Ball.y > RightPlayer.y + CymGame.padleHalfWidth+2) {
    LeftPlayer.score++;
    CymGame.updateScreen = true;
    HandleLostGoal(CymGame, Ball, LeftPlayer, RightPlayer);
  } else {
    if (soundEnabled) MyTune(TON_ODLICZANIE_FREQ,30);
    Ball.updateRate = Ball.updateRate - CymGame.speedInc;
    Ball.dY = updateBallDyAfterPadleHit(Ball.x, Ball.y, LeftPlayer.y, RightPlayer.y);
  }
}

void HandleLostGoal(cymGameStruct& CymGame, cymBallStruct& Ball, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer) {

  DrawBall(Ball);
  DrawGameScreen(CymGame, Ball, LeftPlayer, RightPlayer);
  Ball.prevX = Ball.x;
  if (Ball.x < HalfFieldX) {
    Ball.x = 3;
  } else {
    Ball.x = FieldXmax - 3;
  }
  DisplayGoal(Ball, LeftPlayer, RightPlayer);
  Ball.y = HalfFieldY;
  Ball.dY = updateBallDyAfterPadleHit(Ball.x, Ball.y, LeftPlayer.y, RightPlayer.y);
  DrawBall(Ball);
  Ball.updateRate = Ball.updateRateInit;
  DrawGameScreen(CymGame, Ball, LeftPlayer, RightPlayer);
  CymGame.updateScreen = true;
}

bool itsGameOver(cymGameStruct& CymGame, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer) {
  if (CymGame.playersNo == 2) {
    if (LeftPlayer.score >= CymGame.maxPoints || RightPlayer.score >= CymGame.maxPoints) {
      return true;
    }
  } else if (CymGame.playersNo == 1) {
    if (RightPlayer.score >= CymGame.maxPoints) {
      return true;
    }
  } else if (CymGame.playersNo == 0) {
    if (IsPressed(UpLeft) || 
        IsPressed(DownRight) ||
        IsPressed(UpRight) ||
        IsPressed(DownLeft)) {
      return true;
    }
  }
  return false;
} 

void DrawField() {
  myOLED.drawLine (4, 0, FieldXmax-5, 0, SH110X_WHITE);
  myOLED.drawLine (4, FieldYmax, FieldXmax-5, FieldYmax, SH110X_WHITE);
  myOLED.drawLine (HalfFieldX, 0, HalfFieldX, FieldYmax-1, SH110X_WHITE); // linia srodkowa
  myOLED.drawCircle ( HalfFieldX, HalfFieldY, 7, SH110X_WHITE );
}

void updateBallPos(cymGameStruct& CymGame, cymBallStruct& Ball, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer) {
  Ball.prevX = Ball.x;
  Ball.prevY = Ball.y;
  UpdateBallPosAndCheckForBandOrGoal(CymGame, Ball, LeftPlayer, RightPlayer);
  if (CymGame.ballSpeed == Szybko) {
    UpdateBallPosAndCheckForBandOrGoal(CymGame, Ball, LeftPlayer, RightPlayer);
  }
  CymGame.updateScreen = true;
}

void DrawBall(cymBallStruct& Ball) {
  myOLED.fillCircle (Ball.prevX,Ball.prevY, 2, SH110X_BLACK);
  myOLED.fillCircle (Ball.x,Ball.y, 2, SH110X_WHITE);
}

void DrawPadle(cymGameStruct& CymGame, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer) {
  myOLED.fillRect (0,0, 3, 64, SH110X_BLACK);
  myOLED.drawRect (1,LeftPlayer.y-CymGame.padleHalfWidth, 2, 2*CymGame.padleHalfWidth-3, SH110X_WHITE);

  myOLED.fillRect (FieldXmax-3,1, 2, FieldYmax-2, SH110X_BLACK);
  myOLED.drawRect (FieldXmax-3,RightPlayer.y - CymGame.padleHalfWidth, 2, 2*CymGame.padleHalfWidth-3, SH110X_WHITE);
}

void ShowUserMenu(cymMenuStruct& Menu, cymGameStruct& CymGame) {
  btPressedCode btn;
  int SelectedRow = 0;
  int SelectedRowOption = Menu.rowOptionSelect[SelectedRow];

  while (true) {
    myOLED.clearDisplay();
    myOLED.setCursor(1, 1);
    myOLED.setTextSize(2); 
    myOLED.print("Cymbergaj");
    myOLED.setTextSize(1);
 
    if (Menu.rowMax > SelectedRow) {
      myOLED.setCursor(1, Menu.initY+ Menu.deltaY);
      myOLED.print(">");
      myOLED.setCursor(Menu.initX, Menu.initY + Menu.deltaY);
      myOLED.print(Menu.optionString[SelectedRow][0]);
      myOLED.print(Menu.optionString[SelectedRow][Menu.rowOptionSelect[SelectedRow]]);
    }
    if (Menu.rowMax > SelectedRow+1) {
      myOLED.setCursor(Menu.initX, Menu.initY + 2 * Menu.deltaY);
      myOLED.print(Menu.optionString[SelectedRow+1][0]);
      myOLED.print(Menu.optionString[SelectedRow+1][Menu.rowOptionSelect[SelectedRow+1]]);
    }
    if (Menu.rowMax > SelectedRow+2) {
      myOLED.setCursor(Menu.initX, Menu.initY + 3 * Menu.deltaY);
      myOLED.print(Menu.optionString[SelectedRow+2][0]);
      myOLED.print(Menu.optionString[SelectedRow+2][Menu.rowOptionSelect[SelectedRow+2]]);
    }





    if (IsPressed(UpLeft)) {
      SelectedRow--;
      if (SelectedRow < 0) SelectedRow = 0;
      SelectedRowOption = Menu.rowOptionSelect[SelectedRow];
      while (ReadButton(nullptr, Timer1Sec) != NONE) ; // wait for button release
    }
    if (IsPressed(DownLeft)) {
      SelectedRow++;
      if (SelectedRow >= Menu.rowMax) SelectedRow = Menu.rowMax - 1;
      SelectedRowOption = Menu.rowOptionSelect[SelectedRow];
      while (ReadButton(nullptr, Timer1Sec) != NONE) ; // wait for button release
    }
    if (IsPressed(UpRight)) {
      SelectedRowOption++;
      if (SelectedRowOption >= Menu.optionsInRow) {
        SelectedRowOption = 1;
      }
      Menu.rowOptionSelect[SelectedRow] = SelectedRowOption;
      while (ReadButton(nullptr, Timer1Sec) != NONE) ; // wait for button release
    }
    if (IsPressed(DownRight)) {       // zatwierdzenie wyborow i przejscie do gry
      while (ReadButton(nullptr, Timer1Sec) != NONE) ; // wait for button release
      CymParamsUpdateFromUserMenu(Menu, CymGame);
      return;
    }
    myOLED.display();
  }
}

void CymParamsUpdateFromUserMenu(cymMenuStruct& Menu, cymGameStruct& CymGame) {
  CymGame.playersNo = Menu.menuOptionValue[0][Menu.rowOptionSelect[0]-1];
  CymGame.ballSpeed = (speed)Menu.menuOptionValue[1][Menu.rowOptionSelect[1]-1];
  CymGame.aiLevel = Menu.menuOptionValue[3][Menu.rowOptionSelect[3]-1];
  CymGame.speedInc = Menu.menuOptionValue[4][Menu.rowOptionSelect[4]-1];
}

bool itsTimeForBallPosUpdate(cymBallStruct& Ball) {
  if (millis()-Ball.prevUpdateTime > (Ball.updateRate)) {
    Ball.prevUpdateTime  = millis();
    return true;
  }
    return false;
}

void updateBallXpos(cymBallStruct& Ball) {
  switch (Ball.isHeading) {
    case toLeft:
    case toLeftDown:
    case toLeftUp:
      Ball.x = Ball.x - deltaX;
      break;

    case toRight:
    case toRightDown:
    case toRightUp:
      Ball.x = Ball.x + deltaX;
      break;
  }
}

void updateBallYpos(cymBallStruct& Ball, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer) {
  switch (Ball.isHeading) {
    case toRightUp:
    case toLeftUp:
      Ball.y = Ball.y - Ball.dY;
      break;
    case toRightDown:
    case toLeftDown:
      Ball.y = Ball.y + Ball.dY;
      break;
    case toLeft:
    case toRight:
      Ball.y = Ball.y;
       break;
  }
}

bool itsTimeForPadlePosUpdate(cymGameStruct& CymGame) {
  unsigned long FinalUpdateRate = CymGame.playerPadlePosUpdateRate;

  if (CymGame.ballSpeed == Szybko) FinalUpdateRate = CymGame.playerPadlePosUpdateRate / 2;

  if (millis()-CymGame.playerPadlePosPrevUpdateTime > (FinalUpdateRate)) {
      CymGame.playerPadlePosPrevUpdateTime  = millis();
      return true;
  }
  return false;
}

void CheckIfBallHitBand(cymBallStruct& Ball) {
  if (Ball.y <=2) {
    if ( Ball.isHeading == toLeftUp ) Ball.isHeading = toLeftDown;
    if ( Ball.isHeading == toRightUp ) Ball.isHeading = toRightDown;
    Ball.y = 3;
  }
  if (Ball.y >= FieldYmax-2) {
    if ( Ball.isHeading == toLeftDown ) Ball.isHeading = toLeftUp;
    if ( Ball.isHeading == toRightDown ) Ball.isHeading = toRightUp;
    Ball.y = FieldYmax-4;
  }
};

void DisplayGameSummary(cymGameStruct& CymGame, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer) {
  myOLED.clearDisplay();
  myOLED.setCursor(20, 20);
  myOLED.setTextSize(1); 
  if (CymGame.playersNo == 2) {
    if (LeftPlayer.score >= CymGame.maxPoints) {
      myOLED.print("Lewa WYGRYWA!");
    } else {
      myOLED.print("Prawa WYGRYWA!");
    }
  } else if (CymGame.playersNo == 1) {
    if (LeftPlayer.score > RightPlayer.score && LeftPlayer.score >= CymGame.maxPoints) {
      myOLED.print("Wygrales z AI!");
    } else if (RightPlayer.score == LeftPlayer.score && LeftPlayer.score == CymGame.maxPoints) {
      myOLED.print("REMIS!");
    }else {
      myOLED.print("AI Cie pokonal!");
    }
  } else if (CymGame.playersNo == 0) {
    myOLED.print("To tylko demo ;)");
  }
  myOLED.display();
  delay(800);
}

void DisplayScore(cymGameStruct& CymGame, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer) {
  myOLED.setCursor(HalfFieldX-16, 5);
  myOLED.print(LeftPlayer.score);
  myOLED.setCursor(HalfFieldX+12, 5);
  myOLED.print(RightPlayer.score);

  if (CymGame.playersNo < 2) {
    myOLED.setCursor(HalfFieldX+12, FieldYmax-10);
    myOLED.print("AI");
  }
  if (CymGame.playersNo == 0) {
     myOLED.setCursor(HalfFieldX-16, FieldYmax-10);
    myOLED.print("AI");
  }
}

void DrawGameScreen(cymGameStruct& CymGame, cymBallStruct& Ball, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer) {
  if (CymGame.updateScreen) {
    myOLED.clearDisplay();
    DrawBall(Ball);
    DrawField();
    DrawPadle( CymGame, LeftPlayer, RightPlayer);
    DisplayScore(CymGame, LeftPlayer, RightPlayer);
    myOLED.display();
    CymGame.updateScreen = false;
  }
}

void DisplayGoal(cymBallStruct& Ball, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer) {
  DrawBall(Ball);
  Ball.prevX = Ball.x;
  Ball.prevY = Ball.y;
  myOLED.display();
  myOLED.setTextSize(1); 
  if (Ball.x < HalfFieldX) {
    for (int i = 0; i < 5; i++) {
      myOLED.setCursor(10, 5);
      Ball.x--;Ball.x--;
      Ball.dY = Ball.dY/2;
      updateBallYpos(Ball, LeftPlayer, RightPlayer);
      DrawBall(Ball);
      myOLED.setTextColor(SH110X_WHITE);
      myOLED.print("R");
      myOLED.setCursor(FieldXmax/2+12, 25);
      myOLED.print("GOAL!");
      myOLED.display();
      if (soundEnabled) {
        tone(BUZZER_PIN, TON_PALETKA_FREQ, DELAY250MS);
      } else delay(DELAY100MS);
      noTone(BUZZER_PIN);

      Ball.prevX = Ball.x;
      Ball.prevY = Ball.y;
      myOLED.setCursor(10, 5);
      myOLED.setTextColor(SH110X_BLACK);
      myOLED.print("R");
      myOLED.display();
      delay(130);
    }    

  } else {
    for (int i = 0; i < 5; i++) {
      myOLED.setCursor(FieldXmax-20, 5);
      Ball.x++;Ball.x++;
      Ball.dY = Ball.dY/2;
      updateBallYpos(Ball, LeftPlayer, RightPlayer);
      DrawBall(Ball);
      myOLED.setTextColor(SH110X_WHITE);
      myOLED.print("R");
      myOLED.setCursor(FieldXmax/2-38, 25);
      myOLED.print("GOAL!");
      myOLED.display();
      if (soundEnabled) {
        tone(BUZZER_PIN,TON_PALETKA_FREQ, DELAY250MS);
      }else delay(130);
      noTone(BUZZER_PIN);
      Ball.prevX = Ball.x;
      Ball.prevY = Ball.y;
      myOLED.setCursor(FieldXmax-20, 5);
      myOLED.setTextColor(SH110X_BLACK);
      myOLED.print("R");
      myOLED.display();
      delay(DELAY100MS);
    }
  }
  noTone(BUZZER_PIN);
  myOLED.setTextColor(SH110X_WHITE);
}

const GameInfo GameInfo_Cymbergaj = {
  "Cymbergaj",
  "Klasyczny air hockey na OLED",
  Game_Cymbergaj
};