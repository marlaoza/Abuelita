#ifndef ACTIVITY_TERMO_H
#define ACTIVITY_TERMO_H
#include "Activity.h"
#include <Managers/InputManager.h>
#include <Managers/DisplayManager.h>
#include <vector>

class Termo : public Activity {
private:
    String currentWord;
    int curRow = 0;
    void startGame();

    bool won = false;

    int xOffset = 17;
    int yOffset = 2;

    void drawRow(String word);

   
public:
    Termo();

    void setup() override;
    void loop() override;
    void stop() override;
};

#endif