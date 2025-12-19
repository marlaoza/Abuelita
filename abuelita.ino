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

//BARTENDER
//seleciona ingredientes
//chacoalha ate barra enxer
//tecla pra servir -> drink aparece do lado do cliente
// timerzinho . . .
// reação do cliente
// reação some depois de qualquer tecla, pode fazer novo drink

//VELA
// Mover isqueiro com 2^ 4< 6> 8V ou com giroscopio
// Chegar perto da vela apertar 5 para acender (ligar led junto)
// Salvar vela salva com um titulo
// Ver velas salvas (velas acesas ligar o led)

//ARQUI-TOOLS
// calculadora de rampa
//  adicionar info embaixo (rampa invalida etc)

// calculadora de escada

// escalimetro

// nível

// norte (bússola sla)


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

bool won = false;
bool playingTermo;

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
    won = true;
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

void drawEndScreen(){
  display.clearDisplay();
  display.print("A palavra era: ");
  display.print(targetWord);

  if(won){
    display.print("Você acertou em " + String(curRow) + "tentativas");
  }
  display.print("Aperte qualquer tecla para reiniciar");
  display.display();
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

    if(curRow == 5 || won){
      playingTermo = false;
      drawEndScreen();
    }


  }else{
    if(key){
      //reiniciar
      setupTermo();
    }
  }
}

//VELA
void setupVela(){

}
void loopVela(){
  
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

int selectedNota = 0;
void drawNotasMenu(){
  
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

//BAR
const unsigned char bar_barman_idle [] PROGMEM = {
	0x01, 0xfc, 0x00, 0x03, 0x06, 0x00, 0x07, 0x01, 0x00, 0x06, 0x01, 0x00, 0x3f, 0xdf, 0xe0, 0x1f, 
	0xdd, 0xc0, 0x1d, 0xcd, 0xc0, 0x0c, 0x41, 0x80, 0x04, 0xf1, 0x00, 0x03, 0x82, 0x00, 0x03, 0xf2, 
	0x00, 0x01, 0xfc, 0x00, 0x0f, 0xff, 0x80, 0x3f, 0x27, 0xe0, 0x7e, 0x03, 0xf0, 0x7f, 0x27, 0xf0, 
	0x7f, 0xff, 0xf0, 0xff, 0xff, 0xf8, 0xff, 0xdf, 0xf8, 0xff, 0xff, 0xf8, 0xff, 0xff, 0xf8, 0xff, 
	0xdf, 0xf8, 0xff, 0xff, 0xf8, 0xff, 0xff, 0xf8, 0xff, 0xdf, 0xf8, 0xff, 0xff, 0xf8, 0xff, 0xff, 
	0xf8, 0xff, 0xdf, 0xf8, 0xff, 0xff, 0xf8
};
const unsigned char bar_barman_shake1 [] PROGMEM = {
	0x1f, 0xf0, 0x00, 0x30, 0x10, 0x00, 0x60, 0x08, 0x00, 0x40, 0x0f, 0x00, 0x7b, 0xbe, 0x00, 0x48, 
	0x3e, 0x00, 0x7b, 0xbc, 0x00, 0x48, 0x38, 0x00, 0x4e, 0x78, 0x00, 0x40, 0xf3, 0xc0, 0x2f, 0xf6, 
	0x60, 0x30, 0xf4, 0x38, 0x7f, 0xfe, 0xf8, 0x7f, 0xff, 0x88, 0xfe, 0x4e, 0x08, 0xfc, 0x0c, 0x08, 
	0xee, 0x48, 0x18, 0xef, 0xf0, 0x70, 0xef, 0xf9, 0xe0, 0xff, 0xfe, 0xc0, 0xf7, 0xfc, 0x40, 0xfb, 
	0xfc, 0x40, 0xfc, 0xff, 0xc0, 0xff, 0x7f, 0x80, 0xff, 0x9f, 0x00, 0xff, 0xe6, 0x00, 0xff, 0xf8, 
	0x00, 0xff, 0xf8, 0x00, 0xff, 0xf8, 0x00
};
const unsigned char bar_barman_shake2 [] PROGMEM = {
	0x3f, 0xe0, 0x00, 0x60, 0x20, 0x00, 0xc0, 0x10, 0x00, 0x80, 0x1e, 0x00, 0xf7, 0x7c, 0x00, 0x90, 
	0x7c, 0x00, 0xf7, 0x78, 0x00, 0x90, 0x70, 0x00, 0x9c, 0xf0, 0x00, 0x81, 0xe0, 0x00, 0x5f, 0xe0, 
	0x00, 0x30, 0xf3, 0xc0, 0x7f, 0xf6, 0x60, 0x7f, 0xf4, 0x38, 0xfe, 0x4e, 0xf8, 0xfc, 0x0f, 0x88, 
	0xee, 0x4e, 0x08, 0xef, 0xfc, 0x08, 0xef, 0xf8, 0x18, 0xff, 0xf0, 0x70, 0xf7, 0xf9, 0xe0, 0xfb, 
	0xfe, 0xc0, 0xfc, 0xfc, 0x40, 0xff, 0x7c, 0x40, 0xff, 0x9f, 0xc0, 0xff, 0xef, 0x80, 0xff, 0xff, 
	0x00, 0xff, 0xfe, 0x00, 0xff, 0xf8, 0x00
};

const unsigned char* barmanShakeAnim[2] = {
	bar_barman_shake1, bar_barman_shake2
};

const unsigned char bar_bg [] PROGMEM = {
	0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 
	0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 
	0x20, 0x00, 0x38, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x22, 0x20, 0x38, 0x18, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x72, 0x23, 0xb0, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x06, 0x52, 0x73, 0xb8, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x77, 0x73, 0x3b, 0x98, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x77, 0x53, 0xbb, 0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x07, 0x75, 0x73, 0xbb, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 
	0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x06, 0x00, 0x00, 
	0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x30, 0x06, 0x00, 0x03, 0x81, 0x18, 0x00, 0x00, 0x00, 
	0x00, 0x18, 0x18, 0x06, 0x43, 0xe1, 0x82, 0x98, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x0f, 0x06, 0x77, 
	0xf2, 0xa9, 0x18, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0e, 0x06, 0x74, 0x11, 0xa9, 0x18, 0x00, 0x00, 
	0x00, 0x00, 0x30, 0x0c, 0x07, 0x77, 0xf3, 0xbb, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x38, 0x18, 0x07, 
	0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x38, 0x07, 0x00, 0x00, 0x00, 0x38, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0xf0, 0x06, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 
	0x06, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf8, 0x06, 0x2e, 0x00, 0x1e, 0x18, 
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x06, 0x51, 0x00, 0xf3, 0x98, 0x00, 0x00, 0x00, 0x00, 0x7f, 
	0xfe, 0x06, 0x80, 0x80, 0xaa, 0x98, 0x00, 0x00, 0x00, 0x00, 0x40, 0xfe, 0x06, 0xa1, 0xc3, 0xff, 
	0xd8, 0x00, 0x00, 0x00, 0x00, 0x40, 0x3e, 0x06, 0xaf, 0x81, 0xff, 0x98, 0x00, 0x00, 0x00, 0x00, 
	0x40, 0x3e, 0x06, 0x6f, 0x00, 0xff, 0x18, 0x00, 0x00, 0x00, 0x00, 0x40, 0x3e, 0x06, 0x3f, 0x00, 
	0x7e, 0x18, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 
	0x07, 0xc0, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x01, 0xc0, 0x3e, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xf0, 0x01, 0xc0, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x3e, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0xc3, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xdf, 0xfe, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc7, 
	0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc1, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x01, 0xc1, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0xe1, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xf0, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0
};
const unsigned char bar_glass [] PROGMEM = {
	0x10, 0x20, 0xf8, 0xf8, 0x70, 0x20, 0x20, 0x70
};

typedef struct {
  int pos;
  int type;
  int value;
} item;

typedef struct props{
  int type;
  int value;
};
typedef struct drink{
  props propList[3];
};

drink curDrink;

int row = 0;
int curItem = 0;

item rowItens[][6] = {
  {{33, 0, 3}, {37, 0, 1}, {41, 0, 2}, {46, 1, 2}, {50, 1, 3}, {54, 1, 1}},
  {{33, 2, 1}, {39, 3, 1}, {46, 4, 1}, {50, 5, 1}, {54, 6, 1}},
  {{35, 7, 1}, {51, 8, 1}}
};
int rowY[3] = {
  1, 12, 21
};
int rowl[3] = {
  6, 5, 2
};

int barmanPos[2] = {63,2};

int shakePercent = 0;

int mode = 0;

void setupBar(){
  display.clearDisplay();
  display.drawBitmap(0, 0, bar_bg, 84, 48, BLACK);
  drawBarman();
  drawCursor();
  display.display();

}

void drawCursor(){
  display.fillRect(32, rowY[row], 26, 2, WHITE);
  if(mode == 0){
    display.drawFastHLine(rowItens[row][curItem].pos, rowY[row], 3, BLACK);
    display.drawPixel(rowItens[row][curItem].pos + 1, rowY[row] + 1, BLACK);
  }

}

bool animIndex = false;

long animTime = 0;
long animTarget = 300;

void drawBarman(){
  display.fillRect(barmanPos[0], barmanPos[1], 21, 29, WHITE);
  display.drawBitmap(barmanPos[0], barmanPos[1], (mode == 0 || mode == 2) ? bar_barman_idle : barmanShakeAnim[(int)animIndex], 21, 29, BLACK);
}

void loopBar(){
  if(mode == 0){
    if(key){
      if(key == '6'){
        if(curItem < rowl[row]){
          curItem +=1;
          drawCursor();
          display.display();
        }
      }
      else if(key == '4'){
        if(curItem > 0){
          curItem -=1;
          drawCursor();
          display.display();
        }
      }
      else if(key == '5'){
        curDrink.propList[row].type = rowItens[row][curItem].type;
        curDrink.propList[row].value = rowItens[row][curItem].value;
        if(row < 2){
          row +=1;
          curItem = 0;
        }else{
          mode = 1;
          row = 0;
          curItem = 0;
          animTime = currentTime;
        }
        drawCursor();
        display.display();
      }
    }
  }else if(mode == 1){
    if(currentTime - animTime >= animTarget){
      animIndex = !animIndex;
      animTime = currentTime;
    }
    drawBarman();
    display.display();
    if(shakePercent >= 100){
      mode = 2;
    }
  }else if(mode == 2){
    //desenhar taça
    //animação cliente
  }
}

//Arquitetura
//Rampas

const unsigned char rampa [] PROGMEM = {
	0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x7c, 
	0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x01, 0xfc, 0x00, 0x00, 0x03, 0xfc, 0x00, 0x00, 0x07, 0xfc, 
	0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x1f, 0xd4, 0x00, 0x00, 0x3f, 0xc4, 0x00, 0x00, 0x7f, 0xd4, 
	0x00, 0x00, 0xff, 0xfc, 0x00, 0x01, 0xff, 0xfc, 0x00, 0x03, 0xff, 0xfc, 0x00, 0x07, 0xff, 0xfc, 
	0x00, 0x0f, 0xff, 0xfc, 0x00, 0x1f, 0xff, 0xfc, 0x00, 0x3f, 0xff, 0xfc, 0x00, 0x7f, 0xff, 0xfc, 
	0x00, 0xff, 0xff, 0xfc, 0x01, 0xff, 0xff, 0xfc, 0x03, 0xff, 0xff, 0xfc, 0x07, 0xff, 0xff, 0xfc, 
	0x0e, 0x3f, 0xff, 0xfc, 0x1f, 0x7f, 0xfc, 0x7c, 0x2e, 0x3f, 0xfd, 0xfc, 0x47, 0xff, 0xfc, 0x7c, 
	0xff, 0xff, 0xff, 0xfc
};

int toCalc = 2;
int selectedItem = 0;
int inputMode = 0;

String values[3];
char symbols[3] = {
  'H','C','I'
};

void setupRampa(){
  display.clearDisplay();
  drawControls();
}

void drawControls(){
  display.clearDisplay();
  for (int i = 0; i < 3 ; i ++){
    bool selected = toCalc == i;
    int y = i*11;
    if(selected){
      display.fillRect(0, y, 9, 9, BLACK);
      display.fillRect(11, y, 38, 9, BLACK);

      display.setTextColor(WHITE);
      
    }else{
      display.drawRect(0, y, 9, 9, BLACK);
      display.drawRect(11, y, 38, 9, BLACK);

      display.setTextColor(BLACK);
    }
  
    display.setCursor(2, y+1);
    display.print(symbols[i]);

    display.setCursor(13, y+1);

    if(selected){
      float result = 0;
      int c = values[0].toInt();
      int h = values[1].toInt();
      int ir = values[2].toInt();

      if(i == 0 && ir > 0){
        result = 100.0f * h / (float)ir;
      }else if(i == 1){
        result = (float)(ir*c) / 100.0f;
      }else if(i == 2 && c !=0){
        result = 100.0f * h / c;
      }
      values[i] = String(result);

      display.print(String(result, 2));
    }
    else{
      display.print(values[i]);
    }

    if(selectedItem == i){
      display.drawPixel(51, y+1, BLACK);
      display.drawFastVLine(50, y+2, 4, BLACK);
      display.drawPixel(51, y+6, BLACK);
    }

    display.drawFastVLine(53, 0, 32, BLACK);
    display.drawFastHLine(0, 32, 84, BLACK);

    display.drawBitmap(54, 2, rampa, 30, 29, BLACK);
  }
  display.display();

}

void loopRampa(){
  if(key){
    if(key == 'Z' && selectedItem <= 2){
      toCalc = selectedItem;
    }
    if(key == 'B' && selectedItem != toCalc){
      inputMode = inputMode == 1 ? 0 : 1;
    }
    if(inputMode == 1 && selectedItem != toCalc && key != 'B'){
      if(key == 'F'){
        if(values[selectedItem].length() > 0){
        values[selectedItem].remove(values[selectedItem].length() - 1);
      }
      }else{
        values[selectedItem] += key;
      }
    }else if(inputMode == 0){
      if(key == '2'){
        selectedItem = selectedItem > 0? selectedItem - 1 : 0;
      }
      else if(key == '8'){
        selectedItem = selectedItem < 2? selectedItem + 1 : 2;
      }
    }

    drawControls();
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
  activities[5] = { "RAMPA", setupRampa, loopRampa };
  activities[6] = { "BAR", setupBar, loopBar };

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
