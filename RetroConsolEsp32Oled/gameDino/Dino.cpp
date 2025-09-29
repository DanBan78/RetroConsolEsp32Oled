#include "Dino.h"
#include "Dino_private.h"

namespace DinoGame {

  SpriteSetStr Skins[2] = {
    {dino1, dino2, dino3, obst1, obst2, aero1, aero2, aero3, "Auuuc!!"},      // zestaw 1
    {cat1, cat2, cat3, murek1, murek2, bird1, bird2, bird3, "Miau!"} // zestaw 2
  };

  void Game_Dino() {
    dGameStr DinoGame;
    jumpStr Dino;
    aeroStr Aero;
    obstStr obst1, obst2;
    score = 0;
  
    while(true){
      static const GameConstStr GameConst;
      DinoGame.sessionScore = 0;

      DisplayWelcomeScreen();
      while (1) {
        InitValuesAfterReplay(DinoGame, Dino, Aero, obst1, obst2, GameConst);
        delay(DELAY1000MS);
        FirstGameFrame(DinoGame, Dino, obst1, obst2, GameConst);

        PlayGame(DinoGame, Dino, Aero, obst1, obst2, GameConst);
        DisplayGameSummary(DinoGame, Dino);
        WaitforButton();
        break;
      }
    }
  }

//####################################################################################

void InitValuesAfterReplay(dGameStr& DinoGame, jumpStr& Dino, aeroStr& Aero, obstStr& obst1, obstStr& obst2, const GameConstStr& GameConst) {
  Dino.y = GameConst.jumper_baseY;
  Dino.food = GameConst.jumper_foodInit;
  Dino.state = true;
  Dino.jump = 0;

  score = 0;
  DinoGame.speed = 0;
  pauseGame = false;

  obst1.x = GameConst.first_obst_x;
  obst1.type = OBST_NARROW;
  obst2.x = GameConst.new_obst_x_init;
  obst2.type = OBST_WIDE;

  Aero.x = GameConst.new_obst_x_init;
  Aero.y = GameConst.aero_initY;
  Aero.state = AeroUp;
  Aero.colission = false;
}

void DisplayWelcomeScreen(){
  myOLED.clearDisplay();
  myOLED.setTextColor(SH110X_WHITE);
  myOLED.setTextSize(1);

  myOLED.setTextColor(SH110X_WHITE);
  myOLED.setTextSize(2);
  myOLED.setCursor(10,1);
  myOLED.println("Dino Run");
  myOLED.setTextSize(1);
  myOLED.setCursor(0, 20);
  myOLED.println("o > change skin");
  myOLED.setCursor(0, 31);

  myOLED.println("o > high jump");
  myOLED.setCursor(45, 44);
  myOLED.println("    pause > o");
  myOLED.setCursor(45, 55);

  myOLED.println("long jump > o");
  myOLED.display();
}

void FirstGameFrame(dGameStr& DinoGame, jumpStr& Dino, obstStr& obst1, obstStr& obst2, const GameConstStr& GameConst) {

  myOLED.clearDisplay();
  myOLED.drawBitmap(GameConst.jumper_baseX, GameConst.jumper_baseY, dino1, GameConst.jumper_width, GameConst.jumper_height, SH110X_WHITE);
  MoveTree(DinoGame, obst1, GameConst);
  MoveTree(DinoGame, obst2, GameConst);
  myOLED.drawLine(0, GameConst.baselineY, 127, GameConst.baselineY, SH110X_WHITE);
  myOLED.setCursor(5, 8);
  myOLED.println("Dino jump, run!");
  myOLED.display();
  delay(100);
}

void UpdateGameFrame( dGameStr& DinoGame, jumpStr& Dino, aeroStr& Aero, obstStr& obst1, obstStr& obst2, const GameConstStr& GameConst) {
  myOLED.clearDisplay();
  DisplayScore(DinoGame, Dino);
  displaySoundInfo(120, 0, soundEnabled);
  if (Dino.food <= 0) DisplayOutOfFoodInfo();

  MoveTree(DinoGame,obst1, GameConst);
  MoveTree(DinoGame,obst2, GameConst);
  MoveDino(DinoGame, Dino, GameConst);
  MoveAero(DinoGame, Aero, GameConst);
  myOLED.drawLine(0, GameConst.baselineY, 127, GameConst.baselineY, SH110X_WHITE);
}

void DisplayOutOfFoodInfo(){
  myOLED.setTextSize(1);
  myOLED.setCursor(15,20);
  myOLED.print("No Food - no jump!");
}

// Main play function
void PlayGame(dGameStr& DinoGame, jumpStr& Dino, aeroStr& Aero, obstStr& obst1, obstStr& obst2, const GameConstStr& GameConst) {
  while (true) {
    myOLED.clearDisplay();
    CheckButtons(DinoGame, Dino);
    if ((Dino.food <= 0) && (Dino.jump == 1)) Dino.jump = 0;
    if (Dino.jump != 0) {
      if (Dino.jumpType) {
        JumpHigh(DinoGame, Dino, GameConst);
      } else {
        JumpLong(DinoGame, Dino, GameConst);
      }
    }
    UpdateGameFrame(DinoGame, Dino, Aero, obst1, obst2, GameConst);
    CheckIfAeroColision(DinoGame, Dino, Aero, GameConst);
    CalcTreesXpos(DinoGame, obst1, obst2, GameConst);
    CalcAeroXYpos(DinoGame, Aero, GameConst);
    CalcGameSpeed(DinoGame, GameConst);
    if (CheckIfTreeCollision(DinoGame, Dino, obst1, GameConst) || CheckIfTreeCollision(DinoGame, Dino, obst2, GameConst)) {
      GameOverTreeColisionDetected(DinoGame, Dino, GameConst);
      myOLED.display();
      delay(50);
      return;
    }
    myOLED.display();
    delay(GameConst.maxspeed - DinoGame.speed);
    if (pauseGame) GamePause();
  }
}
  
void GamePause() {
  myOLED.setTextSize(2);
  myOLED.setCursor(20,18);
  myOLED.print("PAUSE");
  myOLED.display();
  myOLED.setTextSize(1);
  delay(300);
  pauseGame = false;
  WaitforButton();
}

void TurboSpeed(dGameStr& DinoGame, bool direction, int16_t* sprite) {
  if (direction) {
    (*sprite)++;
    if (score >60) (*sprite)++;
    if (score >100) (*sprite)++;
    if (score >150) (*sprite)++;
  } else {
    (*sprite)--;
    if (score >60) (*sprite)--;
    if (score >100) (*sprite)--;
    if (score >150) (*sprite)--;
  }
}

void MoveDino(dGameStr& DinoGame, jumpStr& Dino, const GameConstStr& GameConst) {
  if (DinoGame.isRect) {
    int dinoCenterX       = GameConst.jumper_baseX + GameConst.jumper_width/2;
    int dinoColAreaStart  = dinoCenterX - GameConst.jumper_xColRange;
    int dinoColAreaWidth  = 2*GameConst.jumper_xColRange;
    int dinoTop           = Dino.y;
    int dinoBottom        = GameConst.jumper_height;
    myOLED.fillRect(dinoColAreaStart, dinoTop, dinoColAreaWidth, dinoBottom, SH110X_WHITE);
  } else {
    if (Dino.jump == 0) {
      if (Dino.state) {
        myOLED.drawBitmap(GameConst.jumper_baseX, Dino.y, Skins[DinoGame.spriteSetIndex].Jumper1, GameConst.jumper_width, GameConst.jumper_height, SH110X_WHITE);
      } else {
        myOLED.drawBitmap(GameConst.jumper_baseX, Dino.y, Skins[DinoGame.spriteSetIndex].Jumper2, GameConst.jumper_width, GameConst.jumper_height, SH110X_WHITE);
      }
      Dino.state = !Dino.state;
    } else {
      myOLED.drawBitmap(GameConst.jumper_baseX, Dino.y, Skins[DinoGame.spriteSetIndex].Jumper3, GameConst.jumper_width, GameConst.jumper_height, SH110X_WHITE);
    }
  }
}

void MoveAero(dGameStr& DinoGame, aeroStr& Aero, const GameConstStr& GameConst) {
  if (DinoGame.isRect) {
    int AeroColAreaStart   = Aero.x;
    int AeroColAreaWidth  = GameConst.aero_width;
    int AeroTop    = Aero.y;
    int AeroBottom = GameConst.aero_height;
    myOLED.fillRect(AeroColAreaStart, AeroTop, AeroColAreaWidth, AeroBottom, SH110X_WHITE);
    if(Aero.colission) Aero.y+=4;
    return;
  } else {
    if (!Aero.colission) {
      if (random(0,2) == 1) {
        myOLED.drawBitmap(Aero.x, Aero.y, Skins[DinoGame.spriteSetIndex].Aero1, GameConst.aero_width, GameConst.aero_height, SH110X_WHITE);
      } else {
        myOLED.drawBitmap(Aero.x, Aero.y, Skins[DinoGame.spriteSetIndex].Aero2, GameConst.aero_width, GameConst.aero_height, SH110X_WHITE);
      }
    } else {
      Aero.y+=4;
      myOLED.drawBitmap(Aero.x, Aero.y, Skins[DinoGame.spriteSetIndex].Aero3, GameConst.aero_width, GameConst.aero_height, SH110X_WHITE);
    }
  }
}

void MoveTree(dGameStr& DinoGame, obstStr& tree, const GameConstStr& GameConst) {
  int treeCenterX       = tree.x + ObstParams[tree.type].width / 2;
  int treeColAreaStart  = treeCenterX - ObstParams[tree.type].xColisionRange;
  int treeColAreaWidth  = 2 * ObstParams[tree.type].xColisionRange;
  int treeTop           = GameConst.baselineY - ObstParams[tree.type].yColisionRange;
  int treeHeight        = ObstParams[tree.type].yColisionRange;

  if (DinoGame.isRect) {
    if(tree.type==OBST_NARROW) myOLED.fillRect(treeColAreaStart, treeTop, treeColAreaWidth, treeHeight, SH110X_WHITE);
    if(tree.type==OBST_WIDE) myOLED.fillRect(treeColAreaStart, treeTop, treeColAreaWidth, treeHeight, SH110X_WHITE);
  } else {
    if(tree.type==OBST_NARROW) myOLED.drawBitmap(tree.x, GameConst.obst_y, Skins[DinoGame.spriteSetIndex].ObstHigh, ObstParams[OBST_NARROW].width, ObstParams[OBST_NARROW].height, SH110X_WHITE);
    if(tree.type==OBST_WIDE) myOLED.drawBitmap(tree.x, GameConst.obst_y, Skins[DinoGame.spriteSetIndex].ObstWide, ObstParams[OBST_WIDE].width, ObstParams[OBST_WIDE].height, SH110X_WHITE);
  }
}

void GameOverTreeColisionDetected(dGameStr& DinoGame, jumpStr& Dino, const GameConstStr& GameConst) {
  myOLED.setTextSize(1);
  myOLED.setCursor((GameConst.jumper_baseX + GameConst.jumper_width + 4), Dino.y-4);
  myOLED.println(Skins[DinoGame.spriteSetIndex].collisionText);
  myOLED.display();
  if(soundEnabled){
    tone(BUZZER_PIN,TON_PUNKT_CZAS,DELAY100MS);
    delay(DELAY100MS);
    tone(BUZZER_PIN,TON_PALETKA_FREQ,DELAY100MS);
    delay(DELAY100MS);
    tone(BUZZER_PIN,TON_ODLICZANIE_FREQ,DELAY250MS);
    delay(DELAY250MS);
    noTone(BUZZER_PIN);
  } else {
    delay(DELAY1000MS);
  }
  myOLED.clearDisplay();
}

void CheckIfAeroColision(dGameStr& DinoGame, jumpStr& Dino, aeroStr& Aero, const GameConstStr& GameConst) {
  if ((((Aero.x < GameConst.jumper_baseX + GameConst.jumper_xColRange) ||
      (Aero.x + GameConst.aero_width < GameConst.jumper_baseX - GameConst.jumper_xColRange) ) &&
      (Aero.y + GameConst.aero_height > Dino.y)) && !Aero.colission) {
    Aero.colission = true;
    Dino.food = Dino.food + 5;
    if(soundEnabled) {   
      tone(BUZZER_PIN,TON_PUNKT_FREQ,DELAY100MS);
      tone(BUZZER_PIN,TON_PUNKT_FREQ,DELAY100MS);
      noTone(BUZZER_PIN);
    } else delay(DELAY50MS);
  }
  if ((Aero.colission) && ((Aero.y>64)||(Aero.x< -30))) {
    Aero.colission = false;
    Aero.x = GameConst.new_obst_x_init + random(0,40);
    Aero.y = GameConst.aero_initY + random(-5,6);
  }
}


void JumpLong(dGameStr& DinoGame, jumpStr& Dino, const GameConstStr& GameConst) {
	if(Dino.jump == 1){
    Dino.jumpStartX = 0;
		if (soundEnabled && !sounddone) void JumpSound();
    Dino.y-=3;
    TurboSpeed(DinoGame,false, &Dino.y);

    if(Dino.y < (GameConst.jumper_baseY-GameConst.jumper_jumpHeight/2)){
      Dino.y = (GameConst.jumper_baseY-GameConst.jumper_jumpHeight/2);
      Dino.food--;
      Dino.jump = 2;
    }
  } else if (Dino.jump==2){
    Dino.jumpStartX += 1;
    if (Dino.jumpStartX > 10)  Dino.jump = 3;
  } else if (Dino.jump==3){
    Dino.y+=3;
      TurboSpeed(DinoGame,true, &Dino.y);
    if (Dino.y >= GameConst.jumper_baseY){
      Dino.y = GameConst.jumper_baseY;
      Dino.jump = 0;
      Dino.jumpStartX = 0;
      sounddone = false;
      score++;
    }
  }
}

void JumpHigh(dGameStr& DinoGame, jumpStr& Dino, const GameConstStr& GameConst) {
	if(Dino.jump == 1){
    Dino.jumpStartX = 0;
		if (soundEnabled && !sounddone) void JumpSound();
    Dino.y-=3;
    TurboSpeed(DinoGame,false, &Dino.y);

    if(Dino.y < (GameConst.jumper_baseY-GameConst.jumper_jumpHeight)){
      Dino.y = (GameConst.jumper_baseY-GameConst.jumper_jumpHeight);
      Dino.food--;
      Dino.food--;
      Dino.jump = 2;
    }
  } else if (Dino.jump==2){
    Dino.jumpStartX += 1;
    if (Dino.jumpStartX > 6)  Dino.jump = 3;
  } else if (Dino.jump==3){
    Dino.y+=7;
      TurboSpeed(DinoGame,true, &Dino.y);
    if (Dino.y >= GameConst.jumper_baseY){
      Dino.y = GameConst.jumper_baseY;
      Dino.jump = 0;
      Dino.jumpStartX = 0;
      sounddone = false;
      score++;
    }
  }
}


bool CheckIfTreeCollision(dGameStr& DinoGame, jumpStr& Dino, obstStr& tree, const GameConstStr& GameConst) {
  int dinoCenter        = GameConst.jumper_baseX + GameConst.jumper_width/2;
  int dinoColAreaStart  = dinoCenter - GameConst.jumper_xColRange;
  int dinoColAreaWidth  = 2 * GameConst.jumper_xColRange;
  int dinoTop           = Dino.y;
  int dinoBottom        = Dino.y + GameConst.jumper_height;
  int treeCenter        = tree.x + ObstParams[tree.type].width/2;
  int treeColAreaStart  = treeCenter - ObstParams[tree.type].xColisionRange;
  int treeColAreaWidth  = 2 * ObstParams[tree.type].xColisionRange;
  int treeTop           = GameConst.baselineY-ObstParams[tree.type].yColisionRange; 
  int treeHeight        = ObstParams[tree.type].yColisionRange;

  bool overlapX = (dinoColAreaStart + dinoColAreaWidth > treeColAreaStart) &&
                  (dinoColAreaStart < treeColAreaStart + treeColAreaWidth);
  bool overlapY = dinoBottom > treeTop;

  return overlapX && overlapY;
}

void CalcTreesXpos (dGameStr& DinoGame, obstStr& obst1, obstStr& obst2, const GameConstStr& GameConst) {
  int aerodist = abs(obst2.x-obst1.x);
  //displayVar(aerodist);
	obst1.x-=2;
  TurboSpeed(DinoGame,false, &obst1.x);
	obst2.x-=2;
	TurboSpeed(DinoGame,false, &obst2.x);

	if (obst1.x<-ObstParams[obst1.type].xColisionRange) {
		obst1.x = GameConst.new_obst_x_init + random(0,20);
		if (random(0,2)==1) {
			obst1.type =  true;
		} else obst1.type = false;
	}
	if (obst2.x<-ObstParams[obst2.type].xColisionRange) {
		obst2.x = GameConst.new_obst_x_init + random(0,20);
		if (random(0,2)==1) {
			obst2.type = true;
		} else obst2.type = false;
	}
	if (aerodist < 50) {
		if (obst2.x > obst1.x) {
			obst2.x = obst2.x + 45;
		} else obst1.x = obst1.x + 45;
	}    
}

void CalcAeroXYpos(dGameStr& DinoGame, aeroStr& Aero, const GameConstStr& GameConst) {
	int dir_y = random(0, 2);
	if (!Aero.colission) {
		Aero.x = Aero.x - random(2,6);
		if (dir_y == 0) {
			Aero.y = Aero.y + random(0,4);
		} else {
		  Aero.y = Aero.y - random  (0,4);
		}	
		if (Aero.y > 20) Aero.y = 18;
		if (Aero.y < 1) Aero.y = 2;
  } else {
		Aero.y += 2;
		Aero.x = Aero.x - 5;
	}

  if ((Aero.x < 0 - GameConst.aero_width) || (Aero.y < 0)) {
    Aero.x = GameConst.new_obst_x_init + random(30,100);
    Aero.y = (GameConst.aero_initY);
    Aero.colission = false;
  }
}

void CalcGameSpeed(dGameStr& DinoGame, const GameConstStr& GameConst) {
	DinoGame.speed = score/3;
  if (DinoGame.speed > GameConst.maxspeed) DinoGame.speed = GameConst.maxspeed;
}

void ShowScore(dGameStr& DinoGame){
  if ((score > DinoGame.sessionScore)) {
    DinoGame.sessionScore = score;
  }
  myOLED.setCursor(1,4);
  myOLED.print("Your score:   ");
  myOLED.print(score);
  myOLED.setCursor(1,20);
  myOLED.print("Session best:  ");
  myOLED.print(DinoGame.sessionScore);
  myOLED.setCursor(1,36);
  myOLED.print("All time best: ");
  int highscore;
  EEPROM.get(Game_DinoRecord, highscore);
  myOLED.print(highscore);
  myOLED.display();
}

// Game over display with score
void DisplayGameSummary(dGameStr& DinoGame, jumpStr& Dino) {
  myOLED.clearDisplay();
  myOLED.setTextSize(2);
  myOLED.setCursor(10,20);
  myOLED.print("Game over ");
  myOLED.display();
  delay(DELAY500MS);
  myOLED.setTextSize(1);
  myOLED.clearDisplay();

  int highscore;
  EEPROM.get(Game_DinoRecord, highscore);
  score = score + Dino.food;
  if ((score > highscore)) {
    EEPROM.put(Game_DinoRecord, score);
    EEPROM.commit();
  }

  ShowScore(DinoGame);

  myOLED.setCursor(1,54);
  myOLED.println("Push button to Play!");
  myOLED.display();
  delay(300);
}

// Display score while running the game
void DisplayScore(dGameStr& DinoGame, jumpStr& Dino) {
  myOLED.setCursor(1,2);
  myOLED.print("food ");
  myOLED.print(Dino.food);
  myOLED.setCursor(61,2);
  myOLED.print("Jump: ");
  myOLED.print(score);
}

void JumpSound(){
  if (soundEnabled && !sounddone) {
    tone(BUZZER_PIN,TON_ODLICZANIE_FREQ,DELAY100MS);
    delay(50);
    tone(BUZZER_PIN,TON_PUNKT_CZAS,DELAY100MS);
    delay(50);
    noTone(BUZZER_PIN);
    sounddone = true;
  }
}

void CheckButtons(dGameStr& DinoGame, jumpStr& Dino) {
  if(IsPressed(UpLeft)) {
    DinoGame.spriteSetIndex++;
    if (DinoGame.spriteSetIndex > 1){
      DinoGame.spriteSetIndex = 0;
    } 
    while(IsPressed(UpLeft));
  }

  if (IsPressed(DownLeft)) {
    if (Dino.jump==0) Dino.jump=1;
    Dino.jumpType = true; // true - high jump
  }

  if (IsPressed(DownRight)) {
    if (Dino.jump==0) Dino.jump=1;
    Dino.jumpType = false; // false - long jump
  }

  if (IsPressed(UpRight)) {
    pauseGame = !pauseGame;
    while (IsPressed(UpRight));
  }
}

} // namespace DinoGame

const GameInfo GameInfo_Dino = {
  "Dino",
  "Unikaj przeszkód, skacz dinozaurem!",
  DinoGame::Game_Dino,
  Game_DinoRecord
};
