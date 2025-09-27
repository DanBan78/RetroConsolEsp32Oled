#ifndef GLOBAL_H
#define GLOBAL_H

  #include <Arduino.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SH110X.h>
  #include <SPI.h>
  #include <EEPROM.h>
  #include <Wire.h>
  #include <pgmspace.h>
  #include <Fonts/TomThumb.h>

  #define HALT while(1)
	#define OLED_RESET     -1    // Reset pin # (or -1 if sharing Arduino reset pin)
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
  #define Game_DinoRecord 0
  #define Game_WolfRecord 4
  #define Game_SlalomRecord 8
  #define Game_CymbergajRecord 12
  #define Game_SnoopyRecord 16
  #define Game_SpaceShooterRecord 20

//  bool ActiveButton = false;
  enum btPressedCode { UpLeft, DownRight, NONE, UpRight, DownLeft, ALL_BTN };

  typedef void (*GameLaunchFunc)();

  typedef struct GameInfo {
    const char* name;
    const char* description;
    GameLaunchFunc launch;
    int eepromAddress;  // Adres w EEPROM do zapisu highscore
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


const char str0[] PROGMEM = "Przyciski ...";
const char str1[] PROGMEM = "LeftUp/Down ";
const char str2[] PROGMEM = "zmiana wiersza menu";
const char str3[] PROGMEM = "";
const char str4[] PROGMEM = "Przycisk ..";
const char str5[] PROGMEM = "RightUp";
const char str6[] PROGMEM = "zmiana opcji menu";
const char str7[] PROGMEM = "menu gl.wl/wyl dzwiek";
const char str8[] PROGMEM = "";
const char str9[] PROGMEM = "Przycisk .";
const char str10[] PROGMEM = "RightDown";
const char str11[] PROGMEM = "enter / start gry";
const char str12[] PROGMEM = "WSZYSTKIE NA RAZ ";
const char str13[] PROGMEM = "powrot do wyboru gry";


const char* const teksty[] PROGMEM = { str0, str1, str2, str3, str4, str5, str6, str7, str8, str9, str10, str11, str12, str13 };
  btPressedCode ReadButton(void (*callback)(timerStruct&),timerStruct& t);
  bool IsPressed(btPressedCode button);
  bool CheckIfTimePassed(unsigned long& LastTimeCheckOneSec, unsigned long interval);

  void displayVar(int i);
  void DisplayHelpInfo();
  void displaySoundInfo(uint8_t x, uint8_t y, bool sound);
  void displaySoundInfo2(bool sound);
  void displayMenu(int MenuStartRow, int totalGamesNo);
  void WaitforButton();
  void MyTune(int freq, int duration_ms);
  void WelcomeScreen();
  void CheckIfResetHighscores();
  void DisplayHighscores();
  void SerialPrintFreeRam();

  void (*FuncPointer)(timerStruct&);
  
	// Dźwięki

  #define DELAY1000MS 1000
  #define DELAY100MS 100
  #define DELAY10MS 10
  #define DELAY1500MS 1500
  #define DELAY2000MS 2000
  #define DELAY250MS 250
  #define DELAY500MS 500
  #define DELAY50MS 50
  #define TON_ERROR_CZAS 100
	#define TON_POINT_CZAS 50
	#define TON_POINT_FREQ 700
  #define TON_ERROR_FREQ 1320
  #define TON_KONIECGRY_CZAS  2000
  #define TON_KONIECGRY_FREQ  220
  #define TON_MENU_CZAS  50
  #define TON_MENU_FREQ  1320
  #define TON_ODLICZANIE_CZAS  100
  #define TON_ODLICZANIE_FREQ  880
  #define TON_PALETKA_CZAS  200
  #define TON_PALETKA_FREQ  440
  #define TON_PUNKT_CZAS  1000
  #define TON_PUNKT_FREQ  110
  #define TON_RAMKA_CZAS  50
  #define TON_RAMKA_FREQ  1320
  #define TON_EGG_FREQ 400
	#define TON_EGG_CZAS 10
  #define TON_START_CZAS  TON_RAMKA_CZAS
  #define TON_START_FREQ  TON_RAMKA_FREQ
#endif