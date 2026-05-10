#include "Managers/DisplayManager.h"
#include "PinConfig.h"
#include "Assets.h"

DisplayManager& DisplayManager::getInstance() {static DisplayManager instance; return instance;}

DisplayManager::DisplayManager() : screen(Adafruit_PCD8544(DISPLAY_CLK, DISPLAY_DIN, DISPLAY_DC, DISPLAY_CE, DISPLAY_RST)){
  
}

void DisplayManager::Splash(){
  screen.begin();
  screen.setContrast(90);
  screen.clearDisplay();
  screen.setTextSize(1);
  screen.setTextColor(BLACK);
  screen.setCursor(0, 0);
  screen.drawBitmap(0, 0, splash_screen, 84, 48, BLACK);
  screen.display();
  delay(2000);
}

DisplayManager::~DisplayManager(){}
