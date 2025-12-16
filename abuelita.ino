#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Keypad.h>

//TODO 

// MAIS MINIGAMES/APPS
// - LER/ESCREVER POEMAS (quando tiver o SD, por enquanto só ler)
// - TOCA MUSICA (podendo implementar streaming tambem pegar musica do spotify, mas precios de um menu pra logar wifi)
// - FLAPPY BIRD? (faz sentido)
// - MOEDA
// - VELA

// IMPLEMENTAR TECLADO
// - Função de escrever - tecla 1 = abc, tecla 2 = def etc
// - Capturar input de teclado de forma fácil para poder usar em qualquer momento e qualquer tela/minigame

// IMPLEMENTAR GIROSCOPIO
// - Mexer o rosto do bixo baseado no giroscopio
// - shake = ligar flash e enjoo no bixo

// BIXO
// - status, demonstrar pelo rosto (fome, diversão etc)
// - mexer a mao que da comida baseado no giroscopio ou no teclado ?

// MENU
// - opções de minigames e comidas e apps (audios/musicas, texto)

// TEXTO
// -escrever poeminhas

// AUDIO
// - Microfone e Speaker (entrada de fone se der)
// - play musica, o papagaio canta
// - microfone grava voz e reproduz com um filtrozinho de audio (papagaio repete)
// - as vezes o papagaio faz uns sons ae

//STORAGE
// - cartão SD pra guardar mais musicas? 


const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1','4','7','.'},
  {'2','5','8','0'},
  {'3','6','9','B'},
  {'C','D','E','F'}
};

const char* t9Map[] = {
  "0 ",
  "1",
  "2abc",
  "3def",
  "4ghi",
  "5jkl",
  "6mno",
  "7pqrs",
  "8tuv",
  "9wxyz",
  ".,?!"
};

byte rowPins[ROWS] = {23, 22, 32, 33}; 
byte colPins[COLS] = {21, 18, 17, 16};
Keypad keypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

#define RST_PIN   26 
#define CE_PIN    15
#define DC_PIN    27  
#define DIN_PIN   13
#define CLK_PIN   14
Adafruit_PCD8544 display = Adafruit_PCD8544(CLK_PIN, DIN_PIN, DC_PIN, CE_PIN, RST_PIN);

// const unsigned char eyeMap [] PROGMEM = {
// 	0x3f, 0x80, 0x00, 0x7f, 0xf0, 0x00, 0xe0, 0xfe, 0x00, 0xc0, 0x1f, 0x00, 0xc0, 0x03, 0xc0, 0xc0, 
// 	0x01, 0xc0, 0xc0, 0x00, 0x60, 0xc0, 0x00, 0x60, 0xc0, 0x00, 0x30, 0xc0, 0x00, 0x30, 0xc0, 0x00, 
// 	0x18, 0xc0, 0x00, 0x18, 0xc0, 0x00, 0x18, 0xc0, 0x00, 0x38, 0xc0, 0x00, 0x78, 0xc0, 0x00, 0x78, 
// 	0xe0, 0x00, 0x78, 0xe0, 0x00, 0xf0, 0x70, 0x00, 0xf0, 0x78, 0x03, 0xe0, 0x3f, 0xff, 0x80, 0x07, 
// 	0xff, 0x80, 0x03, 0xff, 0x00
// };
// const unsigned char mirrorEyeMap [] PROGMEM = {
// 	0x00, 0x0f, 0xe0, 0x00, 0x7f, 0xf0, 0x03, 0xf8, 0x38, 0x07, 0xc0, 0x18, 0x1e, 0x00, 0x18, 0x1c, 
// 	0x00, 0x18, 0x30, 0x00, 0x18, 0x30, 0x00, 0x18, 0x60, 0x00, 0x18, 0x60, 0x00, 0x18, 0xc0, 0x00, 
// 	0x18, 0xc0, 0x00, 0x18, 0xc0, 0x00, 0x18, 0xe0, 0x00, 0x18, 0xf0, 0x00, 0x18, 0xf0, 0x00, 0x18, 
// 	0xf0, 0x00, 0x38, 0x78, 0x00, 0x38, 0x78, 0x00, 0x70, 0x3e, 0x00, 0xf0, 0x0f, 0xff, 0xe0, 0x0f, 
// 	0xff, 0x00, 0x07, 0xfe, 0x00
// };
// const unsigned char pupilMap [] PROGMEM = {
// 	0x3f, 0x80, 0x7f, 0xc0, 0x7e, 0x60, 0xfe, 0x30, 0xff, 0x30, 0xff, 0xf0, 0xff, 0xf0, 0x7f, 0xf0, 
// 	0x7f, 0xf0, 0x3f, 0xc0, 0x07, 0x80
// };
// const unsigned char mirrorPupilMap [] PROGMEM = {
// 	0x1f, 0xc0, 0x3f, 0xe0, 0x67, 0xe0, 0xc7, 0xf0, 0xcf, 0xf0, 0xff, 0xf0, 0xff, 0xf0, 0xff, 0xe0, 
// 	0xff, 0xe0, 0x3f, 0xc0, 0x1e, 0x00
// };

// struct Pupil {
//   int x; //  1 -> 9 {+63}
//   int y; // 5 -> 14
//   //int rotation; //0 - 3 = 0, 90, 180, 270 
// };
// struct Eye {
//   int state;
//   Pupil pupil;
//   int x;
//   int y;
// };

// void drawEye(Eye eye, bool mirror = false){
//    if(eye.state != 0){
//     Serial.println("olho fechado");
//   }else{
//     //olho
//     display.drawBitmap(eye.x, eye.y, mirror ? eyeMap : mirrorEyeMap, 21, 23, BLACK);
//     //pupila
//     display.drawBitmap(eye.pupil.x, eye.pupil.y, mirror ? pupilMap : mirrorPupilMap, 12, 11, BLACK);
//   }
// }
// void drawEyes(Eye leftEye = {0, {8, 8}, 0, 4}, Eye rightEye = {0, {71, 8}, 63, 4}){
//   drawEye(leftEye);
//   drawEye(rightEye, true);
// }

//# UTILS
void drawBitmapRotated(int16_t x, int16_t y,const uint8_t *bitmap,int w, int h, float angle) {
  float rad = (2*std::numbers::pi*angle)/360;
  
  int diag = sqrt(w*w + h*h);

  int sinA = (int)(sin(rad) * 256);
  int cosA = (int)(cos(rad) * 256);

  int cx_dst = diag / 2;
  int cy_dst = diag / 2;

  int cx_src = w / 2;
  int cy_src = h / 2;

  int bytesPerRow = (w + 7) / 8;

  for (int yy = 0; yy < diag; yy++) {
    for (int xx = 0; xx < diag; xx++) {
      
      int rx = (xx - cx_dst);
      int ry = (yy - cy_dst);

      int px = (int) (cx_src + ((rx * cosA + ry * sinA) >> 8));
      int py = (int) (cy_src + ((-rx * sinA + ry * cosA) >> 8));

      if (px < 0 || px >= w || py < 0 || py >= h)
        continue;

      int byteIndex = py * bytesPerRow + (px / 8);
      int bitIndex  = 7 - (px % 8);

      if (bitmap[byteIndex] & (1 << bitIndex)) {
        display.drawPixel(x + xx, y + yy, BLACK);
      }

      
    }
  }
}

void fillStripedRect(int x, int y, int w, int h, uint16_t color) {
  for (int i = 0; i < w; i +=2) {
    display.drawFastVLine(x + i , y, h, color);
  }
}

unsigned long currentTime = 0;
unsigned long lastPressTime = 0;
char lastKey = '\0';
int charIndex = 0;
char curChar = '\0';
String inputText = "";
String sentText = "";
int maxDigits = -1;

void readKeyBoard(){
  char key = keypad.getKey();
  if(key){
    if(key == 'B'){
      sentText = inputText;
      inputText = "";
      lastPressTime = currentTime;
    }
    else if(key == 'F'){
      if(inputText.length() > 0){
        inputText.remove(inputText.length() - 1);
      }
    }
    else{
      int keyIndex = key - '0';
      int charCount = strlen(t9Map[keyIndex]);
      if(maxDigits == -1 || charCount < maxDigits){
        if(key == lastKey && ((currentTime - lastPressTime) < 1000)){
          charIndex = (charIndex + 1) % charCount;
          curChar = t9Map[keyIndex][charIndex];
        }else{
          charIndex=0;
          if(curChar != '\0'){
            inputText += curChar;
          }
          curChar = t9Map[keyIndex][0];
          lastKey = key;
          lastPressTime = currentTime;
        }
      }
    }
  }

  if ((currentTime - lastPressTime) > 1000) {
    if(curChar != '\0'){
      inputText += curChar;
      curChar = '\0';
      lastKey = '\0';
      charIndex = 0;
      Serial.println(inputText);
    }
  }
}
//# TERMO
// TODO
// criar tela de inicio
// usar teclado no lugar do serial
// criar tela de perdeu ou ganhou
char wordBank[][6] PROGMEM = {
  "termo","teste","carta","linda","festa",
  "amigo","vento","praia","sabor","ponto",
  "linha","porta","livro","canto","chave",
  "olhar","dente","viver","corpo","sinto",
  "noite","claro","sinal","fruta","pilar",
  "cobre","mundo","verde","tente","falar",
  "mesmo","igual","jovem","grato","cegar",
  "amado","justo","risos","pedra","lugar",
  "campo","firme","rival","tocar","dorso",
  "vazio","pegar","louco","queda","brisa",
  "raiva","velho","vigor","ciclo","ideia",
  "plano","humor","afeto","barco","fecho",
  "obvio","sorte","clima","sonho","piano",
  "trono","coisa","amiga","couro","astro",
  "frase","venda","trigo","leite","sutil",
  "levar","norte","chuva","bravo","digno",
  "pular","selva","quiet","criar","amplo",
  "faixa","doido","tarde","fugir","muito",
  "gente","perto","bater","lutar","honra",
  "nuvem","magia","grato","sabor","senso",
  "carga","linha","tempo","linda","risco"
};

char targetWord[6];

int curRow = 0;
int xOffset = 17;
int yOffset= 2;
char targetWordFullCopy[6];

void drawRow(String word, int row){
  Serial.println(word);
  strcpy(targetWordFullCopy, targetWord);
  int amtRight = 0;
  int Ypos = (row*9) + yOffset;

  for (int col = 0; col < 5; col++){
    int Xpos = (col*11) + xOffset;
    if(word[col] != ' '){
      bool allRight = (word[col] == targetWordFullCopy[col]);
      if(allRight){
        display.fillRect(Xpos, Ypos, 9, 8, BLACK);
        display.setTextColor(WHITE);
        display.setCursor(Xpos+2, Ypos);
        display.print(word[col]);
        word[col] = ' ';
        targetWordFullCopy[col] = ' ';
        amtRight++;
      }
    }
  }

  for (int col = 0; col < 5; col ++){
    if(word[col] != ' '){
      int Xpos = (col*11) + xOffset;
      char *ptr = strchr(targetWordFullCopy, word[col]);
      bool letterRight = ptr != NULL;
      if(letterRight){
          int index = ptr - targetWordFullCopy;
          targetWordFullCopy[index] = ' ';
          display.drawRect(Xpos, Ypos, 9, 8, BLACK);
      }
      else{
        display.fillRect(Xpos, Ypos, 9, 8, WHITE);
      }
      display.setTextColor(BLACK);
      display.setCursor(Xpos+2, Ypos);
      display.print(word[col]);
    }
  }

  display.display();
  if(amtRight == 5){
    Serial.println("venceu");
  }
}

void drawMap() {
  for (int row = 0; row < 5; row++){
    for (int col = 0; col<5; col++){
      int Xpos = (col*11) + xOffset;
      int Ypos = (row*9) + yOffset;
      display.drawRect(Xpos, Ypos, 9, 8, BLACK);
    }
  }
  display.display();
}

bool playingTermo;

void setupTermo(){
  Serial.println("Setup Termo");

  strcpy_P(targetWord, wordBank[random(0, sizeof(wordBank) / sizeof(wordBank[0]) - 1 )]);

  curRow = 0;
  maxDigits = 5;
  
  display.clearDisplay();
  display.setTextSize(1);
  drawMap();
  
  playingTermo = true;
}

void loopTermo(){
  if(playingTermo){
    if(sentText != ""){
      drawRow(sentText, curRow);
      curRow++;
      sentText = "";
    }

    else{
      int Ypos = (curRow*9) + yOffset;
      for (int col = 0; col < 5; col++){
        int Xpos = (col*11) + xOffset;
        display.setCursor(Xpos+2, Ypos);
        display.print(inputText[col]);
      }
      display.display();
    }

    if(curRow == 5){
      Serial.println("perdeu");
    }
   

  }
}

//# CARAOUCOROA
// TODO
// criar tela de inicio
// giroscopio gira moeda
// desenhar moeda
// animar giro

const unsigned char moeda [] PROGMEM = {
	0x00, 0x0f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x3c, 0x00, 0x03, 0x07, 0xff, 0xff, 
	0x83, 0x80, 0x0c, 0x7f, 0xff, 0xf8, 0xf8, 0x60, 0x11, 0xcf, 0xff, 0xfe, 0x0f, 0x18, 0x27, 0x7f, 
	0xff, 0xff, 0xe3, 0xc4, 0x4f, 0xff, 0xff, 0xff, 0xf9, 0xf2, 0x9f, 0xff, 0xff, 0xff, 0xff, 0x99, 
	0xbf, 0xff, 0xff, 0xff, 0xff, 0x8d, 0xbf, 0xff, 0xff, 0xff, 0xff, 0x85, 0xbf, 0xff, 0xff, 0xff, 
	0xff, 0x05, 0xbf, 0xff, 0xff, 0xff, 0xff, 0x05, 0xbf, 0xff, 0xff, 0xff, 0xff, 0x05, 0xdf, 0xff, 
	0xff, 0xff, 0xfe, 0x0d, 0xef, 0xff, 0xff, 0xff, 0xfc, 0x3d, 0x77, 0xff, 0xff, 0xff, 0xe0, 0x7b, 
	0x39, 0xcf, 0xff, 0xff, 0x80, 0xe6, 0x1e, 0x7b, 0xff, 0xfc, 0x03, 0xcc, 0x0f, 0x8e, 0x08, 0x00, 
	0x3e, 0x78, 0x07, 0xf3, 0xff, 0xff, 0xe0, 0xf0, 0x01, 0xf8, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x3f, 
	0xff, 0xff, 0xff, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf8, 0x00
};
const unsigned char moedaCara [] PROGMEM = {
	0x00, 0x0f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x3c, 0x00, 0x03, 0x07, 0xff, 0xff, 
	0x83, 0x80, 0x0c, 0x7f, 0xff, 0xf8, 0xf8, 0x60, 0x11, 0xcf, 0xff, 0xfe, 0x0f, 0x18, 0x27, 0x7f, 
	0xff, 0xff, 0xe3, 0xc4, 0x4f, 0xff, 0xff, 0xff, 0xf9, 0xf2, 0x9f, 0xff, 0xff, 0xff, 0xff, 0x99, 
	0xbf, 0xff, 0xff, 0xff, 0xff, 0x8d, 0xbf, 0xff, 0xff, 0xff, 0xff, 0x85, 0xbf, 0xff, 0xff, 0xff, 
	0xff, 0x05, 0xbf, 0xff, 0xff, 0xff, 0xff, 0x05, 0xbf, 0xff, 0xff, 0xff, 0xff, 0x05, 0xdf, 0xff, 
	0xff, 0xff, 0xfe, 0x0d, 0xef, 0xff, 0xff, 0xff, 0xfc, 0x3d, 0x77, 0xff, 0xff, 0xff, 0xe0, 0x7b, 
	0x39, 0xcf, 0xff, 0xff, 0x80, 0xe6, 0x1e, 0x7b, 0xff, 0xfc, 0x03, 0xcc, 0x0f, 0x8e, 0x08, 0x00, 
	0x3e, 0x78, 0x07, 0xf3, 0xff, 0xff, 0xe0, 0xf0, 0x01, 0xf8, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x3f, 
	0xff, 0xff, 0xff, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf8, 0x00
};
const unsigned char moedaCoroa [] PROGMEM = {
	0x00, 0x0f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x3c, 0x00, 0x03, 0x07, 0xff, 0xff, 
	0x83, 0x80, 0x0c, 0x7f, 0xff, 0xf8, 0xf8, 0x60, 0x11, 0xcf, 0xff, 0xfe, 0x0f, 0x18, 0x27, 0x7f, 
	0xff, 0xff, 0xe3, 0xc4, 0x4f, 0xff, 0xff, 0xff, 0xf9, 0xf2, 0x9f, 0xff, 0xff, 0xff, 0xff, 0x99, 
	0xbf, 0xff, 0xff, 0xff, 0xff, 0x8d, 0xbf, 0xff, 0xff, 0xff, 0xff, 0x85, 0xbf, 0xff, 0xff, 0xff, 
	0xff, 0x05, 0xbf, 0xff, 0xff, 0xff, 0xff, 0x05, 0xbf, 0xff, 0xff, 0xff, 0xff, 0x05, 0xdf, 0xff, 
	0xff, 0xff, 0xfe, 0x0d, 0xef, 0xff, 0xff, 0xff, 0xfc, 0x3d, 0x77, 0xff, 0xff, 0xff, 0xe0, 0x7b, 
	0x39, 0xcf, 0xff, 0xff, 0x80, 0xe6, 0x1e, 0x7b, 0xff, 0xfc, 0x03, 0xcc, 0x0f, 0x8e, 0x08, 0x00, 
	0x3e, 0x78, 0x07, 0xf3, 0xff, 0xff, 0xe0, 0xf0, 0x01, 0xf8, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x3f, 
	0xff, 0xff, 0xff, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf8, 0x00
};
unsigned long flipStartTime = 0;
unsigned long flipTime = 3000;
void setupCoinFlip(){
  display.clearDisplay();
  display.drawBitmap(18, 25, moeda, 48, 23, BLACK);
  display.display();      

  for (int h = 0; h < 10; h++){
    display.clearDisplay();
    display.drawBitmap(18, 25 - h, moeda, 48, 23, BLACK);
    display.display();      
    delay(40);
  }

  currentTime = millis();
  flipStartTime = currentTime;
  while (currentTime - flipStartTime <= flipTime) {
    for (int r = 0; r < 360; r+=20) {
      display.clearDisplay();
      drawBitmapRotated(16, 0, moeda, 48, 23, r);
      display.display();      
      delay(40);
      currentTime = millis();

    }
  }

  currentTime = millis();
  flipStartTime = currentTime;
  while (currentTime - flipStartTime <= flipTime) {
    display.clearDisplay();
    drawBitmapRotated(16, 0, moeda, 48, 23, 0);
    display.display();      
    delay(90);
    display.clearDisplay();
    drawBitmapRotated(16, 0, moeda, 48, 23, 15);
    display.display();      
    delay(90);
    display.clearDisplay();
    drawBitmapRotated(16, 0, moeda, 48, 23, 0);
    display.display();      
    delay(90);
    display.clearDisplay();
    drawBitmapRotated(16, 0, moeda, 48, 23, 345);
    display.display();      
    delay(90);
    
    currentTime = millis();

  }

  for (int h = 0; h < 10; h++){
    display.clearDisplay();
    display.drawBitmap(18, 15 + h, moeda, 48, 23, BLACK);
    display.display();      
    delay(40);
  }

  delay(80);
  display.setTextSize(2);
  display.setTextColor(BLACK);


  //aleatorizar
  int i = random(0, 2);
  String text = i == 0 ?  "CARA" : "COROA";

  display.clearDisplay();
  display.drawBitmap(18, 25, i == 0 ? moedaCara : moedaCoroa, 48, 23, BLACK);

  int16_t x1, y1; 
  uint16_t w, h; 
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  int16_t x = (84 - w) / 2;
  display.setCursor(x, 2);

  display.print(text);
  display.display();      
  
}


struct activity {
  String name;
  void (*pSetup)();
  void (*pLoop)(); 
};
struct activity activities[5];
activity* curActivity;


// void changeActivity(activity* pAct){
//   curActivity = pAct;
//   curActivity->pSetup();
// }

void setup()   {  
  Serial.begin(115200);
  display.begin();

  display.setContrast(90);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.display();


  activities[0].name = "TERMO";
  activities[0].pSetup = setupTermo;
  activities[0].pLoop = loopTermo;

  curActivity = &activities[0];
  curActivity->pSetup();

  //changeActivity(&activities[0]);

  // Pupil leftPupil = {7, 9};
  // Eye leftEye = {0, leftPupil, 0, 4};

  // Pupil rightPupil = {65, 9};
  // Eye rightEye = {0, rightPupil, 63, 4};

  // drawEyes(rightEye, leftEye);
  //setupWifi();
  //setupTermo();
  //setupCoinFlip();
	
  

}
void loop(){
  currentTime = millis();
  readKeyBoard();
  curActivity->pLoop();
 
  
}
