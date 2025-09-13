#ifndef SPACESHOOTER_PRIVATE_H
#define SPACESHOOTER_PRIVATE_H

    // Parametry gry
    #define SHIP_WIDTH 10
    #define SHIP_HEIGHT 8
    #define ENEMY_WIDTH 8
    #define ENEMY_HEIGHT 8
    #define BULLET_WIDTH 2
    #define BULLET_HEIGHT 4
    #define MAX_BULLETS 3
    #define MAX_ENEMIES 5

    struct Ship {
    int x, y;
    };

    struct Bullet {
    int x, y;
    bool active;
    };

    struct Enemy {
    int x, y;
    bool active;
    };

    extern Ship playerShip;
    extern Bullet bullets[MAX_BULLETS];
    extern Enemy enemies[MAX_ENEMIES];
    extern int spaceShooterScore;
    extern bool spaceShooterGameOver;

#endif
