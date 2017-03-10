#include <EEPROM.h>
long sizeEEPROM = 4095;

typedef struct{
  uint32_t card;
  uint8_t device_1;
  uint8_t device_2;
  uint8_t device_3;
  uint8_t device_4;
  uint8_t timeOpen;
  uint8_t timeClose;
}white_list_data;


void insertCard(int endereco, uint32_t value){
  for (int i = 3; i >= 0; i--) {
    uint8_t byteGrava = value>>(i*8);
    EEPROM.write(3-i+endereco, byteGrava);
    //Serial.print(byteGrava, BIN);
  }
}
uint32_t readCard(int endereco){
  uint32_t retorno =0;
  for (int i = 0; i < 4; i++){  
    int shift = (3-i)*8;
    //Serial.println(shift);
    uint32_t temp = EEPROM.read(i+endereco);
    retorno += temp<<shift;
    //Serial.println(retorno, BIN);
  }
  return retorno;
}

void grava_dado_lista_branca(white_list_data dado, int endereco){
   // put your setup code here, to run once:
 
  insertCard(endereco*10, dado.card);
  EEPROM.write(endereco*10+4, dado.device_1);
  EEPROM.write(endereco*10+5, dado.device_2);
  EEPROM.write(endereco*10+6, dado.device_3);
  EEPROM.write(endereco*10+7, dado.device_4);
  EEPROM.write(endereco*10+8, dado.timeOpen);
  EEPROM.write(endereco*10+9, dado.timeClose);
  
}

white_list_data le_dado_lista_branca(int endereco){
  white_list_data dado;
  dado.card = readCard(endereco*10);
  dado.device_1 = EEPROM.read(endereco*10+4);
  dado.device_2 = EEPROM.read(endereco*10+5);
  dado.device_3 = EEPROM.read(endereco*10+6);
  dado.device_4 = EEPROM.read(endereco*10+7);
  dado.timeOpen = EEPROM.read(endereco*10+8);
  dado.timeClose = EEPROM.read(endereco*10+9);
  return dado;
}


void setup() {
  Serial.begin(115200);


white_list_data dado;
dado.card = 0xABCDEF12;
dado.device_4 = 0x7;
grava_dado_lista_branca(dado, 120);
white_list_data dadoRet = le_dado_lista_branca(120);
Serial.println(dadoRet.card, HEX);
Serial.println(dadoRet.device_4, HEX);

}

void loop() {
  // put your main code here, to run repeatedly:

}


