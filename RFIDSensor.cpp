#include "SensorClass.h"
#include "SPI.h"
#include "MFRC522.h"
#include "HEX2INT.h"
int SS_PIN = 7;
int RST_PIN = 8;
MFRC522 rfid(SS_PIN,RST_PIN);
bool initbitRFID = false;
String UpdateRFID(Sensor &self)
{
  String code_carte = "";
  uint32_t code;
  String State;
  while(!initbitRFID)
  {
    self.SetDatasetsName(0,"RFID");
    self.SetDatasetsUnits(0,"n° de Carte");
    SPI.begin();
    rfid.PCD_Init();
    initbitRFID=true;
  }
  if (rfid.PICC_IsNewCardPresent())
  {
    if (rfid.PICC_ReadCardSerial()) 
    { // Lecture d’un code RFID via le bus SPI
      for (int i = 0; i < rfid.uid.size; i++) 
      {
      // Ajoute chaque octet hexadécimal à la chaîne de caractères "code_carte"
        code_carte += (rfid.uid.uidByte[i] < 0x10 ? " 0" : "");
        code_carte += String(rfid.uid.uidByte[i], HEX);
        code=HEXTOINT(code_carte.c_str());
        self.SetDatasetsValue(0,code);
        State = "RFID CARD DETECTED";
      }
    }
  }
  else
  {
    State= "NO RFID CARD DETECTED";
  }
  return State;
}