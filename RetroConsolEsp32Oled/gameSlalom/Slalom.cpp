
#include "Slalom.h"
#include "Slalom_private.h"

namespace SlalomGame {

  SpriteCode NewRow_Array[4] = {FUEL,FUEL,FUEL,FUEL};
  SpriteCode Sprites_Array[6][4] = {{CLEAN, CLEAN, CLEAN, CLEAN}, 
                                    {CLEAN, CLEAN, CLEAN, CLEAN},
                                    {CLEAN, CLEAN, CLEAN, CLEAN},
                                    {CLEAN, CLEAN, CLEAN, CLEAN},
                                    {CLEAN, CLEAN, CLEAN, CLEAN}};


  static const uint8_t RowCode[14] = {1, 2, 4, 8, 3, 5, 6, 9, 10, 12, 7, 11, 13, 14};

  const int sprite_x[5][4] = {{23,42,61,80},{20,41,62,83},{17,40,63,86},{14,39,64,89},{4,34,63,92}};
  const int sprite_y[5] = {-4,4,13,27,40};
  int LivesLeft;
  int GameLevel = 0;
  int FuelLevel = FuelInit;
  bool LineSwitch = false;
  bool GameOverSlalom = false;
  bool ButtonPressed = false;
  unsigned long LastTime;
  unsigned long LastFuelBlinkTime;
  bool FuelBlinkState = false;
  int OldScoreForLivesAdding =0;
  int FrameDelay = DelayFramesInit; //odstep w ms pomiedzy klatkami
  int bike_x = 0; // bike position
  int SlalomSesionScore = 0;

  void DisplayInterLines() {
    LineSwitch = !LineSwitch;
	    myOLED.drawLine(95,64, 83, 0, SH110X_BLACK);
      myOLED.drawLine(64,0, 64, 64, SH110X_BLACK);
      myOLED.drawLine(33,64, 45, 0, SH110X_BLACK);
    if (LineSwitch) {
      myOLED.drawLine(92,48, 89, 32, SH110X_WHITE);
      myOLED.drawLine(86,16, 83, 0, SH110X_WHITE);

      myOLED.drawLine(64,0, 64, 8, SH110X_WHITE);
      myOLED.drawLine(64,17, 64, 24, SH110X_WHITE);
      myOLED.drawLine(64,33, 64, 40, SH110X_WHITE);
      myOLED.drawLine(64,49, 64, 56, SH110X_WHITE);
      
      myOLED.drawLine(33,64, 36, 48, SH110X_WHITE);
      myOLED.drawLine(39,32, 42, 16, SH110X_WHITE); 
    }
    if (!LineSwitch) {
      myOLED.drawLine(95,64, 92, 48, SH110X_WHITE);
      myOLED.drawLine(89,32, 86, 16, SH110X_WHITE); 

      myOLED.drawLine(64,9, 64, 16, SH110X_WHITE);
      myOLED.drawLine(64,25, 64, 32, SH110X_WHITE); 
      myOLED.drawLine(64,41, 64, 48, SH110X_WHITE);
      myOLED.drawLine(64,57, 64, 64, SH110X_WHITE);

      myOLED.drawLine(36,48, 39, 32, SH110X_WHITE);
      myOLED.drawLine(42,16, 45, 0, SH110X_WHITE);
    }
  }
  
  void DisplayLivesLeft(int k, uint8_t color) {
    myOLED.drawBitmap(120, 1, live_yes, 5, 6, SH110X_WHITE);
    myOLED.setTextColor(color);
    myOLED.setCursor(113,0);
    myOLED.print(k);
  }

  void GameInitParams() {
    LivesLeft = LivesInit;
    FuelLevel = FuelInit;
    GameLevel = 0;
    bike_x = random(1,5);
    Score = 0;
    FrameDelay = DelayFramesInit;
    OldScoreForLivesAdding =0;
    LastFuelBlinkTime = millis();
    FuelBlinkState = false;

    for (int i = 0; i<6; i++) {
      for (int j = 0; j<4;j++) {
        Sprites_Array[i][j] = CLEAN;
      }
    }
    DisplaySoundInfo(120, 14, SoundEnabled);
    DisplayRoad();
    DisplayInitTrack(SH110X_WHITE);
    DisplayFuelLevel();
    DisplayInterLines();
    DisplayLivesLeft(LivesLeft, SH110X_WHITE);
  }

  void DisplayInitTrack(uint8_t color) {
    for (int row = 0; row < 5; row++){
      for (int column = 0; column < 4; column++) {
        DisplaySprite(row, column, Sprites_Array[row][column], color);
      }
    }
  }
  void DrawAddedFuelStation(uint8_t color) {
    for (int row = 0; row < 5; row++){
      int column = 3;
      DisplaySprite(row, column, Sprites_Array[row][column], color);
    }
  }

  void DisplaySprite(int row, int column, SpriteCode Sprite, uint8_t color) {
    int x;
    int y;
    GetSprite_x_y(row, column, &x, &y);
    switch(Sprite) {
      case(GATE):
        if (row == 0)         {myOLED.drawBitmap(x, y, bramka1, sprite_width, sprite_height, color);
        } else if (row == 1)  {myOLED.drawBitmap(x, y, bramka2, sprite_width, sprite_height, color);
        } else if (row == 2)  {myOLED.drawBitmap(x, y, bramka3, sprite_width, sprite_height, color);
        } else if (row == 3)  {myOLED.drawBitmap(x, y, bramka4, sprite_width, sprite_height, color);}
        break;
      case(FUEL):
        if (row == 0)         {
          myOLED.drawBitmap(x, y, fuel1, sprite_width, sprite_height, color);
        }
        else if (row == 1)  {
          myOLED.drawBitmap(x, y, fuel2, sprite_width, sprite_height, color);
        } else if (row == 2)  {
          myOLED.drawBitmap(x, y, fuel2, sprite_width, sprite_height, color);
        } else if (row == 3)  {
          myOLED.drawBitmap(x, y, fuel3, sprite_width, sprite_height, color);
        }
        break;
      case(OIL):
        if (row == 0)         {myOLED.drawBitmap(x, y, oil1, sprite_width, sprite_height, color);
        } else if (row == 1)  {myOLED.drawBitmap(x, y, oil2, sprite_width, sprite_height, color);
        } else if (row == 2)  {myOLED.drawBitmap(x, y, oil3, sprite_width, sprite_height, color);
        } else if (row == 3)  {myOLED.drawBitmap(x, y, oil4, sprite_width, sprite_height, color);
        }
        break;
      case(CLEAN): 
      default:
        break;
    }
  }

  void DisplayBike(int bike_x) {
    if (bike_x<1 || bike_x>4) return;
    EraseBikeFromDisplay();
    switch(bike_x) {
      case(1):
      case(2):
        myOLED.drawBitmap(sprite_x[4][bike_x-1], sprite_y[4], motor_2, bike_width, bike_height, SH110X_WHITE);
        break;
      case(3):
        myOLED.drawBitmap(sprite_x[4][bike_x-1], sprite_y[4], motor_2, bike_width, bike_height, SH110X_WHITE);
        break;
      case(4):
        myOLED.drawBitmap(sprite_x[4][bike_x-1], sprite_y[4], motor_2, bike_width, bike_height, SH110X_WHITE);
        break;
      default:
        break;
    }
  }

  void DisplayRoad() {
    myOLED.drawLine(sprite_x[4][0]-3, SCREEN_HEIGHT, sprite_x[0][0]+4, 0, SH110X_WHITE);
    myOLED.drawLine(sprite_x[4][0]-4, SCREEN_HEIGHT, sprite_x[0][0]+3, 0, SH110X_WHITE);

    myOLED.drawLine(sprite_x[4][3]+bike_width, SCREEN_HEIGHT, sprite_x[0][3]+sprite_width-5, 0, SH110X_WHITE);
    myOLED.drawLine(sprite_x[4][3]+bike_width-1, SCREEN_HEIGHT, sprite_x[0][3]+sprite_width-6, 0, SH110X_WHITE);
  }

  void GetSprite_x_y(int row, int column, int *x, int *y) {
    *x = sprite_x[row][column];
    *y = sprite_y[row];
  }

  bool CheckFuelBlink() {
    if (FuelLevel <= 14) {
      if (millis() - LastFuelBlinkTime > 300) { // Miga co 300ms
        LastFuelBlinkTime = millis();
        FuelBlinkState = !FuelBlinkState;
        return true;
      }
    } else {
      FuelBlinkState = false; // Zawsze widoczny gdy fuel > 14
    }
    return false;
  }

  void DisplayFuelLevel() {
    myOLED.setTextSize(1);
    
    CheckFuelBlink();
    myOLED.setTextColor(SH110X_BLACK);
    myOLED.setCursor(0,0);
    myOLED.print("F");
    myOLED.print(FuelLevel+1);
    
    if (FuelLevel > 14 || FuelBlinkState) {
      myOLED.setTextColor(SH110X_WHITE);
      myOLED.setCursor(0,0);
      myOLED.print("F");
      myOLED.print(FuelLevel);
    }
  }

  void RedrawScreen() {
    DisplayInterLines();
    DisplayInitTrack(SH110X_BLACK);

    UpdateRaceArray();
    DisplayInitTrack(SH110X_WHITE);
  }

  void UpdateRaceArray() {
    SpriteCode tempArray[4] ={CLEAN,CLEAN,CLEAN,CLEAN};

    for (int column = 0; column < 4; column++) {
      Sprites_Array[4][column] = Sprites_Array[3][column]; //z 3 na 4 czyli kolizyjny
    }
    for (int column = 0; column < 4; column++) {
      Sprites_Array[3][column] = Sprites_Array[2][column]; //z 2 na 3
    }
    for (int column = 0; column < 4; column++) {
      Sprites_Array[2][column] = Sprites_Array[1][column]; //z 1 na 2
    }
    for (int column = 0; column < 4; column++) {
      Sprites_Array[1][column] = Sprites_Array[0][column]; //z 0 na 1
    }
    
    GenerateNextRow(tempArray);

    for (int column = 0; column < 4; column++) {
      Sprites_Array[0][column] = tempArray[column]; //
    }
  }

  void NewRowFromCode(int Code) {
    for (int i = 0; i < 4; i++) {
      NewRow_Array[i] = CLEAN;
      if ((Code >> i) & 1) NewRow_Array[i] = GATE; 
    }
    return;
  }

  void GenerateNextRow(SpriteCode Array[4]) {
    int KodWiersza;
    int LosLevelRow;
    switch (GameLevel) {
      case(0):
        //jedno auto na wiersz
        KodWiersza = random(0,4);
        NewRowFromCode(RowCode[KodWiersza]);
        break;
      case(1):
        if (random(1,3) == 1) {KodWiersza = random(0,10);}
        else {KodWiersza = random(0,4);}
        NewRowFromCode(RowCode[KodWiersza]);
        break;
      case(2):
        LosLevelRow = random(1,3);
        if (LosLevelRow == 1) {KodWiersza = random(0,4);}
        else if (LosLevelRow == 2) {KodWiersza = random(0,10);}
        else {KodWiersza = random(0,14);}
        NewRowFromCode(RowCode[KodWiersza]);
        break;
    }
    for (int j=0; j<4; j++) {
      Array[j] = NewRow_Array[j];
    }
  }

  void EraseBikeFromDisplay() {
    myOLED.drawBitmap(sprite_x[4][0], sprite_y[4], motor_2, bike_width, bike_height, SH110X_BLACK);
    myOLED.drawBitmap(sprite_x[4][1], sprite_y[4], motor_2, bike_width, bike_height, SH110X_BLACK);
    myOLED.drawBitmap(sprite_x[4][2], sprite_y[4], motor_2, bike_width, bike_height, SH110X_BLACK);
    myOLED.drawBitmap(sprite_x[4][3], sprite_y[4], motor_2, bike_width, bike_height, SH110X_BLACK);
  }

  bool CheckIfNextFrame() {
    if (millis()-LastTime > (FrameDelay)) {
        LastTime  = millis();
        return true;
    }
    return false;
  }

  void CheckIfColissionHappen() {
    switch(Sprites_Array[4][bike_x-1]) {
      case(CLEAN):
        if (SoundEnabled) MyTune(TON_RAMKA_FREQ,30);
        Score++;
        FuelLevel--;
        break;
      case(GATE):
        if (SoundEnabled) MyTune(TON_ODLICZANIE_FREQ,30);
        FuelLevel--;
        DisplayLivesLeft(LivesLeft, SH110X_BLACK);
        LivesLeft--;
        DisplayLivesLeft(LivesLeft, SH110X_WHITE);
        ColissionDetected();
        break;
      case (FUEL):
        DisplayTankowanie();
        break;
      default:
        break;
    }
  }

  void DisplayTankowanie() {
    DisplayBike(bike_x);
    myOLED.drawBitmap(117, 30, f1_tanking, 18, 18, SH110X_WHITE);
    for (int i = FuelLevel; i < FuelLevel +1+ IncreaseFuel; i++) {
      if (SoundEnabled) MyTune(TON_KONIECGRY_FREQ+i, 4);

      myOLED.setTextColor(SH110X_WHITE);
      myOLED.setCursor(0,0);
      myOLED.print("F");
      myOLED.print(i);
      myOLED.display();
      delay(4);
      myOLED.setTextColor(SH110X_BLACK);
      myOLED.setCursor(0,0);
      myOLED.print("F");
      myOLED.print(i);
      myOLED.display();
    }
    myOLED.setTextColor(SH110X_WHITE);
    myOLED.drawBitmap(117, 30, f1_tanking, 18, 18, SH110X_BLACK);
    FuelLevel = FuelLevel -1 + IncreaseFuel;
    FrameDelay = DelayFramesInit;
    myOLED.display();
  }

  void ResetRaceArray() {
    for (int column = 0; column < 4; column++) {
      Sprites_Array[0][column] = Sprites_Array[2][column];
    }
    for (int column = 0; column < 4; column++) {
      Sprites_Array[1][column] = Sprites_Array[3][column];
    }
    for (int column = 0; column < 4; column++) {
      Sprites_Array[2][column] = CLEAN;
    }
    for (int column = 0; column < 4; column++) {
      Sprites_Array[3][column] = CLEAN;
    }
    for (int column = 0; column < 4; column++) {
      Sprites_Array[4][column] = CLEAN;
    }
  }

	void ColissionDetected() {
    DisplayInitTrack(SH110X_BLACK);
    ResetRaceArray();
    DisplayInitTrack(SH110X_WHITE);
    DisplayFuelLevel();
    EraseBikeFromDisplay();
    myOLED.drawBitmap(sprite_x[4][bike_x-1], sprite_y[4], Wrak, bike_width, bike_height, SH110X_WHITE);
    myOLED.display();
    delay(400);
    myOLED.drawBitmap(sprite_x[4][bike_x-1], sprite_y[4], Wrak, bike_width, bike_height, SH110X_BLACK);

  }

  void HandleGameOver() {
    myOLED.setTextSize(2);
    myOLED.setTextColor(SH110X_WHITE); 
    myOLED.clearDisplay();
    if (Score > SlalomSesionScore) {
      SlalomSesionScore = Score;
    }
    int Slalom_highscore;
    EEPROM.get(Game_SlalomRecord, Slalom_highscore);
    if (Score > Slalom_highscore) {
      EEPROM.put(Game_SlalomRecord, Score);
      EEPROM.commit();
    }
    if (FuelLevel == 0) {
      myOLED.setCursor(10,0);
      myOLED.print("No fuel!");
    }

    myOLED.setTextSize(1); 
    myOLED.setCursor(1,22);
    myOLED.print("Your Score:   ");
    myOLED.print(Score);
    myOLED.setCursor(1,34);
    myOLED.print("Sesion best:  ");
    myOLED.print(SlalomSesionScore);
    myOLED.setCursor(1,46);
    myOLED.print("All time best: ");
    EEPROM.get(Game_SlalomRecord, Slalom_highscore);
    myOLED.print(Slalom_highscore);
    myOLED.display();
  }

  void CleanRow5() {
    for (int column = 0; column < 4; column++) {
      Sprites_Array[4][column] = CLEAN;
    }
  }

  void CalculateGameSpeedAndLevel() {
    if (Score >= 20) FrameDelay = 400;
    if (Score >= 30) FrameDelay = 380;
    if (Score >= 35) GameLevel = 1;
    if (Score >= 40) FrameDelay = 330;
    if (Score >= 60) FrameDelay = 300;
    if (Score >= 80) FrameDelay = 280;
    if (Score >= 100) FrameDelay = 270;
    if (Score >= 110) GameLevel = 2;
    if (Score >= 120) FrameDelay = 250;
    if (Score >= 130) FrameDelay = 230;
    if (Score >= 140) FrameDelay = 210;
    if (Score >= 150) FrameDelay = 200;
  }

  void CheckIfGameOver() {
    if (LivesLeft == 0 || FuelLevel == 0) GameOverSlalom=true;
    if (Score - OldScoreForLivesAdding > 40) {
      DisplayLivesLeft(LivesLeft, SH110X_BLACK);
      LivesLeft++;
      DisplayLivesLeft(LivesLeft, SH110X_WHITE);
      for (int i=1; i<4; i++){
        if (SoundEnabled) MyTune(TON_ODLICZANIE_FREQ+10*i, 30);
        delay(20);
      }
      OldScoreForLivesAdding = Score;
    }
  }

  void AddFuelStation() {
    if (Sprites_Array[0][3] == CLEAN && 
        Sprites_Array[1][3] == CLEAN && 
        FuelLevel < 70 &&
        Sprites_Array[1][3] != FUEL && 
        Sprites_Array[2][3] != FUEL && 
        Sprites_Array[3][3] != FUEL) {
      DrawAddedFuelStation(SH110X_BLACK);
      Sprites_Array[0][3] = CLEAN;
      Sprites_Array[1][3] = FUEL;
      Sprites_Array[2][3] = CLEAN;
      Sprites_Array[3][3] = CLEAN;
      DrawAddedFuelStation(SH110X_WHITE);
    }
  }

  void PauseGame(){
    myOLED.setTextSize(1); 
    myOLED.setCursor(40,28);
    myOLED.setTextColor(SH110X_WHITE);
    myOLED.print("PAUSE");
    myOLED.display();
    while (!IsPressed(DownRight));

    myOLED.setCursor(40,28);
    myOLED.setTextColor(SH110X_BLACK);
    myOLED.print("PAUSE");
    myOLED.setTextColor(SH110X_WHITE);
  }

// //############################################

void WelcomeScreen() {
  myOLED.clearDisplay();
  myOLED.setTextColor(SH110X_WHITE);
  myOLED.setTextSize(2);
  myOLED.setCursor(2, 1);
  myOLED.println("Just ride");
  myOLED.setTextSize(1);
  myOLED.setCursor(45, 31);
  myOLED.println("    pause > o");

  myOLED.setCursor(0, 42);
  myOLED.println("o > left");

  myOLED.setCursor(45, 42);
  myOLED.println("    right > o");
  myOLED.display();
  delay(DELAY1000MS);
} 

void Game_Slalom() {

  while(1==1) {  
    GameOverSlalom = false;

    WelcomeScreen();
    myOLED.clearDisplay();
    GameInitParams();
    DisplaySoundInfo(120, 14, SoundEnabled);
    myOLED.display();

    while (!GameOverSlalom){
      bool bikeMoved;
      bikeMoved = CheckButtonsAndReturnBikeMovedStatus();
 
      if (CheckIfNextFrame()) {
        RedrawScreen();
        DisplayFuelLevel();
        DisplayBike(bike_x);
        CheckIfColissionHappen();
        CleanRow5();
        AddFuelStation();
        myOLED.display();
      } else if (bikeMoved) {
        DisplayBike(bike_x);
        myOLED.display();
      }
      CalculateGameSpeedAndLevel();
      CheckIfGameOver();
    }
    if (GameOverSlalom) HandleGameOver();
    while (!IsPressed(DownRight));
  }
}

bool CheckButtonsAndReturnBikeMovedStatus(){
  if (!ButtonPressed) {
    switch (ReadButton(nullptr, Timer1Sec)) {
      case UpRight:
        PauseGame();
        WaitForButtonRelease();
        return false;
      case DownLeft:
        ButtonPressed = true;
        if (bike_x > 1) {
          bike_x--;
          return true;
        }
      case DownRight:
        ButtonPressed = true;
        if (bike_x < 4) {
          bike_x++;
          return true;
        }
    }
  } else {
    if (ReadButton(nullptr, Timer1Sec) == NONE) ButtonPressed = false;
  }
  return false;
}

} // namespace SlalomGame

// GameInfo musi być poza namespace, żeby był dostępny globalnie
const GameInfo GameInfo_Slalom = {
	"Slalom",
	"Zjeżdżaj, omijaj bramki i przeszkody!",
	SlalomGame::Game_Slalom,
	Game_SlalomRecord
};
