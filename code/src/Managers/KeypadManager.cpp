#include "Managers/KeyPadManager.h"
#include "PinConfig.h"
#include "SystemData.h"

KeyPadManager& KeyPadManager::getInstance() {static KeyPadManager instance; return instance;}

KeyPadManager::KeyPadManager() : _keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS){
    this->resetKeyPadVariables();
}

KeyPadManager::~KeyPadManager(){}


void KeyPadManager::readKeyPad(){
    char key = _keypad.getKey();
    if(key){
        if(this->_mode == WRITE_MODE){
            switch (key)
            {
                case 'B':
                    if(this->_inputText.length() >= this->minDigits){
                        this->_sentText = this->_inputText;
                        this->_inputText = "";
                        this->_lastPressTime = currentTime;
                    }
                    break;
                case 'F':
                    if(this->_inputText.length() > 0){
                        this->_inputText.remove(this->_inputText.length() - 1);
                    }
                    break;
                
                default:
                    if(key >= '0' && key <= '9' && (this->_inputText.length() < this->maxDigits || this->maxDigits == -1)){
                        int keyIndex = key - '0';
                        int charCount = strlen(t9Map[keyIndex]);

                        if(key == this->_lastKey && ((currentTime - this->_lastPressTime) < 1000)){
                            this->_charIndex = (this->_charIndex + 1) & charCount;
                        }else{
                            this->_charIndex = 0;
                            if(this->_curChar != '\0'){this->_inputText += _curChar;}
                            this->_lastKey = key;
                        }
                        this->_curChar = t9Map[keyIndex][this->_charIndex];
                        this->_lastPressTime = currentTime;
                    }
                    break;
            }

            if ((currentTime - this->_lastPressTime) > 1000) {
                if(this->_curChar != '\0'){
                    this->_inputText += this->_curChar;
                    this->_curChar = '\0';
                    this->_lastKey = '\0';
                    this->_charIndex = 0;
                }
            }
        }
    }

    
}

bool KeyPadManager::getKey(char key){
    return _keypad.getKey() == key;
}

char KeyPadManager::readKey(){
    return _keypad.getKey();
}

void KeyPadManager::resetKeyPadVariables(){
    this->_mode = WRITE_MODE;
    this->maxDigits = -1;
    this->minDigits = 0;
    this->_inputText = "";
    this->_sentText = "";
    this->_curChar = '\0';
    this->_charIndex = 0;
    this->_lastKey = '\0';
}

String KeyPadManager::readRawText(){return _inputText + _curChar;}
String KeyPadManager::readSentText(){
    String tr = this->_sentText;
    this->_sentText = "";
    return tr;
}
void KeyPadManager::setMode(byte mode){
    if(mode == WRITE_MODE || mode == PRESS_MODE) this->_mode = mode;
}