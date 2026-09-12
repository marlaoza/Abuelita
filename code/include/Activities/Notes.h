#ifndef ACTIVITY_NOTES_H
#define ACTIVITY_NOTES_H
#include "Activity.h"
#include <vector>

struct note {
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
    int scroll;
   
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