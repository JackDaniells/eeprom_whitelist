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
  /*EEPROM.write(address*10+4, data.device_1);
  EEPROM.write(address*10+5, data.device_2);
  EEPROM.write(address*10+6, data.device_3);
  EEPROM.write(address*10+7, data.device_4);
  EEPROM.write(address*10+8, data.timeOpen);*/
  
}

 cardType ReadCard(int address){
  cardType data;
  data.card = readCardIdEEPROM(address*10);
  /*data.device_1 = EEPROM.read(address*10+4);
  data.device_2 = EEPROM.read(address*10+5);
  data.device_3 = EEPROM.read(address*10+6);
  data.device_4 = EEPROM.read(address*10+7);
  data.timeOpen = EEPROM.read(address*10+8);*/
  data.next = EEPROM.read(address*10+9);
  return data;
}


/*
 *  Getters and Setters
 */
 
 int getNextValue(int address){
  return EEPROM.read(address * 10 + 9);
}

 void setNextValue(int address, int newNextPosition){
  EEPROM.write(address * 10 + 9, newNextPosition);
}

 int getValue(int pos){
  return EEPROM.read(pos);
}

void setValue(int pos, int value){
  EEPROM.write(pos, value);
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
  Serial.print("Primeiro Livre: ");
  Serial.println(getValue(posFirstFree));
  Serial.print("Primeiro Ocupado: ");
  Serial.println(getValue(posFirstOcc));
  Serial.print("Ultimo Ocupado: ");
  Serial.println(getValue(posLastOcc));
  Serial.print("Qtde Cartoes: ");
  Serial.println(getValue(posQtdeCards));
  Serial.println("Lista de cartoes na EEPROM");

  
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

cardType findCard(int id_card){
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
  t.card = 0;
  return t;
}

boolean deleteCard(uint32_t id_card){
  cardType pos;
  cardType lastPos;
  int next;
  int last;
  boolean ret = false;
  for(int i = 0; i < getValue(posQtdeCards); i++){
    if(i == 0){
      pos = ReadCard(getValue(posFirstOcc));
      
      if(pos.card == id_card){
        Serial.println("Igual");
        int temp = getValue(posFirstFree);
        setValue(posFirstFree, getValue(posFirstOcc));
        setNextValue(getValue(posFirstOcc), temp);
        setNextValue(getValue(posLastOcc), getValue(posFirstOcc));
        setNextValue(posFirstOcc, temp);
        setValue(posFirstOcc, pos.next);
        setValue(posQtdeCards, getValue(posQtdeCards)-1);
        ret = true;   
      }
      
    }else{
      lastPos = pos;
      pos = ReadCard(next);
            
      if(pos.card == id_card){
        Serial.println("Igual");
        int temp = lastPos.next;
        int temp2 = getValue(posFirstFree);
        setNextValue(last, pos.next);
        setNextValue(getValue(posLastOcc), temp);
        setValue(posFirstFree, temp);
        setNextValue(next, temp2);
        setValue(posQtdeCards, getValue(posQtdeCards)-1);
        ret = true;   
      }
      
    }
    last = next;
    next = pos.next;
  }
  return ret;
}


cardType createCard(uint32_t card){
  cardType data;
  data.card = card;
  return data;
}
void setup() {
 
  Serial.begin(115200);
  /*resetEEPROM();
  addCard(createCard(0xabcdef12));
  addCard(createCard(0xabcdef23));
  addCard(createCard(0xabcdef34));
  addCard(createCard(0xabcdef45));*/
  addCard(createCard(0xabcdef90));
  //Serial.println(deleteCard(0xabcdef12));
  listCards();
  //Serial.println(findCard(0xdfdfd));
}

void loop() {
  // put your main code here, to run repeatedly:

}


