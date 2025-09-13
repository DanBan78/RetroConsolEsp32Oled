#include "Wilk.h"
#include "Wilk_private.h"

void Game_Wilk() {

  NuPogodiSetup();
  StartScreen();
  myOLED.clearDisplay();

  StartWolf();
  ShowScore();
  Location();
  GenerationEggs();
  displaySound(soundEnabled);

  Serial.print("Wolf Init");
  SerialPrintFreeRam();
  while(!gameEnd) {

    WolfMove();
    LevelComplicate();
    if (eggJustBroken) StartAfterBrokenEgg();

    if (millis()-Tm>(GameDelay)) {
      Tm  = millis();
      eventEggs = 1;
      if(soundEnabled) {MyTune(TON_EGG_FREQ, TON_EGG_CZAS);}
      MassEggs[1] <<= 1;
      MassEggs[2] <<= 1;
      MassEggs[3] <<= 1;
      MassEggs[4] <<= 1;
      if (eventGenerationEggs) {
        GenerationEggs();
        eventGenerationEggs = 0;
      }
    }
    MoveEggs();
    displaySound(soundEnabled);
    //myOLED.fillRect(0, 60, 128, 64,  SH110X_BLACK);

    if (eventEggs) {myOLED.display(); eventEggs = 0;}
    if (eventWolf) {myOLED.display(); eventWolf = 0;}   
  }
  return;
}


void NuPogodiSetup() {
  eggsLeftUp[1]    = 16;  eggsLeftUp[1]    <<= 8;  eggsLeftUp[1]    += 2;
  eggsLeftUp[2]    = 18;  eggsLeftUp[2]    <<= 8;  eggsLeftUp[2]    += 7;
  eggsLeftUp[3]    = 20;  eggsLeftUp[3]    <<= 8;  eggsLeftUp[3]    += 12;
  eggsLeftUp[4]    = 22;  eggsLeftUp[4]    <<= 8;  eggsLeftUp[4]    += 17;
  eggsLeftUp[5]    = 24;  eggsLeftUp[5]    <<= 8;  eggsLeftUp[5]    += 22;
  eggsLeftUp[6]    = 0;
  eggsLeftDown[1]  = 36;  eggsLeftDown[1]  <<= 8;  eggsLeftDown[1]  += 2;
  eggsLeftDown[2]  = 38;  eggsLeftDown[2]  <<= 8;  eggsLeftDown[2]  += 7;
  eggsLeftDown[3]  = 40;  eggsLeftDown[3]  <<= 8;  eggsLeftDown[3]  += 12;
  eggsLeftDown[4]  = 42;  eggsLeftDown[4]  <<= 8;  eggsLeftDown[4]  += 17;
  eggsLeftDown[5]  = 44;  eggsLeftDown[5]  <<= 8;  eggsLeftDown[5]  += 22;
  eggsLeftDown[6]  = 0;
  eggsRightUp[1]   = 16;  eggsRightUp[1]   <<= 8;  eggsRightUp[1]   += 125;
  eggsRightUp[2]   = 18;  eggsRightUp[2]   <<= 8;  eggsRightUp[2]   += 120;
  eggsRightUp[3]   = 20;  eggsRightUp[3]   <<= 8;  eggsRightUp[3]   += 115;
  eggsRightUp[4]   = 22;  eggsRightUp[4]   <<= 8;  eggsRightUp[4]   += 110;
  eggsRightUp[5]   = 24;  eggsRightUp[5]   <<= 8;  eggsRightUp[5]   += 105;
  eggsRightUp[6]   = 0;
  eggsRightDown[1] = 36;  eggsRightDown[1] <<= 8;  eggsRightDown[1] += 125;
  eggsRightDown[2] = 38;  eggsRightDown[2] <<= 8;  eggsRightDown[2] += 120;
  eggsRightDown[3] = 40;  eggsRightDown[3] <<= 8;  eggsRightDown[3] += 115;
  eggsRightDown[4] = 42;  eggsRightDown[4] <<= 8;  eggsRightDown[4] += 110;
  eggsRightDown[5] = 44;  eggsRightDown[5] <<= 8;  eggsRightDown[5] += 105;
  eggsRightDown[6] = 0;

  GameDelay = 500;

  gameEnd = false;
  eggJustBroken = false;
}

void MoveEggs(){
  EggMoveLeftUp();
  EggMoveLeftDown();
  EggMoveRightUp();
  EggMoveRightDown();
}

void GenerationEggs() {
  do {s=random(1, 5);}                                                                // Сравниваем предыдущее значение и новое сгенерированное
  while(s==eggsPosition);                                                             // Выполняем цикл пока оба значение равны друг другу
  eggsPosition = s;                                                                   // Присваиваем новое значение
  switch (eggsPosition)                                                               // Выбираем позицию яйца
  {
    case 1:                                                                           // Позиция яйца слева сверху
    MassEggs[1] |= 1;                                                                 // Задаем новое яйцо слева сверху
    break;                                                                            // Выходим из оператора case
    case 2:                                                                           // Позиция яйца слева снизу
    MassEggs[2] |= 1;                                                                 // Задаем новое яйцо слева снизу
    break;                                                                            // Выходим из оператора case
    case 3:                                                                           // Позиция яйца справа сверху
    MassEggs[3] |= 1;                                                                 // Задаем новое яйцо справа сверху
    break;                                                                            // Выходим из оператора case
    case 4:                                                                           // Позиция яйца справа снизу
    MassEggs[4] |= 1;                                                                 // Задаем новое яйцо справа снизу
    break;                                                                            // Выходим из оператора case
  }
}

void EggMoveLeftUp() {
  if (bitRead(MassEggs[1],quantityScore)){eventGenerationEggs = 1;}
  if (bitRead(MassEggs[1],6)){CatcheLeftUp();}
  for (int i = 1; i <= 5; i++)
  {
    if (bitRead(MassEggs[1],i))
    {
      myOLED.drawCircle(lowByte(eggsLeftUp[i-1]),highByte(eggsLeftUp[i-1]),3,0);
      myOLED.drawCircle(lowByte(eggsLeftUp[i]),  highByte(eggsLeftUp[i]),  3,1);
    }
  }
}

void EggMoveLeftDown(){
  if (bitRead(MassEggs[2],quantityScore)){eventGenerationEggs = 1;}                   // Разрешаем генерацию нового яйца в зависимости от уровня сложности
  if (bitRead(MassEggs[2],6)){CatcheLeftDown();}                                      // Проверяем поймал волк яйцо
  for (int i = 1; i <= 5; i++)                                                        // Отрисовываем новый массив с помощью цикла
  {
    if (bitRead(MassEggs[2],i))                                                       // Если бит равен единице, то выводим положение яйца на экране
    {
      myOLED.drawCircle(lowByte(eggsLeftDown[i-1]),highByte(eggsLeftDown[i-1]),3,0);
      myOLED.drawCircle(lowByte(eggsLeftDown[i]),  highByte(eggsLeftDown[i]),  3,1);
    }
  }
}

void EggMoveRightUp() {
  if (bitRead(MassEggs[3],quantityScore)){eventGenerationEggs = 1;}                   // Разрешаем генерацию нового яйца в зависимости от уровня сложности
  if (bitRead(MassEggs[3],6)){CatcheRightUp();}                                       // Проверяем поймал волк яйцо
  for (int i = 1; i <= 5; i++)                                                        // Отрисовываем новый массив с помощью цикла
  {
    if (bitRead(MassEggs[3],i))                                                       // Если бит равен единице, то выводим положение яйца на экране
    {
      myOLED.drawCircle(lowByte(eggsRightUp[i-1]),highByte(eggsRightUp[i-1]),3,0);
      myOLED.drawCircle(lowByte(eggsRightUp[i]),  highByte(eggsRightUp[i]),  3,1);
    }
  }
}

void EggMoveRightDown() {
  if (bitRead(MassEggs[4],quantityScore)){eventGenerationEggs = 1;}                   // Разрешаем генерацию нового яйца в зависимости от уровня сложности
  if (bitRead(MassEggs[4],6)){CatcheRightDown();}                                     // Проверяем поймал волк яйцо
  for (int i = 1; i <= 5; i++)                                                        // Отрисовываем новый массив с помощью цикла
  {
    if (bitRead(MassEggs[4],i))                                                       // Если бит равен единице, то выводим положение яйца на экране
    {
      myOLED.drawCircle(lowByte(eggsRightDown[i-1]),highByte(eggsRightDown[i-1]),3,0);
      myOLED.drawCircle(lowByte(eggsRightDown[i]),  highByte(eggsRightDown[i]),  3,1);
    }
  }
}

void LevelComplicate() {
  if (!score)     {quantityScore = levelComplicate1; GameDelay = 500;}
  if (score > 5) {quantityScore = levelComplicate2; GameDelay = 450;}
  if (score > 12){quantityScore = levelComplicate3; GameDelay = 420;}
  if (score > 18){quantityScore = levelComplicate4; GameDelay = 400;}
  if (score > 24){GameDelay = 380;}
  if (score > 30){GameDelay = 350;}
  if (score > 36){GameDelay = 320;}
  if (score > 41){GameDelay = 290;}
  if (score > 47){GameDelay = 260;}
  if (score > 53){GameDelay = 240;}
  if (score > 59){GameDelay = 220;}
  if (score > 65){GameDelay = 200;}
  if (score > 75){GameDelay = 190;}
  if (score > 85){GameDelay = 185;}
  if (score > 95){GameDelay = 180;}
}

void EggIsCaught() {
  score++;
  ShowScore();
  if(soundEnabled) MyTune(TON_POINT_FREQ, TON_POINT_CZAS);
}

void EggIsBroken() {
  brokegg++;
  eggJustBroken = true;
  ShowScore();
  if(soundEnabled) MyTune(TON_ERROR_FREQ, TON_ERROR_CZAS);
  if (brokegg > 2) {
    gameEnd = GameOver();
    delay(300);
  }
  else {
    StartWolf();
  }
}

void CatcheLeftUp() {   
  if (bitRead(MassWolfPosition,2) && bitRead(MassEggs[1],6)) {
    bitClear(MassEggs[1],6);
    myOLED.drawCircle (22, 24, 3, 0);
    EggIsCaught();
  }
  else {
    myOLED.fillRect(25, 14, 76, 52,  SH110X_BLACK);
    myOLED.drawBitmap(50, 16, Wolf_nu_pogodi, 31, 46, SH110X_WHITE);
    myOLED.drawCircle (22, 24, 3, 0);
    myOLED.drawCircle (32, 31, 3, 1);
    myOLED.display();
    delay(250);
    myOLED.drawCircle (32, 31, 3, 0);
    myOLED.drawCircle (32, 41, 3, 1);
    myOLED.display();
    delay(250);
    myOLED.drawCircle (32, 41, 3, 0);
    myOLED.drawCircle (32, 51, 3, 1);
    myOLED.display();
    delay(250);
    myOLED.drawCircle (32, 51, 3, 0);
    myOLED.drawBitmap(24, 47, EggBroken, 23, 16, SH110X_WHITE);
    myOLED.display();
    delay(30);
    bitClear(MassEggs[1],6);
    EggIsBroken();
  }
}

void CatcheLeftDown() {
  if (bitRead(MassWolfPosition,1) && bitRead(MassEggs[2],6)) {
    bitClear(MassEggs[2],6);
    myOLED.drawCircle (22, 44, 3, 0); 
    EggIsCaught();
  }
  else {
    myOLED.fillRect(25, 14, 76, 52,  SH110X_BLACK);
    myOLED.drawBitmap(50, 16, Wolf_nu_pogodi, 31, 46, SH110X_WHITE);
    myOLED.drawCircle (22, 44, 3, 0); 
    myOLED.drawCircle (32, 51, 3, 1); 
    myOLED.display();
    delay(250);
    myOLED.drawCircle (32, 51, 3, 0); 
    myOLED.drawBitmap(24, 47, EggBroken, 23, 16, SH110X_WHITE); 
    myOLED.display();
    delay(450);
    bitClear(MassEggs[2],6);
    EggIsBroken();
  }
}

void CatcheRightUp() {
  if (bitRead(MassWolfPosition,3) && bitRead(MassEggs[3],6)) {
    bitClear(MassEggs[3],6);
    myOLED.drawCircle (105, 24, 3, 0);
    EggIsCaught();
  }
  else {
    myOLED.fillRect(25, 14, 76, 52,  SH110X_BLACK);
    myOLED.drawBitmap(50, 16, Wolf_nu_pogodi, 31, 46, SH110X_WHITE);

    myOLED.drawCircle (105, 24, 3, 0);
    myOLED.drawCircle (95,  31, 3, 1);
    myOLED.display();
    delay(250);
    myOLED.drawCircle (95,  31, 3, 0);
    myOLED.drawCircle (95,  41, 3, 1);
    myOLED.display();
    delay(250);
    myOLED.drawCircle (95,  41, 3, 0);
    myOLED.drawCircle (95,  51, 3, 1);
    myOLED.display();
    delay(250);
    myOLED.drawCircle (95,  51, 3, 0);
    myOLED.drawBitmap(80, 47, EggBroken, 23, 16, SH110X_WHITE);
    myOLED.display();
    delay(30);
    bitClear(MassEggs[3],6); 
    EggIsBroken();
  }
}

void CatcheRightDown() {
  if (bitRead(MassWolfPosition,4) && bitRead(MassEggs[4],6)) {
    bitClear(MassEggs[4],6);
    myOLED.drawCircle (105, 44, 3, 0);
    EggIsCaught();
  }
  else {
    myOLED.fillRect(25, 14, 76, 52,  SH110X_BLACK);
    myOLED.drawBitmap(50, 16, Wolf_nu_pogodi, 31, 46, SH110X_WHITE);
    myOLED.drawCircle (105, 44, 3, 0);
    myOLED.drawCircle (95,  51, 3, 1);
    myOLED.display();
    delay(250);
    myOLED.drawCircle (95, 51, 3, 0); 
    myOLED.drawBitmap(80, 47, EggBroken, 23, 16, SH110X_WHITE);
    myOLED.display();
    delay(450);
    bitClear(MassEggs[4],6);
    EggIsBroken();
  }
}

void WolfMove() {
  previousWolf = MassWolfPosition;
  btPressedCode btRead = ReadButton(nullptr, Timer1Sec);

  if (btRead != NONE) {
    MassWolfPosition = 0; 
 
    switch(btRead) {
      case DownLeft:  
        bitSet(MassWolfPosition,1);
        break;
      case UpLeft:
        bitSet(MassWolfPosition,2);
        break;
      case UpRight:
        bitSet(MassWolfPosition,3);
        break;
      case DownRight:
        bitSet(MassWolfPosition,4);
        break;
      case NONE:
      default:
      break;
    }
    if (previousWolf != MassWolfPosition) {
      if(soundEnabled) {MyTune(TON_CLICK_FREQ, TON_CLICK_CZAS);}
      myOLED.fillRect(25, 14, 76, 52,  SH110X_BLACK);
      switch(btRead) {
        case DownLeft:  
          myOLED.drawBitmap(46, 16, WolfLeft, 31, 48, SH110X_WHITE);
          myOLED.drawBitmap(27, 42, HandLeftUp, 27, 24, SH110X_WHITE); 
          break;
        case UpLeft:
          myOLED.drawBitmap(46, 16, WolfLeft, 31, 48, SH110X_WHITE);
          myOLED.drawBitmap(27, 24, HandLeftDown, 26, 24, SH110X_WHITE); 
          break;
        case UpRight:
          myOLED.drawBitmap(55, 16, WolfRight, 27, 48, SH110X_WHITE);
          myOLED.drawBitmap(73, 24, HandRightUp, 24, 24, SH110X_WHITE);
          break;
        case DownRight:
          myOLED.drawBitmap(55, 16, WolfRight, 27, 48, SH110X_WHITE);
          myOLED.drawBitmap(73, 44, HandRightDown, 29, 24, SH110X_WHITE);
          break;
        case NONE:
        default:
        return;
      }
      myOLED.display();
      eventWolf = 1;
    }
  }
}

void StartWolf() {
  ShowScore();
  Location();
  //myOLED.fillRect(0, 50, 110, 14,  SH110X_BLACK);
  myOLED.fillRect(25, 14, 76, 52,  SH110X_BLACK); 

  myOLED.drawBitmap(46, 16, WolfLeft, 31, 48, SH110X_WHITE);
  myOLED.drawBitmap(42,11, HandScare, 45, 24, SH110X_WHITE); 
  MassWolfPosition = 0;
  myOLED.display();
}

void Location() {
  myOLED.drawLine   (0, 20, 24, 29, 1);
  myOLED.drawLine   (0, 21, 24, 30, 1);
  myOLED.drawLine   (0, 22, 24, 31, 1);
  myOLED.drawLine   (0, 40, 24, 49, 1);
  myOLED.drawLine   (0, 41, 24, 50, 1);
  myOLED.drawLine   (0, 42, 24, 51, 1);
  myOLED.drawLine   (103, 29, 127, 20, 1);
  myOLED.drawLine   (103, 30, 127, 21, 1);
  myOLED.drawLine   (103, 31, 127, 22, 1);
  myOLED.drawLine   (103, 49, 127, 40, 1);
  myOLED.drawLine   (103, 50, 127, 41, 1);
  myOLED.drawLine   (103, 51, 127, 42, 1);
}

void ShowScore() {
  myOLED.fillRect(0, 0, 110, 14,  SH110X_BLACK);
  myOLED.setTextSize(1); 
  myOLED.drawCircle(9, 3, 3, 1);

  myOLED.setCursor(15,0);
  myOLED.print("-");
  myOLED.setCursor(21,0);
  myOLED.print(score);
  myOLED.drawBitmap(53, -5, EggBroken, 23, 16, SH110X_WHITE);
  myOLED.setCursor(77,0);
  myOLED.print("-");
  myOLED.setCursor(85,0);
  myOLED.print(brokegg);
  myOLED.setCursor(92,0);
  myOLED.print("/3");
  myOLED.display();
}

bool GameOver() {
  MassEggs[1] = 0;
  MassEggs[2] = 0;
  MassEggs[3] = 0;
  MassEggs[4] = 0;
  eventGenerationEggs = 0;
  MassWolfPosition = 0;
  eventWolf = 0;
  previousWolf = 0;
  wolfPositionStart = 0;
  eggsPosition = 0;
  timeEggs = 0;
  eventEggs = 0;
  s = 0;
  quantityScore = 0;
  if (score > WolfSesionScore) {
    WolfSesionScore = score;
  }
  EEPROM.get(WolfRecord,record);
  if (score > record) {
    EEPROM.put(WolfRecord, score);
    EEPROM.commit();
  }

  myOLED.clearDisplay();
  myOLED.setTextSize(1); 
  myOLED.setCursor(1,4);
  myOLED.print("Your score:   ");
  myOLED.print(score);
  myOLED.setCursor(1,20);
  myOLED.print("Sesion best:  ");
  myOLED.print(WolfSesionScore);
  myOLED.setCursor(1,36);
  myOLED.print("All time best: ");
  EEPROM.get(WolfRecord,record);
  myOLED.print(record);
  myOLED.display();

  if(soundEnabled) MyTune(TON_GAMEOVER_FREQ, TON_GAMEOVER_CZAS);
  delay(1000);
  myOLED.clearDisplay();
  if (score >= 60) {
    Film();
    myOLED.clearDisplay();
  }
  WaitforButton();
  
  score = 0;
  brokegg = 0;
  return true;
}


//######################################## funkcje
void StartScreen() {
  myOLED.clearDisplay();
  myOLED.drawRect (0, 0, 127, 63, 1);
  myOLED.setTextSize(2);
  myOLED.setTextColor(SH110X_WHITE);
  myOLED.setCursor(5,5);
  myOLED.print("Zajac,");
  myOLED.setTextSize(1);
  myOLED.setCursor(12,30);
  myOLED.print("jak ja Cie dorwe..");
  myOLED.display();
  delay(1000);
}
void Film() {
   myOLED.clearDisplay();
  while (ReadButton(nullptr, Timer1Sec)==NONE) {
    myOLED.clearDisplay();
    myOLED.drawBitmap(46, 16, WolfLeft, 31, 48, SH110X_WHITE);
    myOLED.drawBitmap(27, 24, HandLeftDown, 26, 24, SH110X_WHITE); 
    myOLED.display();
    delay(50);
    
    myOLED.clearDisplay();
    myOLED.drawBitmap(46, 16, WolfLeft, 31, 48, SH110X_WHITE);
    myOLED.drawBitmap(27, 42, HandLeftUp, 27, 24, SH110X_WHITE); 
    myOLED.display();
    delay(50);
   
    myOLED.clearDisplay();
    myOLED.drawBitmap(50, 16, WolfRight, 27, 48, SH110X_WHITE);
    myOLED.drawBitmap(72, 44, HandRightDown, 29, 24, SH110X_WHITE); 
    myOLED.display();
    delay(50);
    
    myOLED.clearDisplay();
    myOLED.drawBitmap(50, 16, WolfRight, 27, 48, SH110X_WHITE);
    myOLED.drawBitmap(72, 24, HandRightUp, 24, 24, SH110X_WHITE); 
    myOLED.display();
    delay(50);
    
  }
  myOLED.clearDisplay();
}

void StartAfterBrokenEgg() {
  delay(100);
  myOLED.fillRect(25, 14, 76, 52,  SH110X_BLACK);
  
  myOLED.clearDisplay();
  StartWolf();
  ShowScore();
  Location();
  myOLED.display();
 
  eggJustBroken = false;
  GameDelay = 400;
  MassEggs[1] = 1;
  MassEggs[2] = 0;
  MassEggs[3] = 0;
  MassEggs[4] = 0;
}
const GameInfo GameInfo_Wilk = {
  "Wilk",
  "Złap jajka, nie pozwól im spaść!",
  Game_Wilk
};
