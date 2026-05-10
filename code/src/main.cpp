#include <Arduino.h>
#include "Managers/ActivityManager.h"
#include "Managers/DisplayManager.h"
#include "Managers/InputManager.h"
#include "Managers/AudioManager.h"
#include "SystemData.h"

#include "Activities/Menu.h"
#include "Activities/Notes.h"
#include <LittleFS.h>

//MANAGERS
ActivityManager& activity = ActivityManager::getInstance();
InputManager& input = InputManager::getInstance();
DisplayManager& display = DisplayManager::getInstance();
AudioManager& audio = AudioManager::getInstance();

//APPS
Menu* MenuAPP = new Menu();
Notes* NotesAPP = new Notes();


void setup() {
    Serial.begin(115200);

    if (!LittleFS.begin(true)) {  
        Serial.println("LittleFS mount failed");
    }

    display.Splash();

    activity.registerActivity(MenuAPP);
    activity.registerActivity(NotesAPP);
    
    activity.setActivity(0);
}

void loop() {
    
    updateSystemData();
    input.readKeyPad();
    activity.loopActivity();
}
