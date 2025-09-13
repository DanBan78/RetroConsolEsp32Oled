#ifndef GLOBAL_H
#define GLOBAL_H

  #include <Arduino.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SH110X.h>
  #include <SPI.h>
  #include <EEPROM.h>
  #include <Wire.h>

  #define HALT while(1)
	#define OLED_RESET     4    // Reset pin # (or -1 if sharing Arduino reset pin)
	#define SCREEN_ADDRESS 0x3C
	#define SCREEN_WIDTH   128  // OLED display width, in pixels
	#define SCREEN_HEIGHT  64   // OLED display height, in pixels
  #define maxMenuRowsOnScreen 5

  extern Adafruit_SH1106G myOLED;

  extern bool soundEnabled;
  extern bool sounddone;
  extern bool pauseGame;
  extern bool Debug;
  extern int GameSpeed;
  extern int GameSelected;
  extern int score;
  extern int SessionScore;
  extern const int totalGamesNo;

  // Buttons
  #define BTN_1  1
  #define BTN_0  0
  #define BTN_3  3
  #define BTN_4  4
 
  //EEPROM adresses for games records
  #define DinoRecord 0
  #define WolfRecord 4
  #define SlalomRecord 8
  #define PongRecord 12

//  bool ActiveButton = false;
  enum btPressedCode { UpLeft, DownRight, NONE, UpRight, DownLeft };


  typedef void (*GameLaunchFunc)();

  typedef struct GameInfo {
    const char* name;
    const char* description;
    GameLaunchFunc launch;
  };
  
  typedef struct timerStruct {
    bool timeOut = false;
    int interval;
    int currTime;
    unsigned long prevTimeCheck;
  };
  extern timerStruct Timer1Sec;

  #define BUZZER_PIN 2
  #define BUZZER_CHANNEL 0

  // 'Glosnik', 8x8px
  #define Glosnik_x_y 8
  const unsigned char PROGMEM Glosnik [] = {
    0x19, 0x2a, 0xec, 0xab, 0xec, 0x2a, 0x19, 0x08
  };
  

//####################################################
  int GameSelectMenu();
  btPressedCode ReadButton(void (*callback)(timerStruct&),timerStruct& t);
  bool IsPressed(btPressedCode button);
  bool CheckIfTimePassed(unsigned long& LastTimeCheckOneSec, unsigned long interval);

  void displayVar(int i);
  void displaySound(bool sound);
  void displaySound2(bool sound);
  void displayMenu(int MenuStartRow, int totalGamesNo);
  void WaitforButton();
  void MyTune(int freq, int duration_ms);
  void WelcomeScreen();
  void CheckIfResetHighscores();
  void DisplayHighscores();
  void SerialPrintFreeRam();

  void (*FuncPointer)(timerStruct&);
  
	// Dźwięki
  #define TON_ODLICZANIE_FREQ  880
  #define TON_ODLICZANIE_CZAS  100
  #define TON_PALETKA_FREQ  440
  #define TON_PALETKA_CZAS  200
  #define TON_PUNKT_FREQ  110
  #define TON_PUNKT_CZAS  1000
  #define TON_RAMKA_FREQ  1320
  #define TON_RAMKA_CZAS  50
  #define TON_KONIECGRY_FREQ  220
  #define TON_KONIECGRY_CZAS  2000
  #define TON_START_FREQ  TON_RAMKA_FREQ
  #define TON_START_CZAS  TON_RAMKA_CZAS
  #define TON_MENU_FREQ  1320
  #define TON_MENU_CZAS  50

#define DELAY10MS 10
#define DELAY250MS 250
#define DELAY500MS 500
#define DELAY100MS 100
#define DELAY50MS 50
#define DELAY1000MS 1000
#endif