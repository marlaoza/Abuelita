#ifndef ACTIVITY_MENU_H
#define ACTIVITY_MENU_H
#include "Activity.h"
#include <Managers/ActivityManager.h>
#include <Managers/KeypadManager.h>
#include <Managers/DisplayManager.h>

class Menu : public Activity {
private:
    int selectedApp;
    int appsPerRow;
    ActivityManager& activityManager;
    KeyPadManager& keyPadManager;
    DisplayManager& displayManager;
   
public:
    Menu();

    void setup() override;
    void loop() override;
    void stop() override;

    void drawMenu();
};

#endif