
#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Adafruit_PCD8544.h>

class DisplayManager {
    public:
        static DisplayManager& getInstance();
        Adafruit_PCD8544 screen;
        void Splash();
    private:
        DisplayManager();
        ~DisplayManager();
};

#endif