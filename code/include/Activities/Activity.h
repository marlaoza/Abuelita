#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <Arduino.h>

class Activity {
    public:
        int id;
        const  unsigned char* icon;

        Activity(const unsigned char* icon) : icon(icon) {}
        virtual ~Activity() {};
        
        virtual void setup() = 0;
        virtual void loop() = 0;
        virtual void stop() = 0;
};

#endif