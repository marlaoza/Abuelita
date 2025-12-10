#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Wire.h>
#include <Preferences.h>
Preferences preferences;
 
//TODO 

// MAIS MINIGAMES/APPS
// - LER/ESCREVER POEMAS (quando tiver o SD, por enquanto só ler)
// - TOCA MUSICA (podendo implementar streaming tambem pegar musica do spotify, mas precios de um menu pra logar wifi)
// - FLAPPY BIRD? (faz sentido)

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

//ALGUMA COISA PRA FAZER COM WIFI / BLUETOOH
//streaming de musica hmm.;


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

//# TERMO
void fillStripedRect(int x, int y, int w, int h, uint16_t color) {
  for (int i = 0; i < w; i +=2) {
    display.drawFastVLine(x + i , y, h, color);
  }
}
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
char rows[5][5] = { 
  { ' ', ' ', ' ', ' ', ' ' },
  { ' ', ' ', ' ', ' ', ' ' },
  { ' ', ' ', ' ', ' ', ' ' },
  { ' ', ' ', ' ', ' ', ' ' },
  { ' ', ' ', ' ', ' ', ' ' },
};
int curRow = 0;
int xOffset = 17;
int yOffset= 2;
char targetWordFullCopy[6];
char targetWordSemiCopy[6];

//se tiver uma letra na palavra target e a mesma letra na palavra enviada duas vezes, uma na posição errada e uma na certa, ele vai flagar duas vezes
//exemplo 
// target: TESTE
// enviado: TSSTE
// ficaria o primeiro S marcado como meio certo e o segundo S marcado como certo, dando a impressão que tem dois s na palavra. (arrumar)
void drawRow(int row){
  strcpy(targetWordFullCopy, targetWord);
  strcpy(targetWordSemiCopy, targetWord);
  int amtRight = 0;
  int Ypos = (row*9) + yOffset;
  for (int col = 0; col < 5; col++){
    int Xpos = (col*11) + xOffset;
    char curChar = rows[row][col];
    if(curChar != ' '){
      bool allRight = (curChar == targetWordFullCopy[col]);
      if(allRight){
        display.fillRect(Xpos, Ypos, 9, 8, BLACK);
        targetWordFullCopy[col] = ' ';
        targetWordSemiCopy[col] = ' ';
        amtRight++;
      }
      else{
        char *ptr = strchr(targetWordSemiCopy, curChar);
        bool letterRight = ptr != NULL;
        if(letterRight){
          int index = ptr - targetWordSemiCopy;
          targetWordSemiCopy[index] = ' ';
          fillStripedRect(Xpos, Ypos, 9, 8, BLACK);
        }
      }
      display.setTextColor(allRight ? WHITE : BLACK);
      display.setCursor(Xpos+2, Ypos);
      display.print(curChar);
      
      //ou mover pra depois de todas as rows 
      display.display();

    }
  }
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
  strcpy_P(targetWord, wordBank[random(0, sizeof(wordBank) / sizeof(wordBank[0]) - 1 )]);
  memset(rows, ' ', sizeof(rows));

  Serial.println(targetWord);
  curRow = 0;
  
  display.clearDisplay();
  display.setTextSize(1);
  drawMap();
  
  playingTermo = true;
  while(playingTermo){
    if(Serial.available() > 0){
      String receivedString = Serial.readString();
      receivedString.trim();
      Serial.println(receivedString);

      for (int i = 0; i < 5; i ++){
        rows[curRow][i] = receivedString[i];
      }
      drawRow(curRow);

      curRow ++;
      if(curRow == 5){
        Serial.println("perdeu");
      }
    }

  }
}


unsigned int hunger = 0;
unsigned int age = 0;

void setup()   {  
  Serial.begin(115200);
  preferences.begin("status", false);

  hunger = preferences.getUInt("hunger", 100);
  age = preferences.getUInt("age", 0);
  preferences.putUInt("hunger", hunger);
  preferences.putUInt("age", age);

  preferences.end();

  display.begin();

  display.setContrast(90);
  display.clearDisplay();

  // Pupil leftPupil = {7, 9};
  // Eye leftEye = {0, leftPupil, 0, 4};

  // Pupil rightPupil = {65, 9};
  // Eye rightEye = {0, rightPupil, 63, 4};

  // drawEyes(rightEye, leftEye);
  //
  setupTermo();
	display.display();
  

}
void loop(){

 

}