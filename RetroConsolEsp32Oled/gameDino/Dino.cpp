#include "Dino.h"
#include "Dino_private.h"

  bool tempBool = false;
  int food = FOOD_INIT;
  int maxspeed = 60;
  int jump = 0;
  int DinoSesionScore = 0;

  int dino_y = DINO_INIT_Y;
  int tree_type = 0;
  int tree_type1 = 1;
  int tree_x = BASE_LINE_X1_INIT;
  int tree1_x = BASE_LINE_X2;
  
  int ptero_x = PTERO_INIT_X;
  int ptero_y = PTERO_INIT_Y;
	
  bool frameswitch = false;
  bool pteroColission = false;

  void Game_Dino() {
    InitValues();
    introMessage();

    while (1) {
      ReplayInitValues();
      WaitforButton();
      renderScene();
      
      play();
      displaySummary(score);
      WaitforButton();
      return;
    }
  }


//####################################################################################
// Main play function
void play() {
  bool btnPressed = false;
  Serial.print("Dino Init");
  SerialPrintFreeRam();

  while (1==1) {
    myOLED.clearDisplay();
    btPressedCode btn = ReadButton(nullptr, Timer1Sec);
    if (!btnPressed) {
      if ((btn==DownLeft)||(btn==DownRight)) {
        if(jump==0) jump=1;
      } else if (btn==UpLeft){
          pauseGame = true;
      }
      btnPressed = true;
    }
    if (btn==NONE) btnPressed = false;

      if ((food==0) && jump ==1) jump = 0;
      if (jump !=0) dino_jump(&jump, &dino_y);

      displayGameFrame();
      checkPteroColision();
      CalcTreesAnimations();
      CalcGameSpeed();
      if ((food==0)) OutOfFood();

      if (CheckCollision()) {
        if(CheckCollision()) ColisionDetected(dino_y);
      myOLED.display();
        delay(50);
      return;
    }
    myOLED.display();
    delay(maxspeed-GameSpeed);
    if (pauseGame) {
      myOLED.setTextSize(2);
        myOLED.setCursor(20,18);
      myOLED.print("PAUSE");
      myOLED.display();
      myOLED.setTextSize(1);
      delay(300);
      pauseGame = false;
      WaitforButton();
    }
  }
}	

void ExtraSpeed(int direction, int* sprite) {
	if (direction==1) {
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

// Move dino function
void moveDino(int *y){
	if (jump == 0) {
		if (frameswitch) {
			myOLED.drawBitmap(DINO_INIT_X, *y, dino1, DINO_WIDTH, DINO_HEIGHT, SH110X_WHITE);
		} else {
			myOLED.drawBitmap(DINO_INIT_X, *y, dino2, DINO_WIDTH, DINO_HEIGHT, SH110X_WHITE);
		}
		frameswitch = !frameswitch;
	} else {
		myOLED.drawBitmap(DINO_INIT_X, *y, dino1, DINO_WIDTH, DINO_HEIGHT, SH110X_WHITE);
	}
}
// Move ptero function
void movePtero() {
  if (!pteroColission) {
	if (random(0,2) == 1) {
      myOLED.drawBitmap(ptero_x, ptero_y, ptero1, PTERO_WIDTH, PTERO_HEIGHT, SH110X_WHITE);
  	} else {
	  myOLED.drawBitmap(ptero_x, ptero_y, ptero2, PTERO_WIDTH, PTERO_HEIGHT, SH110X_WHITE);
    }
  } else {
	myOLED.drawBitmap(ptero_x, ptero_y, ptero3, PTERO_HEIGHT, PTERO_WIDTH, SH110X_WHITE);
  }
}

// Move tree funciton
void moveTree(int *x, int type){
  if(type==0){
    myOLED.drawBitmap(*x, TREE_Y, tree1, TREE1_WIDTH, TREE1_HEIGHT, SH110X_WHITE);
  }
  else if(type==1){
    myOLED.drawBitmap(*x, TREE_Y, tree2, TREE2_WIDTH, TREE2_HEIGHT, SH110X_WHITE);
  }
}

void ColisionDetected(int dino_y) {
	//Serial.println("KOLIZJA");
	displayAuu(dino_y-2);
	myOLED.display();  
	if(soundEnabled){
		tone(BUZZER_PIN,1000,100);
		delay(100);
		tone(BUZZER_PIN,500,100);
		delay(100);
		tone(BUZZER_PIN,800,200);
		delay(200);
		noTone(BUZZER_PIN);
	} else {
		delay(1200);
	}
	myOLED.clearDisplay();
}

void checkPteroColision() {
	if ((ptero_x < DINO_INIT_X+12) && !(ptero_x < DINO_INIT_X) && (ptero_y > dino_y-10) && !pteroColission) {
		pteroColission = true;
		food = food + 5;
		if(soundEnabled){
			tone(BUZZER_PIN,100,100);
			delay(150);
			tone(BUZZER_PIN,100,100);
			delay(150);
			noTone(BUZZER_PIN);
		}
	}
	if ((pteroColission) & ((ptero_y>64)||(ptero_x< -20))) {
		pteroColission = false;
		ptero_x = PTERO_INIT_X + random(0,40);
		ptero_y = PTERO_INIT_Y + random(0,10);
	}
}

void dino_jump(int *jump, int *dino_y){
	if(*jump==1){
		if (soundEnabled && !sounddone) {
			tone(BUZZER_PIN,800,100);
			delay(50);
			tone(BUZZER_PIN,1100,100);
			delay(50);
			noTone(BUZZER_PIN);
			sounddone =true;
		}

		(*dino_y)--; (*dino_y)--;//(*dino_y)--;
		ExtraSpeed(0, dino_y);

		if(*dino_y <= (DINO_INIT_Y-JUMP_PIXEL)){
			*dino_y = (DINO_INIT_Y-JUMP_PIXEL);
			food--;
			*jump=2;
		}
	} else if (*jump==2){
		//if (soundEnabled) tone(BUZZER_PIN,800,30);
		(*dino_y)++; (*dino_y)++;//(*dino_y)++;
   		ExtraSpeed(1, dino_y);
		if (*dino_y >= DINO_INIT_Y){
			*dino_y = DINO_INIT_Y;
			*jump=0; 
			score++;
			sounddone =false;
		}
	}
}

bool CheckCollision(){
  int dinoXcol = (DINO_INIT_X + DINO_WIDTH/2);

  if(( dinoXcol >= (tree_x+TREE1_WIDTH/2-4) && dinoXcol <= (tree_x+TREE1_WIDTH/2+4)) ||
     ( dinoXcol >= (tree1_x+TREE2_WIDTH/2-4) && dinoXcol <= (tree1_x+TREE2_WIDTH/2+4))) {
    if(dino_y>=(DINO_INIT_Y-6)){
      // Collision Happened
      return true;
    }    
  }
  return false;
}

void CalcTreesAnimations(){
	int treedist = abs(tree1_x-tree_x);

	tree_x--; tree_x--;//tree_x--;
	//Serial.print("tree_x ");
    ExtraSpeed(0, &tree_x);
	tree1_x--; tree1_x--;//tree1_x--;
	//Serial.print("tree1_x ");
	ExtraSpeed(0, &tree1_x);

	if(tree_x<-10) {
		tree_x = BASE_LINE_X1 + random(0,15);
		tree_type = random(0,2);
	}
	if (tree1_x<-10) {
		tree1_x = BASE_LINE_X2 + random(0,15);
		tree_type1 = random(0,2);
	}
	if (treedist > 10 && treedist < 35) {
		if (tree1_x>tree_x) {
				tree1_x = tree1_x+30;
		} else tree_x = tree_x+30;
	}
	treedist = abs(tree1_x-tree_x);
    //displayVar(treedist);
	int dir_y = random(0,2);
	if (!pteroColission){
		ptero_x = ptero_x - random(2,6);
		if (dir_y == 0) {
			ptero_y = ptero_y + random(0,4);
		} else {
			ptero_y = ptero_y - random(0,4);
		}	
		if (ptero_y > 22) ptero_y = 18;
		if (ptero_y < 1) ptero_y = 2;
	} else {
		ptero_y++; ptero_y++;
		ptero_x = ptero_x-3;

	}
	if ((ptero_x < 0 - PTERO_WIDTH) || (ptero_y<0)) {
		ptero_x = (PTERO_INIT_X + random(0,80));
		ptero_y = (PTERO_INIT_Y);
		pteroColission = false;
	}
}

void CalcGameSpeed(){
	GameSpeed = score/3;
    if (GameSpeed > maxspeed) GameSpeed = maxspeed;
}

void ReplayInitValues() {
  score = 0;
  jump = 0;
  int gamespeed = 0;
  pauseGame = false;
  food = FOOD_INIT;
  dino_y = DINO_INIT_Y;
  tree_x = BASE_LINE_X1_INIT;
  tree1_x = BASE_LINE_X2;
  ptero_x = PTERO_INIT_X;
  ptero_y = PTERO_INIT_Y;
  pteroColission = false;

  sounddone = false;
}

void InitValues() {
  GameSpeed = 0;
  pauseGame = false;
};

void introMessage(){
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

void ShowScore(int score){
  if ((score > DinoSesionScore)) {
    DinoSesionScore = score;
  }
  myOLED.setCursor(1,4);
  myOLED.print("Your score:   ");
  myOLED.print(score);
  myOLED.setCursor(1,20);
  myOLED.print("Sesion best:  ");
  myOLED.print(DinoSesionScore);
  myOLED.setCursor(1,36);
  myOLED.print("All time best: ");
  int highscore;
  EEPROM.get(DinoRecord, highscore);
  myOLED.print(highscore);
  myOLED.display();
}

// Game over display with score
void displaySummary(int score){
  myOLED.clearDisplay();
  myOLED.setTextSize(2);
  myOLED.setCursor(10,20);
  myOLED.print("Game over ");
  myOLED.display();
  delay(600);
  myOLED.setTextSize(1);
  myOLED.clearDisplay();

  int highscore;
  EEPROM.get(DinoRecord, highscore);
  score = score + food;
  if ((score > highscore)) {
    EEPROM.put(DinoRecord, score);
    EEPROM.commit();
  }

  ShowScore(score);

  myOLED.setCursor(1,54);
  myOLED.println("Push button to Play!");
  myOLED.display();
  delay(300);
}

void displayAuu(int y){
  myOLED.setCursor((DINO_INIT_X + DINO_WIDTH + 2+2), y-2);
  myOLED.println("Auuuc!!");
}

// Display score while running the game
void displayScore(int score){
  myOLED.setCursor(1,2);
  myOLED.print("food ");
  myOLED.print(food);
  myOLED.setCursor(61,2);
  myOLED.print("Jump: ");
  myOLED.print(score);
}

void renderScene(){
  myOLED.clearDisplay();
  myOLED.drawBitmap(DINO_INIT_X, DINO_INIT_Y, dino1, DINO_WIDTH, DINO_HEIGHT, SH110X_WHITE);
  myOLED.drawBitmap(BASE_LINE_X1_INIT, TREE_Y, tree1, TREE1_WIDTH, TREE1_HEIGHT, SH110X_WHITE);
	myOLED.drawBitmap(BASE_LINE_X2, TREE_Y, tree2, TREE2_WIDTH, TREE2_HEIGHT, SH110X_WHITE);

  myOLED.drawLine(0, BASE_LINE_Y, 127, BASE_LINE_Y, SH110X_WHITE);

	myOLED.setCursor(5, 8);
  myOLED.println("Dino jump, run!");
  myOLED.display();
  delay(100);
}

void displayGameFrame() {
	displayScore(score);
	displaySound(soundEnabled);

	moveTree(&tree_x, tree_type);
	moveTree(&tree1_x, tree_type1);
	moveDino(&dino_y);
  movePtero();
	myOLED.drawLine(0, BASE_LINE_Y, 127, BASE_LINE_Y, SH110X_WHITE);
}


void OutOfFood(){
  myOLED.setTextSize(1);
  myOLED.setCursor(15,55);
  myOLED.print("No Food - no jump!");
  myOLED.display();
}

const GameInfo GameInfo_Dino = {
  "Dino",
  "Unikaj przeszkód, skacz dinozaurem!",
  Game_Dino
};
