#include "Dino.h"
#include "Dino_private.h"


  void Game_Dino() {
    dGameStruct DinoGame;
    dinoStruct Dino;
    pteroStruct Ptero;
    treeStruct tree1, tree2;
    while(true){
      static const GameConstStruct GameConst = {
        .StartTreeX = 120,
        .NewTreeXInit = 160,
        .minDeltaTrees = 10,
        .baselineY = 54,
        .maxspeed = 20,
        .tree_y = 35,
        .dino_foodInit = 8,
        .dino_baseX = 10,
        .dino_baseY = 29,
        .dino_xColRange = 3,
        .dino_width = 25,
        .dino_height = 26,
        .dino_jumpHeight = 26,
        .ptero_width = 16,
        .ptero_height = 10,
        .ptero_initY = 12};
      DinoGame.sessionScore = 0;
      Dino.colision = false;

      IntroMessage();
      while (1) {
        ReplayInitValues(DinoGame, Dino, Ptero, tree1, tree2, GameConst);
        WaitforButton();
        RenderScene(DinoGame, Dino, tree1, tree2, GameConst);

        Play(DinoGame, Dino, Ptero, tree1, tree2, GameConst);
        if (AllButtonsPressedCheck()) return;

        displaySummary(DinoGame, Dino);
        WaitforButton();
        break;
      }
    }
  }


//####################################################################################

void ReplayInitValues(dGameStruct& DinoGame, dinoStruct& Dino, pteroStruct& Ptero, treeStruct& tree1, treeStruct& tree2, const GameConstStruct& GameConst) {
  Dino.y = GameConst.dino_baseY;
  Dino.food = GameConst.dino_foodInit;
  Dino.state = true;
  Dino.jump = 0;

  DinoGame.score = 0;
  DinoGame.speed = 0;
  DinoGame.pause = false;

  tree1.x = GameConst.StartTreeX;
  tree1.type = TREE_NARROW;
  tree2.x = GameConst.NewTreeXInit;
  tree2.type = TREE_WIDE;

  Ptero.x = GameConst.NewTreeXInit;
  Ptero.y = GameConst.ptero_initY;
  Ptero.state = PteroUp;
  Ptero.colission = false;
};

void IntroMessage(){
  myOLED.clearDisplay();
  myOLED.setTextColor(SH110X_WHITE);
  myOLED.setTextSize(2);
  myOLED.setCursor(10,5);
  myOLED.println("Dino Run");
  myOLED.setTextSize(1);
  myOLED.setCursor(5,45);
  myOLED.println("Push button to Play");
  myOLED.display();
}

void RenderScene(dGameStruct& DinoGame, dinoStruct& Dino, treeStruct& tree1, treeStruct& tree2, const GameConstStruct& GameConst) {
  myOLED.clearDisplay();
  myOLED.drawBitmap(GameConst.dino_baseX, GameConst.dino_baseY, dino1, GameConst.dino_width, GameConst.dino_height, SH110X_WHITE);
  // Rysuj drzewa przez moveTree, aby uniknąć błędu typów
  moveTree(tree1, GameConst);
  moveTree(tree2, GameConst);
  myOLED.drawLine(0, GameConst.baselineY, 127, GameConst.baselineY, SH110X_WHITE);
  myOLED.setCursor(5, 8);
  myOLED.println("Dino jump, run!");
  myOLED.display();
  delay(100);
}

void DisplayGameFrame( dGameStruct& DinoGame, dinoStruct& Dino, pteroStruct& Ptero, treeStruct& tree1, treeStruct& tree2, const GameConstStruct& GameConst) {
  myOLED.clearDisplay();
  displayScore(DinoGame, Dino);
  displaySound(soundEnabled);

  moveTree(tree1, GameConst);
  moveTree(tree2, GameConst);
  moveDino(Dino, GameConst);
  movePtero(Ptero, GameConst);
  myOLED.drawLine(0, GameConst.baselineY, 127, GameConst.baselineY, SH110X_WHITE);
}

void OutOfFood(){
  myOLED.setTextSize(1);
  myOLED.setCursor(15,55);
  myOLED.print("No Food - no jump!");
  myOLED.display();
}

// Main play function
void Play(dGameStruct& DinoGame, dinoStruct& Dino, pteroStruct& Ptero, treeStruct& tree1, treeStruct& tree2, const GameConstStruct& GameConst) {
  while (true) {
    myOLED.clearDisplay();
    if (AllButtonsPressedCheck()) return;
    if (IsPressed(DownLeft) || IsPressed(DownRight)) {
      if (Dino.jump==0) Dino.jump=1;
    }
    if (IsPressed(UpRight)) {
      DinoGame.pause = true;
      while (IsPressed(UpRight));
    }

    if ((Dino.food == 0) && Dino.jump == 1) Dino.jump = 0;
    if (Dino.jump != 0) DinoJump(DinoGame, Dino, GameConst);
    DisplayGameFrame(DinoGame, Dino, Ptero, tree1, tree2, GameConst);
    CheckPteroColision(DinoGame, Dino, Ptero, GameConst);
    CalcTreesAnimations(DinoGame, tree1, tree2, GameConst);
    CalcPteroAnimations(DinoGame, Ptero, GameConst);
    CalcGameSpeed(DinoGame, GameConst);
    if ((Dino.food == 0)) OutOfFood();
    if (CheckCollision(DinoGame, Dino, tree1, GameConst) || CheckCollision(DinoGame, Dino, tree2, GameConst)) Dino.colision = true;
    if (Dino.colision) {
      ColisionDetected(Dino, GameConst);
      myOLED.display();
      delay(50);
      return;
    }
    myOLED.display();
    delay(GameConst.maxspeed - DinoGame.speed);
    if (DinoGame.pause) DinoGamePause();
  }
}
  
void DinoGamePause() {
  myOLED.setTextSize(2);
  myOLED.setCursor(20,18);
  myOLED.print("PAUSE");
  myOLED.display();
  myOLED.setTextSize(1);
  delay(300);
  pauseGame = false;
  WaitforButton();
}

void ExtraSpeed(dGameStruct& DinoGame, bool direction, uint8_t* sprite) {
  if (direction) {
    (*sprite)++;
    if (DinoGame.score >60) (*sprite)++;
    if (DinoGame.score >100) (*sprite)++;
    if (DinoGame.score >150) (*sprite)++;
  } else {
    (*sprite)--;
    if (DinoGame.score >60) (*sprite)--;
    if (DinoGame.score >100) (*sprite)--;
    if (DinoGame.score >150) (*sprite)--;
  }
}

// Move dino function
void moveDino(dinoStruct& Dino, const GameConstStruct& GameConst) {
  if (Dino.jump == 0) {
    if (Dino.state) {
      myOLED.drawBitmap(GameConst.dino_baseX, Dino.y, dino1, GameConst.dino_width, GameConst.dino_height, SH110X_WHITE);
    } else {
      myOLED.drawBitmap(GameConst.dino_baseX, Dino.y, dino2, GameConst.dino_width, GameConst.dino_height, SH110X_WHITE);
    }
    Dino.state = !Dino.state;
  } else {
    myOLED.drawBitmap(GameConst.dino_baseX, Dino.y, dino1, GameConst.dino_width, GameConst.dino_height, SH110X_WHITE);
  }
}
// Move ptero function
void movePtero(pteroStruct& Ptero, const GameConstStruct& GameConst) {
  if (!Ptero.colission) {
    if (random(0,2) == 1) {
        myOLED.drawBitmap(Ptero.x, Ptero.y, ptero1, GameConst.ptero_width, GameConst.ptero_height, SH110X_WHITE);
    } else {
      myOLED.drawBitmap(Ptero.x, Ptero.y, ptero2, GameConst.ptero_width, GameConst.ptero_height, SH110X_WHITE);
    }
  } else {
    myOLED.drawBitmap(Ptero.x, Ptero.y, ptero3, GameConst.ptero_width, GameConst.ptero_height, SH110X_WHITE);
  }
}
// Move tree funciton
void moveTree(treeStruct& tree, const GameConstStruct& GameConst) {
  if(tree.type==TREE_NARROW){
    myOLED.drawBitmap(tree.x, GameConst.tree_y, tree1, TreeParams[TREE_NARROW].width, TreeParams[TREE_NARROW].height, SH110X_WHITE);
  }
  else if(tree.type==TREE_WIDE){
    myOLED.drawBitmap(tree.x, GameConst.tree_y, tree2, TreeParams[TREE_WIDE].width, TreeParams[TREE_WIDE].height, SH110X_WHITE);
  }
}

void ColisionDetected(dinoStruct& Dino, const GameConstStruct& GameConst) {
  myOLED.setTextSize(1);
  myOLED.setCursor((GameConst.dino_baseX + GameConst.dino_width + 4), Dino.y-4);
  myOLED.println("Auuuc!!");
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

void CheckPteroColision(dGameStruct& DinoGame, dinoStruct& Dino, pteroStruct& Ptero, const GameConstStruct& GameConst) {
  if ((Ptero.x < GameConst.dino_baseX + GameConst.dino_xColRange) &&
   !(GameConst.dino_baseX - GameConst.dino_xColRange < Ptero.x + GameConst.ptero_width) &&
     (Ptero.y + GameConst.ptero_height > Dino.y) && !Ptero.colission) {
    Ptero.colission = true;
    Dino.food = Dino.food + 5;
    if(soundEnabled) {   
      tone(BUZZER_PIN,TON_PUNKT_FREQ,DELAY100MS);
      delay(DELAY100MS);
      tone(BUZZER_PIN,TON_PUNKT_FREQ,DELAY100MS);
      delay(DELAY100MS);
      noTone(BUZZER_PIN);
    } else delay(DELAY250MS);
  }
  if ((Ptero.colission) && ((Ptero.y>64)||(Ptero.x< -30))) {
    Ptero.colission = false;
    Ptero.x = GameConst.NewTreeXInit + random(0,40);
    Ptero.y = GameConst.ptero_initY + random(-5,6);
  }
}

void DinoJump(dGameStruct& DinoGame, dinoStruct& Dino, const GameConstStruct& GameConst) {
	if(Dino.jump == 1){
		if (soundEnabled) {
			  tone(BUZZER_PIN,TON_ODLICZANIE_FREQ,DELAY100MS);
			delay(50);
			tone(BUZZER_PIN,TON_PUNKT_CZAS,DELAY100MS);
      delay(50);
      noTone(BUZZER_PIN);
    }
    Dino.y-=2;
    ExtraSpeed(DinoGame,false, &Dino.y);

    if(Dino.y <= (GameConst.dino_baseY-GameConst.dino_jumpHeight)){
      Dino.y = (GameConst.dino_baseY-GameConst.dino_jumpHeight);
      Dino.food--;
      Dino.jump = 2;
    }
  } else if (Dino.jump==2){
    Dino.y+=2;
     ExtraSpeed(DinoGame,true, &Dino.y);
    if (Dino.y >= GameConst.dino_baseY){
      Dino.y = GameConst.dino_baseY;
      Dino.jump = 0; 
      score++;
    }
  }
}

bool CheckCollision(dGameStruct& DinoGame, dinoStruct& Dino, treeStruct& tree, const GameConstStruct& GameConst) {
    int dinoLeft   = GameConst.dino_baseX;
    int dinoRight  = GameConst.dino_baseX + GameConst.dino_width;
    int dinoTop    = Dino.y;
    int dinoBottom = Dino.y + GameConst.dino_height;

    int treeLeft   = tree.x;
    int treeRight  = tree.x + TreeParams[tree.type].width;
    int treeTop    = GameConst.baselineY - TreeParams[tree.type].height; // zwykle 35
    int treeBottom = GameConst.baselineY;

    bool overlapX = dinoRight > treeLeft && dinoLeft < treeRight;
    bool overlapY = dinoBottom > treeTop && dinoTop < treeBottom;

    return overlapX && overlapY;
}

void CalcTreesAnimations (dGameStruct& DinoGame, treeStruct& tree1, treeStruct& tree2, const GameConstStruct& GameConst) {
  int treedist = abs(tree2.x-tree1.x);
  //displayVar(treedist);
	tree1.x-=2;
  ExtraSpeed(DinoGame,false, &tree1.x);
	tree2.x-=2;
	ExtraSpeed(DinoGame,false, &tree2.x);

	if (tree1.x<-10) {
		tree1.x = GameConst.NewTreeXInit + random(0,  0);
		if (random(0,2)==1) {
			tree1.type =  true;
		} else tree1.type = false;
	}
	if (tree2.x<-10) {
		tree2.x = GameConst.NewTreeXInit + random(0,20);
		if (random(0,2)==1) {
			tree2.type = true;
		} else tree2.type = false;
	}
	if (treedist > 8 && treedist < 20) {
		if (tree2.x > tree1.x) {
			tree2.x = tree2.x + 30;
		} else tree1.x = tree1.x + 30;
	}       
  //displayVar(treedist);
}

void CalcPteroAnimations(dGameStruct& DinoGame, pteroStruct& Ptero, const GameConstStruct& GameConst) {
	int dir_y = random(0, 2);
	if (!Ptero.colission) {
		Ptero.x = Ptero.x - random(2,6);
		if (dir_y == 0) {
			Ptero.y = Ptero.y + random(0,4);
		} else {
		  Ptero.y = Ptero.y - random  (0,4);
		}	
		if (Ptero.y > 22) Ptero.y = 18;
		if (Ptero.y < 1) Ptero.y = 2;
		else {
			Ptero.y += 2;
			Ptero.x = Ptero.x - 3;
		}

    if ((Ptero.x < 0 - GameConst.ptero_width) || (Ptero.y < 0)) {
      Ptero.x = GameConst.NewTreeXInit + random(0,80);
      Ptero.y = (GameConst.ptero_initY);
      Ptero.colission = false;
    }
  }
}

void CalcGameSpeed(dGameStruct& DinoGame, const GameConstStruct& GameConst) {
	DinoGame.speed = DinoGame.score/3;
  if (DinoGame.speed > GameConst.maxspeed) DinoGame.speed = GameConst.maxspeed;
}

void ShowScore(dGameStruct& DinoGame){
  if ((DinoGame.score > DinoGame.sessionScore)) {
    DinoGame.sessionScore = DinoGame.score;
  }
  myOLED.setCursor(1,4);
  myOLED.print("Your score:   ");
  myOLED.print(DinoGame.score);
  myOLED.setCursor(1,20);
  myOLED.print("Session best:  ");
  myOLED.print(DinoGame.sessionScore);
  myOLED.setCursor(1,36);
  myOLED.print("All time best: ");
  int highscore;
  EEPROM.get(DinoRecord, highscore);
  myOLED.print(highscore);
  myOLED.display();
}

// Game over display with score
void displaySummary(dGameStruct& DinoGame, dinoStruct& Dino) {
  myOLED.clearDisplay();
  myOLED.setTextSize(2);
  myOLED.setCursor(10,20);
  myOLED.print("Game over ");
  myOLED.display();
  delay(DELAY500MS);
  myOLED.setTextSize(1);
  myOLED.clearDisplay();

  int highscore;
  EEPROM.get(DinoRecord, highscore);
  DinoGame.score = DinoGame.score + Dino.food;
  if ((DinoGame.score > highscore)) {
    EEPROM.put(DinoRecord, DinoGame.score);
    EEPROM.commit();
  }

  ShowScore(DinoGame);

  myOLED.setCursor(1,54);
  myOLED.println("Push button to Play!");
  myOLED.display();
  delay(300);
}

// Display score while running the game
void displayScore(dGameStruct& DinoGame, dinoStruct& Dino) {
  myOLED.setCursor(1,2);
  myOLED.print("food ");
  myOLED.print(Dino.food);
  myOLED.setCursor(61,2);
  myOLED.print("Jump: ");
  myOLED.print(DinoGame.score);
}



const GameInfo GameInfo_Dino = {
  "Dino",
  "Unikaj przeszkód, skacz dinozaurem!",
  Game_Dino
};
