#include "Activities/Notes.h"
#include "Assets.h"

Notes::Notes() 
    : Activity(ico_notes), 
      keyPadManager(KeyPadManager::getInstance()), displayManager(DisplayManager::getInstance()) {}

void Notes::setup() {
    displayManager.screen.clearDisplay();
    notes.push_back({ "nota0", "" });
    notes.push_back({ "nota1", "" });
    notes.push_back({ "nota2", "" });
    notes.push_back({ "nota3", "" });
    notes.push_back({ "nota4", "" });
    notes.push_back({ "nota5", "" });
    notes.push_back({ "nota6", "" });
    notes.push_back({ "nota7", "" });
    notes.push_back({ "nota8", "" });
    notes.push_back({ "nota9", "" });
    notes.push_back({ "nota10", "" });
    keyPadManager.resetKeyPadVariables();
    onMenu = true;
    openNote = nullptr;
    fixedOffset = 0;
    hoveredNota = 0;
    selectedNota = 0;
    onNewNota = false;
    hoverNewNotaItem = 0;
    onInput = false;
    noteName = "";
    drawNotesMenu();
 
}

void Notes::loop() {
    char key = keyPadManager.readKey();
   if(onMenu){
      if(key){
        if(onInput && onNewNota){
          noteName = keyPadManager.readRawText();
          if(key == 'B'){
            onInput = false;
            keyPadManager.setMode(PRESS_MODE);
            keyPadManager.maxDigits = -1;
            noteName = keyPadManager.readSentText();
            
          }
        }else{
          switch(key){
            case '2':
              if(!onNewNota){if(hoveredNota >= 1) hoveredNota -= 1;}
              else{hoverNewNotaItem = hoverNewNotaItem == 1 ? 0 : 1;}
              break;
            case '8':
              if(!onNewNota){if(hoveredNota < (notes.size()) - 1){ hoveredNota += 1;}}
              else{hoverNewNotaItem = hoverNewNotaItem == 0 ? 1 : 0;}
              break;
            case '6':
              if(!onNewNota){onNewNota = true; lastHoveredNota = hoveredNota; hoveredNota = -1;}
              break;
            case '4':
              if(onNewNota){onNewNota = false; hoveredNota = lastHoveredNota;}
              break;
            case '5':
              if(!onNewNota){selectNote(hoveredNota);}
              if(onNewNota){
                if(hoverNewNotaItem == 1){
                  notes.push_back({noteName, "" });
                  selectNote(notes.size()-1);
                }else if(hoverNewNotaItem == 0){
                  if(!onInput){
                    keyPadManager.resetKeyPadVariables();
                    keyPadManager.maxDigits = 5;
                    onInput = true;
                    
                  };
                }
              }
              break;
          }
        }
        drawNotesMenu();
      }
    }else{
      if(key == 'F'){
        if(keyPadManager.readRawText().length() <= 0){
          if(openNote->text.length() > 0){
            openNote->text.remove(openNote->text.length() - 1);
          }
        }
      }
      if(key == 'B'){
        openNote->text += (keyPadManager.readSentText() + "\n");
      }
      if(key == 'Z'){
        onMenu = true;
        drawNotesMenu();
        return;
      }

    //   if(isRotating){
    //     if(direction == DIRECTION_CW){
    //       fixedOffset += 1;
    //     }else{
    //       fixedOffset -= 1;
    //     }
    //   }
      
      drawNote();
    }
}

void Notes::stop() {
    displayManager.screen.clearDisplay();
    keyPadManager.resetKeyPadVariables();
    onMenu = true;
    openNote = nullptr;
    fixedOffset = 0;
    hoveredNota = 0;
    selectedNota = 0;
    onNewNota = false;
    hoverNewNotaItem = 0;
    onInput = false;
    noteName = "";
}

void Notes::drawNotesMenu() {
    displayManager.screen.clearDisplay();
    displayManager.screen.drawBitmap(0,0,notes_background,84,48,BLACK);
    int xOffset = 3;
    int yOffset = 3;
    int Xpos = 0;
    int Ypos = 0;
    int index = 0;

    int width = 38;
    int height = 11;
    
    if((hoveredNota*(height+1)) + yOffset + height > 44) {
        Serial.println("fora");
        yOffset -= (((hoveredNota*(height+1)) + yOffset + height) - 44);
    }

    for (int i = 0; i < notes.size(); i++){
        Xpos = xOffset;
        Ypos = (i*(height+1)) + yOffset;
        displayManager.screen.setTextColor(BLACK);
        displayManager.screen.drawRect(Xpos, Ypos, width, height, BLACK);

        if(hoveredNota == i){
        displayManager.screen.setTextColor(WHITE);
        displayManager.screen.fillRect(Xpos, Ypos, width, height, BLACK);
        }

        displayManager.screen.setCursor(Xpos+2, Ypos+2);
        displayManager.screen.print(notes[i].name);
    }

    displayManager.screen.setTextColor(BLACK);

    if(onNewNota && hoverNewNotaItem == 0){
        displayManager.screen.drawFastHLine(46, 28, 35, BLACK);
    }

    displayManager.screen.setCursor(46, 20);
    displayManager.screen.print(noteName);
    
    if(onNewNota && hoverNewNotaItem == 1){
        displayManager.screen.drawRect(51, 34, 27, 9, BLACK);
    }

    displayManager.screen.display();
}

void Notes::drawNote(){
    displayManager.screen.clearDisplay();
    String txt = openNote->text + keyPadManager.readRawText();
    int offset = (txt.length() - 56);
    offset = offset < 0 ? 0 : ceil((offset + 0.5) / 14);

    displayManager.screen.setTextColor(BLACK);
    displayManager.screen.setCursor(0, 13 - ((offset * 7) + fixedOffset));
    displayManager.screen.print(txt);

    displayManager.screen.fillRect(0, 0, 84, 9, BLACK);

    displayManager.screen.drawFastHLine(0, 10, 84, WHITE);
    displayManager.screen.drawFastHLine(0, 11, 84, BLACK);
    displayManager.screen.drawFastHLine(0, 47, 84, BLACK);

    displayManager.screen.setTextSize(1);
    displayManager.screen.setTextColor(WHITE);
    displayManager.screen.setCursor(1, 1);
    displayManager.screen.print(openNote->name);

    
    displayManager.screen.display();
}

void Notes::selectNote(int id){
    openNote = &notes[id];
    onMenu = false;
    keyPadManager.resetKeyPadVariables();
    drawNote();
}