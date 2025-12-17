#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Keypad.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <vector>
//TODO

//GERAL
// Chacoalhar o aparelho liga o led igual motorola em qualquer app
// Animação de startup

//HOME
// Rosto do papagaio cobre tudo, reage ao giroscopio (gira os olhos, se chacoalhar, fica enjoado)
// Mostrar data/hora e bateria
// Botao para abrir menu de apps
// Botao para gravar audio e o papagaio repetir (repetir pitch mais alto)
// Reagir a musica (como se o papagaio estivesse tocando)
// Acessado a qualquer momento com a tecla H

//MENU
// Colocar icones

//CARA OU COROA
// Giroscopio para lançar tambem

//TERMO
// Tela de vitória / derrota e restart

//VELA
// Mover isqueiro com 2^ 4< 6> 8V ou com giroscopio
// Chegar perto da vela apertar 5 para acender (ligar led junto)
// Salvar vela salva com um titulo
// Ver velas salvas (velas acesas ligar o led)

//ARQUI-TOOLS
// Conversor de unidades
// calculadora de rampa
// calculadora de escada
// escalimetro
// nível
// norte (bússola sla)
// trena

//CONFIGURAÇÕES
// Setar/Ajustar Data
// Mudar cor do led

//GIROSCOPIO
// Acesso aos valores de qualquer lugar igual teclado

//NOTAS
// Melhorar Scroll (scrollar mquando quiser sem precisar estar digitando) [nem sei se tem tecla pra isso]
// Menu de notas salvas (ler do cartao SD) -> opção de criar nota. tamanho de titulo max 7 char por ai
// Salvar notas no Z

//MUSICA
// Keypad para mexer no volume
// Telinha de selecionar musica
// Quando selecionada, ficar tocando de fundo ate acabar independente do app aberto #always

//COMPRAR
// DAC audio e alto falante
// Microfone
// Leitor de mini-sd
// Led alto brilho RGB

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1','4','7','.'},
  {'2','5','8','0'},
  {'3','6','9','B'},
  {'H','M','Z','F'}
};

const char* t9Map[] = {
  "0 ",
  "1'()",
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

byte rowPins[ROWS] = {23, 19, 32, 33};
byte colPins[COLS] = {4, 18, 17, 16};
Keypad keypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

#define RST_PIN   26
#define CE_PIN    15
#define DC_PIN    27
#define DIN_PIN   13
#define CLK_PIN   14
Adafruit_PCD8544 display = Adafruit_PCD8544(CLK_PIN, DIN_PIN, DC_PIN, CE_PIN, RST_PIN);

Adafruit_MPU6050 mpu;

//# UTILS
typedef struct activity {
  const char* name;
  void (*pSetup)();
  void (*pLoop)();
  //unsigned char icon [];
};

#define TOTALAPPS 10
struct activity activities[TOTALAPPS + 1];

activity* curActivity;

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
char key = '\0';
int charIndex = 0;
char curChar = '\0';
String inputText = "";
String sentText = "";
int maxDigits = -1;
int keyboardMode = 0;

void readKeyBoard(){
  key = keypad.getKey();
  if(key){
    if(key == 'M'){
      Serial.println("ir pro menu");
      keyboardMode = 0;
      maxDigits = -1;
      curActivity = &activities[0];
      curActivity->pSetup();
    }
    else if(key == 'H'){
      Serial.println("ir pra home");
      keyboardMode = 0;
      maxDigits = -1;
      curActivity = &activities[1];
      curActivity->pSetup();
    }
    else if(key == 'B'){
      if(keyboardMode == 1){
        sentText = inputText;
        inputText = "";
        lastPressTime = currentTime;
      }
    }
    else if(key == 'F'){
      if(keyboardMode == 1){
        if(inputText.length() > 0){
          inputText.remove(inputText.length() - 1);
        }
      }
    }
    else{
      if(keyboardMode == 1){
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

// void setupMPU(){
//   while (!mpu.begin()) {
//     Serial.println("Failed to find MPU6050 chip");
//   }
//   Serial.println("MPU6050 Found!");
//   // mpu.begin();
//   mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
//   Serial.print("Accelerometer range set to: ");
//   switch (mpu.getAccelerometerRange()) {
//   case MPU6050_RANGE_2_G:
//     Serial.println("+-2G");
//     break;
//   case MPU6050_RANGE_4_G:
//     Serial.println("+-4G");
//     break;
//   case MPU6050_RANGE_8_G:
//     Serial.println("+-8G");
//     break;
//   case MPU6050_RANGE_16_G:
//     Serial.println("+-16G");
//     break;
//   }
//   mpu.setGyroRange(MPU6050_RANGE_500_DEG);
//   Serial.print("Gyro range set to: ");
//   switch (mpu.getGyroRange()) {
//   case MPU6050_RANGE_250_DEG:
//     Serial.println("+- 250 deg/s");
//     break;
//   case MPU6050_RANGE_500_DEG:
//     Serial.println("+- 500 deg/s");
//     break;
//   case MPU6050_RANGE_1000_DEG:
//     Serial.println("+- 1000 deg/s");
//     break;
//   case MPU6050_RANGE_2000_DEG:
//     Serial.println("+- 2000 deg/s");
//     break;
//   }

//   mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
//   Serial.print("Filter bandwidth set to: ");
//   switch (mpu.getFilterBandwidth()) {
//   case MPU6050_BAND_260_HZ:
//     Serial.println("260 Hz");
//     break;
//   case MPU6050_BAND_184_HZ:
//     Serial.println("184 Hz");
//     break;
//   case MPU6050_BAND_94_HZ:
//     Serial.println("94 Hz");
//     break;
//   case MPU6050_BAND_44_HZ:
//     Serial.println("44 Hz");
//     break;
//   case MPU6050_BAND_21_HZ:
//     Serial.println("21 Hz");
//     break;
//   case MPU6050_BAND_10_HZ:
//     Serial.println("10 Hz");
//     break;
//   case MPU6050_BAND_5_HZ:
//     Serial.println("5 Hz");
//     break;
//   }
// }

// void readMpu(){
//   sensors_event_t a, g, temp;
//   mpu.getEvent(&a, &g, &temp);

//   Serial.print("Acceleration X: ");
//   Serial.print(a.acceleration.x);
//   Serial.print(", Y: ");
//   Serial.print(a.acceleration.y);
//   Serial.print(", Z: ");
//   Serial.print(a.acceleration.z);
//   Serial.println(" m/s^2");

//   Serial.print("Rotation X: ");
//   Serial.print(g.gyro.x);
//   Serial.print(", Y: ");
//   Serial.print(g.gyro.y);
//   Serial.print(", Z: ");
//   Serial.print(g.gyro.z);
//   Serial.println(" rad/s");

//   Serial.print("Temperature: ");
//   Serial.print(temp.temperature);
//   Serial.println(" degC");

//   Serial.println("");
//   delay(500);
// }

//TERMO
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
  keyboardMode = 1;

  display.clearDisplay();
  //animação startup
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

//CARAOUCOROA
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
}

void flipCoin(){
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

void loopCoinFlip(){
  if(key == '5'){
    flipCoin();
  }
}

//NOTAS
typedef struct nota {
  const char* name;
  String text;
};

std::vector<nota> notas;

nota* openNota;


bool notasMenu = true;

void drawNota(){
  display.clearDisplay();
  //14 por linha
  //6 linhas
  // 6 * 14 = 84
  //5 por vez
  String txt = openNota->text + inputText;
  int offset = (txt.length() - 56);
  offset = offset < 0 ? 0 : ceil((offset + 0.5) / 14);

  display.setTextColor(BLACK);
  display.setCursor(0, 13 - (offset * 7));
  display.print(txt);

  display.fillRect(0, 0, 84, 9, BLACK);

  display.drawFastHLine(0, 10, 84, WHITE);
  display.drawFastHLine(0, 11, 84, BLACK);
  display.drawFastHLine(0, 47, 84, BLACK);

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(1, 1);
  display.print(openNota->name);

  
  display.display();
}

void setupNotas(){
  display.clearDisplay();
  notas.push_back({ "teste", "textoTestewasdfsadasdasdasdsavafadfwdawdadwdawdwdw" });

  int openNotaIndex = notas.size() - 1;
  openNota = &notas[openNotaIndex];
  notasMenu = false;
  keyboardMode = 1;
  maxDigits = -1;
  drawNota();
  //mostrar lista de notas salvas

}

void loopNotas(){
    if(notasMenu){
      Serial.println("menu");
    }else{
      if(key == 'F'){
        if(inputText.length() <= 0){
          if(openNota->text.length() > 0){
            openNota->text.remove(openNota->text.length() - 1);
          }
        }
      }
      if(key == 'B'){
        openNota->text += (sentText + "\n");
        sentText = "";
      }
      
      drawNota();
    }
}

//HOME
const unsigned char eyeMap [] PROGMEM = {
	0x3f, 0x80, 0x00, 0x7f, 0xf0, 0x00, 0xe0, 0xfe, 0x00, 0xc0, 0x1f, 0x00, 0xc0, 0x03, 0xc0, 0xc0,
	0x01, 0xc0, 0xc0, 0x00, 0x60, 0xc0, 0x00, 0x60, 0xc0, 0x00, 0x30, 0xc0, 0x00, 0x30, 0xc0, 0x00,
	0x18, 0xc0, 0x00, 0x18, 0xc0, 0x00, 0x18, 0xc0, 0x00, 0x38, 0xc0, 0x00, 0x78, 0xc0, 0x00, 0x78,
	0xe0, 0x00, 0x78, 0xe0, 0x00, 0xf0, 0x70, 0x00, 0xf0, 0x78, 0x03, 0xe0, 0x3f, 0xff, 0x80, 0x07,
	0xff, 0x80, 0x03, 0xff, 0x00
};
const unsigned char mirrorEyeMap [] PROGMEM = {
	0x00, 0x0f, 0xe0, 0x00, 0x7f, 0xf0, 0x03, 0xf8, 0x38, 0x07, 0xc0, 0x18, 0x1e, 0x00, 0x18, 0x1c,
	0x00, 0x18, 0x30, 0x00, 0x18, 0x30, 0x00, 0x18, 0x60, 0x00, 0x18, 0x60, 0x00, 0x18, 0xc0, 0x00,
	0x18, 0xc0, 0x00, 0x18, 0xc0, 0x00, 0x18, 0xe0, 0x00, 0x18, 0xf0, 0x00, 0x18, 0xf0, 0x00, 0x18,
	0xf0, 0x00, 0x38, 0x78, 0x00, 0x38, 0x78, 0x00, 0x70, 0x3e, 0x00, 0xf0, 0x0f, 0xff, 0xe0, 0x0f,
	0xff, 0x00, 0x07, 0xfe, 0x00
};
const unsigned char pupilMap [] PROGMEM = {
	0x3f, 0x80, 0x7f, 0xc0, 0x7e, 0x60, 0xfe, 0x30, 0xff, 0x30, 0xff, 0xf0, 0xff, 0xf0, 0x7f, 0xf0,
	0x7f, 0xf0, 0x3f, 0xc0, 0x07, 0x80
};
const unsigned char mirrorPupilMap [] PROGMEM = {
	0x1f, 0xc0, 0x3f, 0xe0, 0x67, 0xe0, 0xc7, 0xf0, 0xcf, 0xf0, 0xff, 0xf0, 0xff, 0xf0, 0xff, 0xe0,
	0xff, 0xe0, 0x3f, 0xc0, 0x1e, 0x00
};

struct Pupil {
  int x; //  1 -> 9 {+63}
  int y; // 5 -> 14
  //int rotation; //0 - 3 = 0, 90, 180, 270
};
struct Eye {
  int state;
  Pupil pupil;
  int x;
  int y;
};

void drawEye(Eye eye, bool mirror = false){
   if(eye.state != 0){
    Serial.println("olho fechado");
  }else{
    //olho
    display.drawBitmap(eye.x, eye.y, mirror ? eyeMap : mirrorEyeMap, 21, 23, BLACK);
    //pupila
    display.drawBitmap(eye.pupil.x, eye.pupil.y, mirror ? pupilMap : mirrorPupilMap, 12, 11, BLACK);
  }
}
void drawEyes(Eye leftEye = {0, {8, 8}, 0, 4}, Eye rightEye = {0, {71, 8}, 63, 4}){
  drawEye(leftEye);
  drawEye(rightEye, true);
}

void setupHome(){
  display.clearDisplay();
  Pupil leftPupil = {7, 9};
  Eye leftEye = {0, leftPupil, 0, 4};

  Pupil rightPupil = {65, 9};
  Eye rightEye = {0, rightPupil, 63, 4};

  drawEyes(rightEye, leftEye);
  display.display();
}

void loopHome(){

}


//MENU
int selectedApp = 0;
void setupMenu(){
  selectedApp = 0;
  drawMenu();  
}

int menuPagesize = TOTALAPPS > 6 ? 6 : TOTALAPPS;
int curPage = 1;
const int totalPages = ceil(TOTALAPPS / 6);

void drawMenu(){
  display.clearDisplay();
  int xOffset = 10;
  int yOffset = 3;
  int Xpos = 0;
  int Ypos = 0;

  for (int i = 0; i < menuPagesize; i++){
    Xpos = ((i % 3)*23) + xOffset;
    Ypos = (((i >= 3? 1 : 0))*24) + yOffset;
    display.drawRect(Xpos, Ypos, 20, 20, BLACK);
    if(selectedApp == i){
      display.drawRect(Xpos-1, Ypos-1, 22, 22, BLACK);
    }
  }

  if(curPage > 1){
    display.drawRect(0, 20, 10, 10, BLACK);
    if(selectedApp == -1){
      display.fillRect(0, 20, 10, 10, BLACK);
    }
  }

  if(curPage < totalPages){
    display.drawRect(74, 20, 10, 10, BLACK);
    if(selectedApp == -2){
      display.fillRect(74, 20, 10, 10, BLACK);
    }
  }

  display.display();
}

void loopMenu(){
  if(key){
    int oldApp = selectedApp;

    switch(key){
      case '2':
        if(selectedApp >= 3) selectedApp = selectedApp - 3;
        break;

      case '8':
        if(selectedApp < 3 && (selectedApp + 3 <= menuPagesize)) selectedApp = selectedApp + 3;
        break;

      case '4':
        if(selectedApp != 0 && selectedApp != 3) { selectedApp = selectedApp - 1; break;}

        else if((selectedApp == 0 || selectedApp == 3) && curPage > 1){selectedApp = -1; break;}

        else if(selectedApp == -2){ selectedApp = 5; break; }

        break;

      case '6':
        if(selectedApp != 2 && selectedApp != (menuPagesize-1)){ selectedApp = selectedApp + 1; break;}

        else if((selectedApp == 2 || selectedApp == 5) && curPage < totalPages){ selectedApp = -2; break; }

        else if(selectedApp == -1){ selectedApp = 0; break; }
        

        break;

      case '5':
        if(selectedApp == -1){
          curPage = curPage - 1;
          menuPagesize = 6;
          selectedApp = 5;
          break;

        }else if(selectedApp == -2){
          menuPagesize = TOTALAPPS - (menuPagesize*curPage);
          curPage = curPage + 1;
          selectedApp = 0;
          break;
        }

        else{
          int index = (selectedApp+1) + ((curPage-1) * menuPagesize);
          Serial.println(activities[index].name);

          keyboardMode = 0;
          maxDigits = -1;
          curActivity = &activities[index];
          curActivity->pSetup();

          return;
        }
        break;
    }
  
    if(selectedApp != oldApp){
      drawMenu();
    }
  }
}

void setup()   {
  Serial.begin(115200);

  activities[0] = { "MENU",  setupMenu, loopMenu };
  activities[1] = { "HOME", setupHome, loopHome };
  activities[2] = { "TERMO", setupTermo, loopTermo };
  activities[3] = { "CARA OU COROA", setupCoinFlip, loopCoinFlip };
  activities[4] = { "NOTAS", setupNotas, loopNotas };


  display.begin();

  display.setContrast(90);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.display();

  curActivity = &activities[0];
  curActivity->pSetup();
}

void loop(){
  currentTime = millis();
  readKeyBoard();
  curActivity->pLoop();
}
