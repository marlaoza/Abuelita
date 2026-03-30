#include "Activities/Menu.h"
#include "Assets.h"
#include <Adafruit_PCD8544.h>

Menu::Menu() 
    : Activity(ico_home),activityManager(ActivityManager::getInstance()), 
      keyPadManager(KeyPadManager::getInstance()), displayManager(DisplayManager::getInstance()) {}

void Menu::setup() {
   displayManager.screen.clearDisplay();
   selectedApp = 0;
   appsPerRow = 3;
   drawMenu();
}

void Menu::drawMenu () {
    displayManager.screen.clearDisplay();
    int xOffset = 10;
    int yOffset = 3;
    int Xpos = 0;
    int Ypos = 0;
    int index = 0;

    if((floor((selectedApp-1)/appsPerRow)*24) + yOffset < 48){
        Serial.println("fora");
    }

    for (int i = 1; i < activityManager.getActivitiesCount(); i++){
        Xpos = (((i-1) % appsPerRow)*23) + xOffset;
        Ypos = (floor((i-1)/appsPerRow)*24) + yOffset;
        displayManager.screen.drawRect(Xpos, Ypos, 20, 20, BLACK);
        displayManager.screen.drawBitmap(Xpos+1, Ypos+1, activityManager.getActivities()[i]->icon, 18, 18, BLACK);

        if(selectedApp == i){
            displayManager.screen.drawRect(Xpos-1, Ypos-1, 22, 22, BLACK);
        }
    }

    displayManager.screen.display();
}

void Menu::loop() {
    char key = keyPadManager.readKey();
    if(key){
        switch(key){
            case '2':if(selectedApp >= appsPerRow) selectedApp -= appsPerRow;
                break;
            case '8':if(selectedApp + appsPerRow <= activityManager.getActivitiesCount() - 1) selectedApp += appsPerRow;
                break;
            case '4':if(selectedApp > 1) selectedApp -= 1;
                break;
            case '6':if(selectedApp < activityManager.getActivitiesCount() - 1) selectedApp += 1;
                break;
            case '5':activityManager.setActivity(selectedApp);
                return;
        }
        drawMenu();
    }
}

void Menu::stop() {
    selectedApp = 0;
}