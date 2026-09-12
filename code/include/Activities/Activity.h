#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <Arduino.h>
#include <map>
#include <functional>
#include <Managers/InputManager.h>
#include <Managers/DisplayManager.h>

struct Page {
    std::function<void()> draw;
    std::function<void()> loop;
};

class Activity {
    public:
        int id;
        const  unsigned char* icon;

        Activity(const unsigned char* icon) 
        : 
        icon(icon), inputManager(InputManager::getInstance()), displayManager(DisplayManager::getInstance()) 
        {}
        virtual ~Activity() {};
        
        virtual void setup() = 0;
        virtual void loop() = 0;
        virtual void stop() = 0;

    protected:
        InputManager& inputManager;
        DisplayManager& displayManager;

        Page* currentPage = nullptr;
        std::map<String, Page> pages;

        void setPage(String name) {
            this->currentPage = &pages[name];
            this->currentPage->draw();
        }
        void registerPage(String name, Page page){pages[name] = page;}
};

#endif