#include "Managers/InputManager.h"
#include "PinConfig.h"
#include "SystemData.h"
#include <Managers/ActivityManager.h>

InputManager& InputManager::getInstance() {static InputManager instance; return instance;}

InputManager::InputManager() : _keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS){
    this->resetKeyPadVariables();
    pinMode(RE_CLK, INPUT);
    pinMode(RE_DT, INPUT);
    this->_prev_CLK_state = digitalRead(RE_CLK);
    // attachInterrupt(digitalPinToInterrupt(RE_CLK), handleREInterrupt, CHANGE);
}

void InputManager::handleREInterrupt(){
    if(digitalRead(RE_CLK) == digitalRead(RE_DT)){
        Serial.println("clock");
    }else{
        Serial.println("counter clock");
    }
}

InputManager::~InputManager(){}


void InputManager::readKeyPad(){
    _key = _keypad.getKey();
    if(_key){
        Serial.println(_key);
        if(_key == 'M'){
            Serial.println("menu");
            ActivityManager::getInstance().setActivity(0);
            return;
        }

        if(this->_mode == WRITE_MODE){
            switch (_key)
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
                    if(_key >= '0' && _key <= '9' && (this->_inputText.length() < this->maxDigits || this->maxDigits == -1)){
                        int keyIndex = _key - '0';
                        int charCount = strlen(t9Map[keyIndex]);

                        if(_key == this->_lastKey && ((currentTime - this->_lastPressTime) < 1000)){
                            this->_charIndex = (this->_charIndex + 1) & charCount;
                        }else{
                            this->_charIndex = 0;
                            if(this->_curChar != '\0'){this->_inputText += _curChar;}
                            this->_lastKey = _key;
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

bool InputManager::getKey(char key){
    return _key == key;
}

char InputManager::readKey(){
    return _key;
}

void InputManager::resetKeyPadVariables(){
    this->_mode = WRITE_MODE;
    this->maxDigits = -1;
    this->minDigits = 0;
    this->_inputText = "";
    this->_sentText = "";
    this->_curChar = '\0';
    this->_charIndex = 0;
    this->_lastKey = '\0';
}

String InputManager::readRawText(){return _inputText + _curChar;}
String InputManager::readSentText(){
    String tr = this->_sentText;
    this->_sentText = "";
    return tr;
}
void InputManager::setMode(byte mode){
    if(mode == WRITE_MODE || mode == PRESS_MODE) this->_mode = mode;
}