// Include 
#include "global/global.h"
#include "global/global.cpp"


// Tablica wskaźników do struktur z informacjami o grach
const GameInfo* allGames[] = {
    &GameInfo_Cymbergaj,
    &GameInfo_Slalom,
    &GameInfo_Dino,
    &GameInfo_Wilk,
    //&GameInfo_SpaceShooter,
    &GameInfo_HackMe,
    &HelpInfo,

    // Dodaj kolejne gry: &GameInfo_SpaceShooter, &GameInfo_Slalom, ...
  };
// Liczba dostępnych gier
const int totalGamesNo = sizeof(allGames)/sizeof(allGames[0]);

//####################################################################################
// put your setup code here, to run once:
void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(A0));

  pinMode(BTN_0, INPUT_PULLUP);
  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_3, INPUT_PULLUP);
  pinMode(BTN_4, INPUT_PULLUP);

  Wire.begin(8, 9); // SDA, SCL dla ESP32-C3
  EEPROM.begin(64); // Inicjalizacja EEPROM
  ledcAttach(BUZZER_PIN, BUZZER_CHANNEL, LEDC_LOW_SPEED_MODE); // Buzzer
 
  Wire.setClock(400000); // 400 kHz
  delay(250); // wait for the OLED to power up
  myOLED.begin(SCREEN_ADDRESS, true); // Address 0x3C default
  myOLED.clearDisplay();

  WelcomeScreen();
  delay(500);
  CheckIfResetHighscores();
  delay(100);
  DisplayHighscores();
}
//####################################################################################
void loop() {
 delay(100);
  Serial.println("loop");
  score = 0;
  int selected = GameSelectMenu();
  if (selected >= 0 && selected < (sizeof(allGames)/sizeof(allGames[0]))) {
    allGames[selected]->launch();
  }
}