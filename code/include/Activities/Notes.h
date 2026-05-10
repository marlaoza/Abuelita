#ifndef ACTIVITY_NOTES_H
#define ACTIVITY_NOTES_H
#include "Activity.h"
#include <Managers/InputManager.h>
#include <Managers/DisplayManager.h>
#include <vector>

typedef struct note {
  int id;
  String name;
  String text;
};

extern std::vector<note> notesList;

class Notes : public Activity {
private:
    note* openNote;
    bool onMenu;
    int fixedOffset;
    int hoveredNota;
    int selectedNota;
    bool onNewNota;
    uint8_t hoverNewNotaItem;
    bool onInput;
    String noteName;
    int lastHoveredNota;
    InputManager& inputManager;
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