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
  EEPROM.write(posFirstOcc, 200);
  EEPROM.write(posFirstFree, 0);
  EEPROM.write(posLastOcc, 200);
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

void listCardsInOrder(){
  Serial.print("Primeiro Livre: ");
  Serial.println(getValue(posFirstFree));
  Serial.print("Primeiro Ocupado: ");
  Serial.println(getValue(posFirstOcc));
  Serial.print("Ultimo Ocupado: ");
  Serial.println(getValue(posLastOcc));
  Serial.print("Qtde Cartoes: ");
  Serial.println(getValue(posQtdeCards));
  Serial.println("Lista de cartoes na EEPROM");
  listCards();
  Serial.println("");
  
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
    Serial.println(card.card);
    Serial.print("Position Next: ");
    Serial.println(card.next);
  }
}

void listCards(){
  for(int i = 0; i < getValue(posQtdeCards); i++){
    cardType card = ReadCard(i);
    Serial.print("|");
    Serial.print(card.card);
    Serial.print("|");
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

/*boolean deleteCard(uint32_t id_card){
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
}*/


boolean deleteCard(uint32_t id_card){
  cardType inPosition;
  int thisPos = getValue(posFirstOcc);
  int antPos = 0;
  boolean ret = false;
  for(int i = 0; i < getValue(posQtdeCards); i++){
    
    if(ret) break;
    
    inPosition = ReadCard(thisPos);

    if(inPosition.card == id_card){
      
      //del first element
      if(i == 0){
        int temp = getNextValue(getValue(posLastOcc));
        int temp2 = inPosition.next;

        setNextValue(getValue(posLastOcc), getValue(posFirstOcc));
        setValue(posFirstFree, getValue(posFirstOcc));
        setNextValue(thisPos, temp);
        setValue(posFirstOcc, temp2);  
      } 
      
      //del last element
      else if(i == getValue(posQtdeCards)-1) {
        setValue(posFirstFree, getValue(posLastOcc));
        setValue(posLastOcc, antPos);
      }
      
      //del another elements
      else{
        int temp = getNextValue(thisPos);
        int temp2 = getValue(posFirstFree);
        setNextValue(antPos, temp);
        setNextValue(thisPos, temp2);
        setNextValue(getValue(posLastOcc), thisPos);
        setValue(posFirstFree, thisPos);
      }
      
      //default operation
      setValue(posQtdeCards, getValue(posQtdeCards)-1);
      ret = true;
      
    }// if(inPosition.card == id_card)
    
    antPos = thisPos;
    thisPos = inPosition.next;
    
  }//for
  return ret;
}


cardType createCard(uint32_t card){
  cardType data;
  data.card = card;
  return data;
}




String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete



void setup() {
 
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void serialEvent() {

  while (Serial.available()) {
    // get the new byte:
    char inChar =(char) Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
            Serial.println(inputString);

     // stringComplete = true;
    //}
   // if(stringComplete == true){
      String command = inputString.substring(0,3);
      uint32_t card = inputString.substring(4).toInt();
      inputString = "";
      if(command == "add"){
          addCard(createCard(card));
          Serial.println("Cartao adicionado");
      }else if(command == "lst"){
          listCardsInOrder();
      }else if(command == "clr"){
          resetEEPROM();
           Serial.println("EEPROM resetada");
      }else if(command == "rmv"){
           if(deleteCard(card)==true){ 
              Serial.println("Cartao deletado");
           } else {
             Serial.println("Cartao nao encontrado");
           }
      }else if(command == "fnd"){
           Serial.println(findCard(card).card);
      }
    }
  }
}


