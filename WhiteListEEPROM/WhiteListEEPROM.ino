#include <EEPROM.h>
 int sizeEEPROM = 4095;
 int cardLimit = 200; //cardLimit*10= EEPROM consumed in Bytes

 int posFirstOcc = 4010 ;
 int posLastOcc = 4011;
 int posFirstFree = 4012;
 int posQtdeCards = 4013;


/*
 * Card structure in EEPROM - size: 10B
 */
 
typedef struct{
  uint32_t card;
  uint8_t device_1;
  uint8_t device_2;
  uint8_t device_3;
  uint8_t device_4;
  uint8_t timeOpen;
  uint8_t next;
}cardType;

void insertCardIdEEPROM(int endereco, uint32_t value){
  for (int i = 3; i >= 0; i--) {
    uint8_t byteGrava = value>>(i*8);
    EEPROM.write(3-i+endereco, byteGrava);
    //Serial.print(byteGrava, BIN);
  }
}
 uint32_t readCardIdEEPROM(int endereco){
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

void WriteCard(cardType data, int address){
   // put your setup code here, to run once:
 
  insertCardIdEEPROM(address*10, data.card);
  EEPROM.write(address*10+4, data.device_1);
  EEPROM.write(address*10+5, data.device_2);
  EEPROM.write(address*10+6, data.device_3);
  EEPROM.write(address*10+7, data.device_4);
  EEPROM.write(address*10+8, data.timeOpen);
  
}

 cardType ReadCard(int address){
  cardType data;
  data.card = readCardIdEEPROM(address*10);
  data.device_1 = EEPROM.read(address*10+4);
  data.device_2 = EEPROM.read(address*10+5);
  data.device_3 = EEPROM.read(address*10+6);
  data.device_4 = EEPROM.read(address*10+7);
  data.timeOpen = EEPROM.read(address*10+8);
  data.next = EEPROM.read(address*10+9);
  return data;
}


/*
 *  Getters and Setters
 */
 
 int getNextPosition(int address){
  return EEPROM.read(address * 10 + 9);
}

 void setNextPosition(int address, int newNextPosition){
  EEPROM.write(address * 10 + 9, newNextPosition);
}

 int getValue(int pos){
  return EEPROM.read(pos);
}

/*
 *  EEPROM
 */

 void resetEEPROM(){
  EEPROM.write(posFirstOcc, -1);
  EEPROM.write(posFirstFree, 0);
  EEPROM.write(posLastOcc, -1);
  EEPROM.write(posQtdeCards, 0);
  for(int i = 0; i < cardLimit; i++){
    EEPROM.write(i*10+9, i+1);
  }
  //EEPROM.commit();
}


boolean addCard(cardType data){
  if(getValue(posFirstFree) < cardLimit){
    Serial.println(getValue(posFirstFree));
    WriteCard(data, getValue(posFirstFree));
    cardType data = ReadCard(getValue(posFirstFree));
    if(getValue(posQtdeCards) == 0) EEPROM.write(posFirstOcc,0);
    EEPROM.write(posLastOcc, getValue(posFirstFree));
    EEPROM.write(posFirstFree, data.next);
    

    EEPROM.write(posQtdeCards, getValue(posQtdeCards)+1);
    return true;
  }else{
    Serial.println("Cartão não adicionado");
    return false;
  }
}

void listCards(){
  Serial.println("Lista de cartões na EEPROM");
  int next;
  for(int i = 0; i < getValue(posQtdeCards); i++){
    cardType card;
    if(i == 0){
      card = ReadCard(getValue(posFirstOcc));
      
    }else{
      card = ReadCard(next);
    }
    next = card.next;
    Serial.println("-------------------");
    Serial.print("Card: ");
    Serial.println(card.card, HEX);
    Serial.print("Position Next: ");
    Serial.println(card.next);
  }
}

/*cardType findCard(int id_card){
  int next;
  cardType data;
  for(int i = 0; i < getValue(posQtdeCards); i++){
    if(i == 0){
      data = ReadCard(getValue(posFirstOcc));
    }else{
      data = ReadCard(next);
    }
    next = data.next;
    if(data.card == id_card){
       return data;
    }
  }
  cardType t;
  return t;
}*/


cardType createCard(uint32_t card){
  cardType data;
  data.card = card;
  return data;
}
void setup() {
  //resetEEPROM();
  Serial.begin(115200);
  addCard(createCard(0x12345678));
  listCards();
  //Serial.println(findCard(0xdfdfd));
}

void loop() {
  // put your main code here, to run repeatedly:

}


