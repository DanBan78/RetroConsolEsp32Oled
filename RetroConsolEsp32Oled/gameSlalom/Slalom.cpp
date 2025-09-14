
#include "Slalom.h"
#include "Slalom_private.h"

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
  int PlamaOlejuCount=0;
  unsigned long LastTime;
  unsigned long LastTime2;
  bool Wyciekoleju = false;
  int colissionsCount = 0;
  int OldScoreForLivesAdding =0;
  int FrameDelay = DelayFramesInit; //odstep w ms pomiedzy klatkami
  int car_x = 0; // car position
  int SlalomSesionScore = 0;

  void display_roadLine() {
    LineSwitch = !LineSwitch;
    if (LineSwitch) {
      myOLED.drawLine(95,64, 92, 48, SH110X_BLACK);
      myOLED.drawLine(92,48, 89, 32, SH110X_WHITE);
      myOLED.drawLine(89,32, 86, 16, SH110X_BLACK); 
      myOLED.drawLine(86,16, 83, 0, SH110X_WHITE);

      myOLED.drawLine(64,0, 64, 8, SH110X_WHITE);
      myOLED.drawLine(64,9, 64, 16, SH110X_BLACK);
      myOLED.drawLine(64,17, 64, 24, SH110X_WHITE);
      myOLED.drawLine(64,25, 64, 32, SH110X_BLACK); 
      myOLED.drawLine(64,33, 64, 40, SH110X_WHITE);
      myOLED.drawLine(64,41, 64, 48, SH110X_BLACK);
      myOLED.drawLine(64,49, 64, 56, SH110X_WHITE);
      myOLED.drawLine(64,57, 64, 64, SH110X_BLACK);
      
      myOLED.drawLine(33,64, 36, 48, SH110X_WHITE);
      myOLED.drawLine(36,48, 39, 32, SH110X_BLACK);
      myOLED.drawLine(39,32, 42, 16, SH110X_WHITE); 
      myOLED.drawLine(42,16, 45, 0, SH110X_BLACK);
      myOLED.display();
    }
    if (!LineSwitch) {
      myOLED.drawLine(95,64, 92, 48, SH110X_WHITE);
      myOLED.drawLine(92,48, 89, 32, SH110X_BLACK);
      myOLED.drawLine(89,32, 86, 16, SH110X_WHITE); 
      myOLED.drawLine(86,16, 83, 0, SH110X_BLACK);

      myOLED.drawLine(64,0, 64, 8, SH110X_BLACK);
      myOLED.drawLine(64,9, 64, 16, SH110X_WHITE);
      myOLED.drawLine(64,17, 64, 24, SH110X_BLACK);
      myOLED.drawLine(64,25, 64, 32, SH110X_WHITE); 
      myOLED.drawLine(64,33, 64, 40, SH110X_BLACK);
      myOLED.drawLine(64,41, 64, 48, SH110X_WHITE);
      myOLED.drawLine(64,49, 64, 56, SH110X_BLACK);
      myOLED.drawLine(64,57, 64, 64, SH110X_WHITE);

      myOLED.drawLine(33,64, 36, 48, SH110X_BLACK);
      myOLED.drawLine(36,48, 39, 32, SH110X_WHITE);
      myOLED.drawLine(39,32, 42, 16, SH110X_BLACK); 
      myOLED.drawLine(42,16, 45, 0, SH110X_WHITE);
      myOLED.display();
    }
  }
  
  void LivesLeftDisplay(int k) {
    myOLED.fillRect(114, 0, 128, 12, SH110X_BLACK);
    if (k>0) {
      myOLED.drawBitmap(120, 1, live_yes, 5, 6, SH110X_WHITE);
      myOLED.setTextColor(SH110X_WHITE);
      myOLED.setCursor(113,0);
      myOLED.print(k);
    } else  {myOLED.drawBitmap(120, 1, live_no, 5, 6, SH110X_WHITE);}
  }
  void GameSlalomInit() {
    LivesLeft = LivesInit;
    FuelLevel = FuelInit;
    GameSpeed = 0;
    GameLevel = 0;
    car_x = random(1,5);
    score = 0;
    FrameDelay = DelayFramesInit;
    OldScoreForLivesAdding =0;

    for (int i = 0; i<6; i++) {
      for (int j = 0; j<4;j++) {
        Sprites_Array[i][j] = CLEAN;
      }
    }
    displaySound2(soundEnabled);
    DisplayRoad();
    DisplayInitTrack();
    LivesLeftDisplay(LivesLeft);
    displayPoints();
    display_roadLine();

  }
  void DisplayInitTrack() {
    for (int row = 0; row < 5; row++){
      for (int column = 0; column < 4; column++) {
        DisplaySprite(row, column, Sprites_Array[row][column]);
      }
    }
  }
  void DisplaySprite(int row, int column, SpriteCode Sprite) {
    int x;
    int y;
    GetSprite_x_y(row, column, &x, &y);
    switch(Sprite) {
      case(GATE):
        if (row == 0)         {myOLED.drawBitmap(x, y, bramka1, sprite_width, Sprite_height, SH110X_WHITE);
        } else if (row == 1)  {myOLED.drawBitmap(x, y, bramka2, sprite_width, Sprite_height, SH110X_WHITE);
        } else if (row == 2)  {myOLED.drawBitmap(x, y, bramka3, sprite_width, Sprite_height, SH110X_WHITE);
        } else if (row == 3)  {myOLED.drawBitmap(x, y, bramka4, sprite_width, Sprite_height, SH110X_WHITE);}
        break;
      case(FUEL):
        if (row == 0)         {
          myOLED.drawBitmap(x, y, fuel1, sprite_width, Sprite_height, SH110X_WHITE);
          //myOLED.drawBitmap(x+sprite_width-6, y-5, paliwo1, paliwo_width, paliwo_height, SH110X_WHITE);
        }
        else if (row == 1)  {
          myOLED.drawBitmap(x, y, fuel2, sprite_width, Sprite_height, SH110X_WHITE);
          //myOLED.drawBitmap(x+sprite_width-6, y-3, paliwo1, paliwo_width, paliwo_height, SH110X_WHITE);
        } else if (row == 2)  {
          myOLED.drawBitmap(x, y, fuel2, sprite_width, Sprite_height, SH110X_WHITE);
          //myOLED.drawBitmap(x+sprite_width-5, y-5, paliwo2, paliwo_width, paliwo_height, SH110X_WHITE);
        } else if (row == 3)  {
          myOLED.drawBitmap(x, y, fuel3, sprite_width, Sprite_height, SH110X_WHITE);
          //myOLED.drawBitmap(x+sprite_width-2, y-5, paliwo3, paliwo_width, paliwo_height, SH110X_WHITE);
        }
        break;
      case(OIL):
        if (row == 0)         {myOLED.drawBitmap(x, y, oil1, sprite_width, Sprite_height, SH110X_WHITE);
        } else if (row == 1)  {myOLED.drawBitmap(x, y, oil2, sprite_width, Sprite_height, SH110X_WHITE);
        } else if (row == 2)  {myOLED.drawBitmap(x, y, oil3, sprite_width, Sprite_height, SH110X_WHITE);
        } else if (row == 3)  {myOLED.drawBitmap(x, y, oil4, sprite_width, Sprite_height, SH110X_WHITE);
        }
        break;
      case(CLEAN): 
      default:
        break;
    }
  }
  void DisplayCar(int car_x) {

    if (car_x<1 || car_x>4) return;
    EraseCarFromDisplay();
    switch(car_x) {
      case(1):
      case(2):
        myOLED.drawBitmap(sprite_x[4][car_x-1], sprite_y[4], motor_2, f1_width, f1_height, SH110X_WHITE);
        break;
      case(3):
        myOLED.drawBitmap(sprite_x[4][car_x-1], sprite_y[4], motor_2, f1_width, f1_height, SH110X_WHITE);
        break;
      case(4):
        myOLED.drawBitmap(sprite_x[4][car_x-1], sprite_y[4], motor_2, f1_width, f1_height, SH110X_WHITE);
        break;
      default:
        break;
    }
  }
  void DisplayRoad() {
    myOLED.drawLine(sprite_x[4][0]-3, SCREEN_HEIGHT, sprite_x[0][0]+4, 0, SH110X_WHITE);
    myOLED.drawLine(sprite_x[4][0]-4, SCREEN_HEIGHT, sprite_x[0][0]+3, 0, SH110X_WHITE);

    myOLED.drawLine(sprite_x[4][3]+f1_width, SCREEN_HEIGHT, sprite_x[0][3]+sprite_width-5, 0, SH110X_WHITE);
    myOLED.drawLine(sprite_x[4][3]+f1_width-1, SCREEN_HEIGHT, sprite_x[0][3]+sprite_width-6, 0, SH110X_WHITE);
  }
  void GetSprite_x_y(int row, int column, int *x, int *y) {
    *x = sprite_x[row][column];
    *y = sprite_y[row];
  }
  void displayPoints() {
    myOLED.setTextSize(1);
    myOLED.setCursor(0,0);
    myOLED.print("F");
    myOLED.print(FuelLevel);
    // myOLED.setCursor(0,14);
    // myOLED.print("score");myOLED.print(score);
    // myOLED.setCursor(0,28);
    // myOLED.print("sped");myOLED.print(FrameDelay);
    
  }
  void RedrawScreen() {
    myOLED.clearDisplay();
    display_roadLine();
  
    UpdateRaceArray();
    displaySound2(soundEnabled);
    DisplayRoad();
    DisplayInitTrack();
    LivesLeftDisplay(LivesLeft);
    displayPoints();
    myOLED.display();
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
  void EraseCarFromDisplay() {
    myOLED.drawBitmap(sprite_x[4][0], sprite_y[4], motor_2, f1_width, f1_height, SH110X_BLACK);
    myOLED.drawBitmap(sprite_x[4][1], sprite_y[4], motor_2, f1_width, f1_height, SH110X_BLACK);
    myOLED.drawBitmap(sprite_x[4][2], sprite_y[4], motor_2, f1_width, f1_height, SH110X_BLACK);
    myOLED.drawBitmap(sprite_x[4][3], sprite_y[4], motor_2, f1_width, f1_height, SH110X_BLACK);
  }
  bool CheckIfNextFrame() {
    if (millis()-LastTime > (FrameDelay)) {
        LastTime  = millis();
        return true;
    }
    return false;
  }
  bool CheckIfRoadLanesSwitch() {
    if (millis()-LastTime2 > (FrameDelay)) {
      LastTime2  = millis();
      return true;
    }
    return false;
  }
  void CheckIfColissionHappen() {
    switch(Sprites_Array[4][car_x-1]) {
      case(CLEAN):
        if (soundEnabled) MyTune(TON_RAMKA_FREQ,30);
        score++;
        FuelLevel--;
        break;
      case(GATE):
        if (soundEnabled) MyTune(TON_ODLICZANIE_FREQ,30);
        FuelLevel--;
        LivesLeft--;
        ColissionDetected();
        break;
      case(OIL):
        FuelLevel--;FuelLevel--;
        score++;score++;
        Poslizg();
        break;
      case (FUEL):
        DisplayTankowanie();
        break;
      default:
        break;
    }
  }
  void DisplayTankowanie() {
    DisplayCar(car_x);
    myOLED.drawBitmap(113, 30, f1_tanking, 18, 18, SH110X_WHITE);
    myOLED.display();
    for (int i = FuelLevel; i < FuelLevel +1+ IncreaseFuel; i++) {
      if (soundEnabled) MyTune(TON_KONIECGRY_FREQ+i, 4);

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
    myOLED.drawBitmap(113, 30, f1_tanking, 18, 18, SH110X_BLACK);
    FuelLevel = FuelLevel -1 + IncreaseFuel;
    FrameDelay = DelayFramesInit;
    myOLED.display();
  }
	void Poslizg() {
    delay(20);
    EraseCarFromDisplay();
    
    if (soundEnabled) MyTune(TON_PUNKT_FREQ,10);
    myOLED.drawBitmap(sprite_x[4][car_x-1], sprite_y[4], rotates0, f1_width, f1_height, SH110X_WHITE);
    myOLED.display();
    myOLED.drawBitmap(sprite_x[4][car_x-1], sprite_y[4], rotates0, f1_width, f1_height, SH110X_BLACK);
    delay(50);

    if (soundEnabled) MyTune(TON_PUNKT_FREQ,10);
    myOLED.drawBitmap(sprite_x[4][car_x-1], sprite_y[4], rotates1, f1_width, f1_height, SH110X_WHITE);
    myOLED.display();
    myOLED.drawBitmap(sprite_x[4][car_x-1], sprite_y[4], rotates1, f1_width, f1_height, SH110X_BLACK);
    delay(50);

    if (soundEnabled) MyTune(TON_PUNKT_FREQ,10);
    myOLED.drawBitmap(sprite_x[4][car_x-1], sprite_y[4], rotates2, f1_width, f1_height, SH110X_WHITE);
    myOLED.display();
    myOLED.drawBitmap(sprite_x[4][car_x-1], sprite_y[4], rotates2, f1_width, f1_height, SH110X_BLACK);
    delay(50);

    if (soundEnabled) MyTune(TON_PUNKT_FREQ,10);
    myOLED.drawBitmap(sprite_x[4][car_x-1], sprite_y[4], rotates3, f1_width, f1_height, SH110X_WHITE);
    myOLED.display();
    myOLED.drawBitmap(sprite_x[4][car_x-1], sprite_y[4], rotates3, f1_width, f1_height, SH110X_BLACK);
    delay(50);
    switch(car_x){
      case(1):
        car_x = car_x + random(0,2);
        break;
      case(2):
      case(3):
        car_x = car_x + random(-1,2);
        break;
      case(4):
        car_x = car_x + random(-1,1);
        break;  
    }
    
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

    colissionsCount++;
    ResetRaceArray();
    displaySound2(soundEnabled);
    DisplayRoad();
    DisplayInitTrack();
    LivesLeftDisplay(LivesLeft);
    displayPoints();
    EraseCarFromDisplay();
    myOLED.drawBitmap(sprite_x[4][car_x-1], sprite_y[4], Wrak, f1_width, f1_height, SH110X_WHITE);
    myOLED.display();
    delay(400);
  }
  void GameOverSlalomDisplay() {
    myOLED.setTextSize(1); 
    myOLED.clearDisplay();
    if (score > SlalomSesionScore) {
      SlalomSesionScore = score;
    }
    int Slalom_highscore;
    EEPROM.get(SlalomRecord, Slalom_highscore);
    if (score > Slalom_highscore) {
      EEPROM.put(SlalomRecord, score);
      EEPROM.commit();
    }
    if (FuelLevel == 0) {
      myOLED.setCursor(1,1);
      myOLED.print("No fuel!");
    }

    myOLED.setTextSize(1); 
    myOLED.setCursor(1,12);
    myOLED.print("Your score:   ");
    myOLED.print(score);
    myOLED.setCursor(1,24);
    myOLED.print("Sesion best:  ");
    myOLED.print(SlalomSesionScore);
    myOLED.setCursor(1,36);
    myOLED.print("All time best: ");
    EEPROM.get(SlalomRecord, Slalom_highscore);
    myOLED.print(Slalom_highscore);
    myOLED.display();

  }
  void CleanRow5() {
    for (int column = 0; column < 4; column++) {
      Sprites_Array[4][column] = CLEAN;
    }
  }
  void CalculateGameSpeedAndLevel() {
    if (score >= 20) FrameDelay = 400;
    if (score >= 30) FrameDelay = 380;
    if (score >= 35) GameLevel = 1;
    if (score >= 40) FrameDelay = 330;
    if (score >= 60) FrameDelay = 300;
    if (score >= 80) FrameDelay = 280;
    if (score >= 100) FrameDelay = 270;
    if (score >= 110) GameLevel = 2;
    if (score >= 120) FrameDelay = 250;
    if (score >= 130) FrameDelay = 230;
    if (score >= 140) FrameDelay = 210;
    if (score >= 150) FrameDelay = 200;

    // if (score == 10) GameLevel = 2;
    // if (score == 15) FrameDelay = 400;
    // if (score == 20) FrameDelay = 380;
    // if (score == 25) FrameDelay = 350;
    // if (score == 30) FrameDelay = 300;
  }
  void checkIfGameOver() {
    if (LivesLeft == 0 || FuelLevel == 0) GameOverSlalom=true;
    if (score - OldScoreForLivesAdding > 40) {
      LivesLeft++;
      for (int i=1; i<4; i++){
        if (soundEnabled) MyTune(TON_ODLICZANIE_FREQ+10*i, 30);
        delay(20);
      }
      OldScoreForLivesAdding = score;
    }
  }
  void AddFuelStation() {
    if (Sprites_Array[0][3] == CLEAN && 
        Sprites_Array[1][3] == CLEAN && 
        FuelLevel < 70 &&
        Sprites_Array[1][3] != FUEL && 
        Sprites_Array[2][3] != FUEL && 
        Sprites_Array[3][3] != FUEL) {

      Sprites_Array[0][3] = CLEAN;
      Sprites_Array[1][3] = FUEL;
      Sprites_Array[2][3] = CLEAN;
      Sprites_Array[3][3] = CLEAN;
    } 
  }
  void AddPlamaOleju() {
    if (random(1,100)<15) {
      if (Sprites_Array[0][0] == GATE && Sprites_Array[0][1]) {
        Sprites_Array[0][1] = OIL;
      } else if ( Sprites_Array[0][1]  && Sprites_Array[0][2] == GATE) {
        Sprites_Array[0][2] = OIL;  
      }
    }
    if (Sprites_Array[3][3] == FUEL)  {Sprites_Array[2][3] = CLEAN;}
  }

  void Pause(){
    myOLED.setTextSize(1); 
    myOLED.setCursor(40,28);
    myOLED.print("PAUSE");
    myOLED.display();
    delay(500);
    while(ReadButton(nullptr, Timer1Sec) == NONE);
    myOLED.setCursor(40,28);
    myOLED.print("     ");
    myOLED.display();
  }

// //############################################

void Game_Slalom() {
  while(1==1) {  
    bool btnActive = false;
    btPressedCode btn;
    myOLED.clearDisplay();
    GameSlalomInit();
    myOLED.display();
    GameOverSlalom = false;

    while (!GameOverSlalom){
      btn = ReadButton(nullptr, Timer1Sec);
      if (btn == UpRight) {
        Pause();
      }
      if (btn == DownLeft && !btnActive) {
        if (car_x > 1) car_x--;
        btnActive = true;
      } else if (btn == DownRight && !btnActive) {
        if (car_x < 4) car_x++;
        btnActive = true;
      } else if (btn == NONE && btnActive) {
        btnActive = false;
      }
      DisplayCar(car_x);

      if (CheckIfNextFrame()) {
        RedrawScreen();
        myOLED.display();
        CheckIfColissionHappen();
        CleanRow5(); // <- po sprawdzeniu kolizji skasuj wiersz testow kolizji
        AddFuelStation();
        AddPlamaOleju();
      }
      CalculateGameSpeedAndLevel();
      myOLED.display();
      checkIfGameOver();
      if (IsPressed(UpLeft) && IsPressed(DownLeft) &&
          IsPressed(UpRight) && IsPressed(DownRight)) return; 
    }
    if (GameOverSlalom) GameOverSlalomDisplay();
    delay(1000);
    WaitforButton();
  }
}


const GameInfo GameInfo_Slalom = {
	"Slalom",
	"Zjeżdżaj, omijaj bramki i przeszkody!",
	Game_Slalom
};
