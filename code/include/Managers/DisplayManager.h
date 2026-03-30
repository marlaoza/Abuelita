#include <Adafruit_PCD8544.h>
#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

class DisplayManager {
    public:
        static DisplayManager& getInstance();
        Adafruit_PCD8544 screen;
    private:
        DisplayManager();
        ~DisplayManager();
};

#endif