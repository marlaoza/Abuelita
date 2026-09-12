#include "Activities/Termo.h"
#include "Assets.h"

String wordBank[] PROGMEM = {
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

Termo::Termo() 
    : Activity(ico_termo) {}

void Termo::setup() {
    Page gamePage;
    gamePage.draw = [this]() {
        this->displayManager.screen.clearDisplay();
        for (int row = 0; row < 5; row++){
            for (int col = 0; col<5; col++){
            int Xpos = (col*11) + xOffset;
            int Ypos = (row*9) + yOffset;
            this->displayManager.screen.drawRect(Xpos, Ypos, 9, 8, BLACK);
            }
        }
        this->displayManager.screen.display();
    };

    gamePage.loop = [this]() {
        String sentText = inputManager.readSentText();
        if(sentText != ""){
            drawRow(sentText);
            curRow++;
        }
        else{
            String input = inputManager.readRawText();
            int Ypos = (curRow*9) + yOffset;
            for (int col = 0; col < 5; col++){
                int Xpos = (col*11) + xOffset;
                this->displayManager.screen.fillRect(Xpos+1, Ypos+1, 7, 6, WHITE);
                this->displayManager.screen.setCursor(Xpos+2, Ypos);
                this->displayManager.screen.print(input[col]);
            }
            this->displayManager.screen.display();
        }

        if(curRow == 5 || won){
            setPage("end");
        }
    };

    registerPage("game", gamePage);

    Page endPage;
    endPage.draw = [this]() {
        this->displayManager.screen.setTextColor(BLACK);
        this->displayManager.screen.clearDisplay();
        this->displayManager.screen.drawBitmap(0, 0, termo_endscreen, 84, 48, BLACK);
        this->displayManager.screen.setCursor(26, 10);
        this->displayManager.screen.print(currentWord);

        if(won){
            this->displayManager.screen.setCursor(13,28);
            this->displayManager.screen.print(String(curRow));
        }else{
            this->displayManager.screen.fillRect(11,20, 62, 16, BLACK);
        }
        this->displayManager.screen.display();
        
    };
    endPage.loop = [this]() {
        char key = inputManager.readKey();
        if(key){startGame();}
    };
  
    registerPage("end", endPage);

    startGame();
}

void Termo::loop() {
    if(currentPage)currentPage->loop();
}

void Termo::stop() {
    displayManager.screen.clearDisplay();
    inputManager.resetKeyPadVariables();
    pages.clear();
}

void Termo::startGame() {
    inputManager.resetKeyPadVariables();
    curRow = 0;
    this->currentWord = wordBank[random(0, sizeof(wordBank) / sizeof(wordBank[0]) - 1 )];
    Serial.println(this->currentWord);
    inputManager.maxDigits = 5;
    inputManager.minDigits = 5;
    won = false;
    setPage("game");
}


void Termo::drawRow(String word){
  String targetWordCopy = currentWord;
  int amtRight = 0;
  int Ypos = (curRow*9) + yOffset;

  for (int col = 0; col < 5; col++){
    int Xpos = (col*11) + xOffset;
    if(word[col] != ' '){
      bool allRight = (word[col] == targetWordCopy[col]);
      if(allRight){
        this->displayManager.screen.fillRect(Xpos, Ypos, 9, 8, BLACK);
        this->displayManager.screen.setTextColor(WHITE);
        this->displayManager.screen.setCursor(Xpos+2, Ypos);
        this->displayManager.screen.print(word[col]);
        word[col] = ' ';
        targetWordCopy[col] = ' ';
        amtRight++;
      }
    }
  }

  targetWordCopy = currentWord;
  for (int col = 0; col < 5; col ++){
    if(word[col] != ' '){
      int Xpos = (col*11) + xOffset;
      int index = targetWordCopy.indexOf(word[col]);
      bool letterRight = index != -1;
      if(letterRight){
          targetWordCopy[index] = ' ';
          this->displayManager.screen.drawRect(Xpos, Ypos, 9, 8, BLACK);
      }
      else{
        this->displayManager.screen.fillRect(Xpos, Ypos, 9, 8, WHITE);
      }
      this->displayManager.screen.setTextColor(BLACK);
      this->displayManager.screen.setCursor(Xpos+2, Ypos);
      this->displayManager.screen.print(word[col]);
    }
  }

  this->displayManager.screen.display();
  if(amtRight == 5){won = true;}
}