#ifndef ACTIVITY_NOTES_H
#define ACTIVITY_NOTES_H
#include "Activity.h"
#include <Managers/KeypadManager.h>
#include <Managers/DisplayManager.h>
#include <vector>


typedef struct note {
  String name;
  String text;
};

std::vector<note> notes;

class Notes : public Activity {
private:
    note* openNote;
    bool onMenu;
    int fixedOffset;
    int hoveredNota;
    int selectedNota;
    bool onNewNota;
    int hoverNewNotaItem;
    bool onInput;
    String noteName;
    int lastHoveredNota;
    KeyPadManager& keyPadManager;
    DisplayManager& displayManager;
   
public:
    Notes();

    void setup() override;
    void loop() override;
    void stop() override;

    void drawNotesMenu();
    void drawNote();
    void selectNote(int id);
};

#endif