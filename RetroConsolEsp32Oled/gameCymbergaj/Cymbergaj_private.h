#ifndef CYMBERGAJ_PRIVATE_H
#define CYMBERGAJ_PRIVATE_H

    #define HalfFieldY 31 
    #define HalfFieldX 63
    #define FieldXmax 126
    #define FieldYmax 62
    #define  deltaX 4

    extern const GameInfo* allGames[] ; 
    enum direction {toRight=0, toRightDown, toRightUp, toLeft, toLeftDown, toLeftUp};
    enum collision {noCollision=0, collisionLeft, collisionRight};
    enum speed {Wolno=0, Srednio, Szybko};

    typedef struct cymBallStruct {
      int x;  // wspolrzedna x pilki
      int y;  // wspolrzedna y pilki
      int prevX;
      int prevY;
      unsigned long prevUpdateTime = 0;
      unsigned long updateRate;
      unsigned long updateRateInit;
      direction isHeading;
      int dY;
    };

    typedef struct cymPlayerStruct {
      int y;          //srodek paletkiY
      int score = 0;
    };

    typedef struct cymGameStruct {
      bool updateScreen;
      collision isCollision;
      int aiLevel;
      unsigned long aiPadlePosPrevUpdateTime = 0;
      unsigned long aiPadlePosUpdateRate = 150;
      unsigned long playerPadlePosPrevUpdateTime = 0;
      unsigned long playerPadlePosUpdateRate = 50;
      int maxPoints = 5;
      int padleDeltaMoveY;
      int padleHalfWidth;
      int playersNo;
      int speedInc;
      speed ballSpeed;
    };

    typedef struct  cymMenuStruct {
        const int initX = 7;
        const int initY = 18;
        const int deltaY = 11;
        const int rowMax = 4;
        const int optionsInRow =  sizeof(rowOptionSelect)/sizeof(rowOptionSelect[0]);
        const String optionString[4][4] =  {
            { "Tryb: ", "1 gracz", "2 graczy", "demo", }, 
            { "Szybkosc: ", "wolno", "srednio", "szybko" }, 
            { "AI: ", "amator", "normalny", "geniusz" }, 
            { "Add Speed: ", "0", "1", "2" }
        };
        const int menuOptionValue[4][3] =  {
            { 1, 2, 0 }, 
            { 0, 1, 2 }, 
            { 1, 2, 3 }, 
            { 0, 1, 2 }
        };
        int rowOptionSelect[4] = { 1, 2,  2, 1 };
    };
 


  bool itsGameOver(cymGameStruct& Game, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer);
  bool itsTimeForBallPosUpdate(cymBallStruct& Ball);
  bool itsTimeForPadlePosUpdate(cymGameStruct& Game);
  int updateBallDyAfterPadleHit(int BallX, int BallY, int LeftPadleY, int RightPadleY);
  void updateBallPos(cymGameStruct& Game, cymBallStruct& Ball, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer);
  void updateBallXpos(cymBallStruct& Ball);
  void updateBallYpos(cymBallStruct& Ball, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer);
  void CheckIfBallHitBand(cymBallStruct& Ball);
  void CheckIfLeftPlayerLostGoal(cymGameStruct& Game, cymBallStruct& Ball, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer);
  void CheckIfRightPlayerLostGoal(cymGameStruct& Game, cymBallStruct& Ball, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer);
  void CymInitParams(cymGameStruct& Game, cymBallStruct& Ball, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer);
  void CymParamsUpdateFromUserMenu(cymMenuStruct& Menu, cymGameStruct& Game);
  void DisplayGameSummary(cymGameStruct& Game, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer);
  void DisplayGoal(cymBallStruct& Ball, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer);
  void DisplayScore(cymGameStruct& Game, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer);
  void DrawBall(cymBallStruct& Ball);
  void DrawField();
  void DrawGameScreen(cymGameStruct& Game, cymBallStruct& Ball, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer);
  void DrawPadle(cymGameStruct& Game, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer);
  void Game_Cymbergaj();
  void HandleLostGoal(cymGameStruct& Game, cymBallStruct& Ball, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer);
  void PadlePosAiUpdate(cymGameStruct& Game, cymBallStruct& Ball, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer);
  void PadlePosPlayerUpdate(cymGameStruct& Game, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer);
  void ShowUserMenu(cymMenuStruct& Menu, cymGameStruct& Game);
  void UpdateBallPosAndCheckForColission(cymGameStruct& Game, cymBallStruct& Ball, cymPlayerStruct& LeftPlayer, cymPlayerStruct& RightPlayer);

#endif
