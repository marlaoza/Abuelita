#include <Arduino.h>
#include "Managers/ActivityManager.h"
#include "Managers/DisplayManager.h"
#include "Managers/KeyPadManager.h"
#include "SystemData.h"
#include "Activities/Menu.h"

//MANAGERS
ActivityManager& activity;
KeyPadManager& keypad;
DisplayManager& display;

//APPS
Menu* MenuAPP = new Menu();


void setup() {
    Serial.begin(115200);
    activity = ActivityManager::getInstance();
    keypad = KeyPadManager::getInstance();
    display = DisplayManager::getInstance();

    activity.registerActivity(MenuAPP);
    activity.setActivity(0);
}

void loop() {
    updateSystemData();
    activity.loopActivity();
}