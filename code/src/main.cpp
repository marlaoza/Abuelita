#include <Arduino.h>
#include "Managers/ActivityManager.h"
#include "Managers/DisplayManager.h"
#include "Managers/KeyPadManager.h"
#include "SystemData.h"

#include "Activities/Menu.h"
#include "Activities/Notes.h"

//MANAGERS
ActivityManager& activity = ActivityManager::getInstance();
KeyPadManager& keypad = KeyPadManager::getInstance();
DisplayManager& display = DisplayManager::getInstance();

//APPS
Menu* MenuAPP = new Menu();
Notes* NotesAPP = new Notes();


void setup() {
    Serial.begin(115200);

    activity.registerActivity(MenuAPP);
    activity.registerActivity(NotesAPP);
    
    activity.setActivity(0);
}

void loop() {
    updateSystemData();
    activity.loopActivity();
}