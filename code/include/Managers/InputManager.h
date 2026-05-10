#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <Arduino.h>
#include "Constants.h"
#include <Keypad.h>
#include <PinConfig.h>

static const char t9Map[10][5] = {
  " ",
  ".,()",
  "abc",
  "def",
  "ghi",
  "jkl",
  "mno",
  "pqrs",
  "tuv",
  "wxyz",
};

static const byte ROWS = 4;
static const byte COLS = 4;
static char hexaKeys[ROWS][COLS] = {
    {'1','4','7','.'},
    {'2','5','8','0'},
    {'3','6','9','B'},
    {'H','M','Z','F'}
};
static byte rowPins[ROWS] = {KEYPAD_R1, KEYPAD_R2, KEYPAD_R3, KEYPAD_R4};
static byte colPins[COLS] = {KEYPAD_C1, KEYPAD_C2, KEYPAD_C3, KEYPAD_C4};

#define WRITE_MODE 0
#define PRESS_MODE 1

#define DIRECTION_NONE 0
#define DIRECTION_CW 1
#define DIRECTION_CCW 2

class InputManager {
    public:
        static InputManager& getInstance();
        void readKeyPad();
        bool getKey(char key);
        char readKey();
        void resetKeyPadVariables();
        String readRawText();
        String readSentText();
        void setMode(byte mode);
        uint8_t maxDigits;
        uint8_t minDigits;
        static volatile uint8_t encoderDirection;

    private:
        InputManager();
        ~InputManager();
        void handleREInterrupt();
        void handleKeyPadInterrupt();
        unsigned long _lastPressTime;
        char _key;
        char _lastKey;
        uint8_t _charIndex;
        char _curChar;
        String _inputText;
        String _sentText;
        byte _mode;
        Keypad _keypad;
        uint8_t _CLK_state;
        uint8_t _prev_CLK_state;
        
};

#endif