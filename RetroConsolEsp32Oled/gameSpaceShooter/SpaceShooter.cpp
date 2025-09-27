#include "SpaceShooter.h"
#include "SpaceShooter_private.h"

Ship playerShip;
Bullet bullets[MAX_BULLETS];
Enemy enemies[MAX_ENEMIES];
int spaceShooterScore = 0;
bool spaceShooterGameOver = false;

void resetSpaceShooter() {
  playerShip.x = 10;
  playerShip.y = SCREEN_HEIGHT / 2 - SHIP_HEIGHT / 2;
  for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
  for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
  spaceShooterScore = 0;
  spaceShooterGameOver = false;
}

void drawShip() {
  myOLED.fillRect(playerShip.x, playerShip.y, SHIP_WIDTH, SHIP_HEIGHT, SH110X_WHITE);
}

void drawBullets() {
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active)
      myOLED.fillRect(bullets[i].x, bullets[i].y, BULLET_WIDTH, BULLET_HEIGHT, SH110X_WHITE);
  }
}

void drawEnemies() {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].active)
      myOLED.fillRect(enemies[i].x, enemies[i].y, ENEMY_WIDTH, ENEMY_HEIGHT, SH110X_WHITE);
  }
}

void moveBullets() {
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active) {
      bullets[i].x += 4;
      if (bullets[i].x > SCREEN_WIDTH) bullets[i].active = false;
    }
  }
}

void moveEnemies() {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].active) {
      enemies[i].x -= 2;
      if (enemies[i].x + ENEMY_WIDTH < 0) enemies[i].active = false;
    }
  }
}

void spawnEnemy() {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (!enemies[i].active) {
      enemies[i].active = true;
      enemies[i].x = SCREEN_WIDTH - ENEMY_WIDTH;
      enemies[i].y = random(0, SCREEN_HEIGHT - ENEMY_HEIGHT);
      break;
    }
  }
}

void shootBullet() {
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (!bullets[i].active) {
      bullets[i].active = true;
      bullets[i].x = playerShip.x + SHIP_WIDTH;
      bullets[i].y = playerShip.y + SHIP_HEIGHT / 2 - BULLET_HEIGHT / 2;
      break;
    }
  }
}

void checkCollisions() {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].active) {
      // Kolizja z graczem
      if (enemies[i].x < playerShip.x + SHIP_WIDTH && enemies[i].x + ENEMY_WIDTH > playerShip.x &&
          enemies[i].y < playerShip.y + SHIP_HEIGHT && enemies[i].y + ENEMY_HEIGHT > playerShip.y) {
        spaceShooterGameOver = true;
      }
      // Kolizja z pociskiem
      for (int j = 0; j < MAX_BULLETS; j++) {
        if (bullets[j].active &&
            bullets[j].x < enemies[i].x + ENEMY_WIDTH && bullets[j].x + BULLET_WIDTH > enemies[i].x &&
            bullets[j].y < enemies[i].y + ENEMY_HEIGHT && bullets[j].y + BULLET_HEIGHT > enemies[i].y) {
          bullets[j].active = false;
          enemies[i].active = false;
          spaceShooterScore++;
        }
      }
    }
  }
}

void drawScore() {
  myOLED.setTextSize(1);
  myOLED.setTextColor(SH110X_WHITE);
  myOLED.setCursor(0, 0);
  myOLED.print("Score: ");
  myOLED.print(spaceShooterScore);
}

void Game_SpaceShooter() {
  resetSpaceShooter();
  unsigned long lastEnemySpawn = millis();
  while (!spaceShooterGameOver) {
    myOLED.clearDisplay();
    // Sterowanie
    if (IsPressed(UpLeft) && playerShip.y > 0) playerShip.y -= 2;
    if (IsPressed(DownLeft) && playerShip.y < SCREEN_HEIGHT - SHIP_HEIGHT) playerShip.y += 2;
    if (IsPressed(UpRight)) shootBullet();
    // Aktualizacja
    moveBullets();
    moveEnemies();
    checkCollisions();
    if (millis() - lastEnemySpawn > 700) {
      spawnEnemy();
      lastEnemySpawn = millis();
    }
    // Rysowanie
    drawShip();
    drawBullets();
    drawEnemies();
    drawScore();
    myOLED.display();
    delay(30);
  }
  // Ekran końca gry
  myOLED.clearDisplay();
  myOLED.setTextSize(2);
  myOLED.setTextColor(SH110X_WHITE);
  myOLED.setCursor(10, 20);
  myOLED.print("KONIEC");
  myOLED.setTextSize(1);
  myOLED.setCursor(10, 45);
  myOLED.print("Wynik: ");
  myOLED.print(spaceShooterScore);
  myOLED.display();
  delay(2000);
}
const GameInfo GameInfo_SpaceShooter = {
  "Space Shooter",
  "Strzelaj do wrogów, omijaj przeszkody!",
  Game_SpaceShooter,
  Game_SpaceShooterRecord
};
