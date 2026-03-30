#ifndef SYSTEM_DATA_H
#define SYSTEM_DATA_H

#include <Arduino.h>

unsigned long currentTime = 0;

void updateSystemData(){
    currentTime = millis();
}

#endif