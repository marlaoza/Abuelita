#include "Activities/Notes.h"
#include "Assets.h"
#include <FS.h>
#include <LittleFS.h> 
#include <ArduinoJson.h> 

std::vector<note> notesList;

Notes::Notes() 
    : Activity(ico_notes), 
      inputManager(InputManager::getInstance()), displayManager(DisplayManager::getInstance()) {}

void Notes::setup() {


    displayManager.screen.clearDisplay();
    notesList.clear();

    if (!LittleFS.exists("/notes")) {
        bool ok = LittleFS.mkdir("/notes");
        if (!ok) {
            Serial.println("mkdir failed, formatting...");
            LittleFS.format();
            LittleFS.begin();
            LittleFS.mkdir("/notes");
        }
    }
    File dir = LittleFS.open("/notes");
    if (dir && dir.isDirectory()) {
       File file = dir.openNextFile();
      while (file) {
          if (!file.isDirectory()) {
              StaticJsonDocument<512> doc;
              deserializeJson(doc, file);
              notesList.push_back({
                  doc["id"].as<int>(),
                  doc["name"].as<String>(),
                  doc["text"].as<String>()
              });
          }
          file = dir.openNextFile();
      }
    };

    inputManager.resetKeyPadVariables();
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

void saveNote(note note) {
   if (note.id == 0) note.id = notesList.size() + 1;
   
    Serial.println(note.id);
    Serial.println(note.text);

    File f = LittleFS.open(String("/notes") + "/" + String(note.id) + ".json", "w");
    if (!f) return;

    StaticJsonDocument<512> doc;
    doc["id"]   = note.id;
    doc["name"] = note.name;
    doc["text"] = note.text;
    serializeJson(doc, f);
    f.close();

    for (auto& n : notesList) {
        if (n.id == note.id) { n = note; return; }
    }
    notesList.push_back(note);
}

void Notes::loop() {
    char key = inputManager.readKey();
   if(onMenu){
      if(key){
        if(onInput && onNewNota){
          noteName = inputManager.readRawText();
          if(key == 'F'){
            onInput = false;
            inputManager.setMode(PRESS_MODE);
            inputManager.maxDigits = -1;
            noteName = inputManager.readSentText();
            
          }
        }else{
          switch(key){
            case '2':
              if(!onNewNota){if(hoveredNota >= 1) hoveredNota -= 1;}
              else{hoverNewNotaItem = hoverNewNotaItem == 1 ? 0 : 1;}
              break;
            case '8':
              if(!onNewNota){if(hoveredNota < (notesList.size()) - 1){ hoveredNota += 1;}}
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
                  saveNote({0, noteName, ""});
                  selectNote(notesList.size()-1);
                }else if(hoverNewNotaItem == 0){
                  if(!onInput){
                    inputManager.resetKeyPadVariables();
                    inputManager.maxDigits = 5;
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
        if(inputManager.readRawText().length() <= 0){
          if(openNote->text.length() > 0){
            openNote->text.remove(openNote->text.length() - 1);
          }
        }
      }
      if(key == 'B'){
        openNote->text += (inputManager.readSentText() + "\n");
      }
      if(key == 'Z'){
        openNote->text += inputManager.readRawText();
        saveNote(*openNote);
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
    notesList.clear();
    displayManager.screen.clearDisplay();
    inputManager.resetKeyPadVariables();
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
        yOffset -= (((hoveredNota*(height+1)) + yOffset + height) - 44);
    }

    for (int i = 0; i < notesList.size(); i++){
        Xpos = xOffset;
        Ypos = (i*(height+1)) + yOffset;
        displayManager.screen.setTextColor(BLACK);
        displayManager.screen.drawRect(Xpos, Ypos, width, height, BLACK);

        if(hoveredNota == i){
        displayManager.screen.setTextColor(WHITE);
        displayManager.screen.fillRect(Xpos, Ypos, width, height, BLACK);
        }

        displayManager.screen.setCursor(Xpos+2, Ypos+2);
        displayManager.screen.print(notesList[i].name);
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
    String txt = openNote->text + inputManager.readRawText();
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
    openNote = &notesList[id];
    onMenu = false;
    inputManager.resetKeyPadVariables();
    drawNote();
}