#include <WiFi.h>            // Com WiFi
#include <PubSubClient.h>    // MQTT
#include <DFRobot_DHT20.h>  // DHT20
#include "PCF8574.h"        // Extension de GPIO via I²C pour contrôler volets
#include <Wire.h>           // Gestion de l'I²C


const int inputPinD = 0;    // GPIO 0 du PCF pour retour commande de descente volet 1
const int outputPinD = 1;   // GPIO 1 du PCF pour commande de descente volet 1
const int inputPinM = 2;    // GPIO 2 du PCF pour retour commande de montée volet 1
const int outputPinM = 3;   // GPIO 3 du PCF pour commande de montée volet 1
const int inputPinD2 = 4;   // GPIO 4 du PCF pour retour commande de descente volet 2
const int outputPinD2 = 5;  // GPIO 5 du PCF pour commande de descente volet 2
const int inputPinM2 = 6;   // GPIO 6 du PCF pour retour commande de montée volet 2
const int outputPinM2 = 7;  // GPIO 7 du PCF pour commande de montée volet 2

const int outputPinL = 6;    // GPIO pour la sortie lumière
const int inputPinL = 7;     // GPIO relais retour lumière
const int PinRad = 5;        // GPIO pour commande transistor chauffage
const int TEMT6000_PIN = 4;  // Entrée GPIO pour capteur luminosité
const int SDA_DHT20 = 18;    // GPIO pour l'I²C
const int SCL_DHT20 = 19;

unsigned long previousMillis1 = 0;  // Variable pour stocker le temps du dernier envoi
const long interval_bit_vie = 1000; // Bit de vie tous les 1s
unsigned long previousMillis2 = 0;
const long interval_hum = 2000;     // Envoi humidité tous les 2s
unsigned long previousMillis3 = 0;
const long interval_temp = 2000;    // Envoi température tous les 2s
unsigned long previousMillis4 = 0;
const long interval_volet = 500;    // Rafraichissement volet tous les 100ms    
unsigned long previousMillis5 = 0;
const long interval_lux = 1000;    // Envoi luminosite ambiante tous les 1s
unsigned long previousMillis6 = 0;
const long interval_volet2 = 500;  // Rafraichissement volet tous les 500ms
unsigned long previousMillis7 = 0;
const long intervalLum = 300; // Intervalle pour changer l'état de la lumière
int counter = 0;                  // Bit de vie
int pos = 0;                      // Position du volet 1
int pos2 = 0;                     // Position du volet 2
bool changer_etat_lum = false;


DFRobot_DHT20 dht20;                              // Déclaration de l'objet DHT20 (addresse 0x38)
PCF8574 pcf8574(0x20, SDA_DHT20, SCL_DHT20);      // Adresse I2C du PCF8574 (addresse 0x20)

const char* ssid = "SAE_DomoIQ";
const char* wifi_password = "Valerieestlaplusbelle59";
const char* mqtt_server = "192.168.200.114";
const int mqtt_port = 1883;
const char* volet_topic = "home/Sam/volet";
const char* volet_topic2 = "home/Sam/volet2";
const char* vie_topic = "home/Sam/bit";
const char* etatvolet_topic = "home/Sam/voletconf";
const char* posvolet_topic = "home/Sam/posvolet";
const char* etatvolet_topic2 = "home/Sam/voletconf2";
const char* posvolet_topic2 = "home/Sam/posvolet2";
const char* lux_topic = "home/Sam/lux";
const char* lum_topic = "home/Sam/lum";
const char* retour_lum_topic = "home/Sam/lumconf";
const char* hum_topic = "home/Sam/hum";
const char* temp_topic = "home/Sam/temp";
const char* rad_topic = "home/Sam/rad";


const char* mqtt_username = "Valerie";
const char* mqtt_password = "Damidomotique";
const char* clientID = "ESP-SAM";
WiFiClient wifiClient;
PubSubClient client(wifiClient);

void connectWiFi() {
  Serial.println("Connexion au WIFI...");
  WiFi.begin(ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("En cours...");
  }
  Serial.println("Connecté au WIFI !");
}

void connectMQTT() {
  while (!client.connected()) {
    Serial.println("Connexion au MQTT...");
    if (client.connect(clientID, mqtt_username, mqtt_password)) {
      Serial.println("Connecté au MQTT!");
      client.subscribe(volet_topic);  // On s'abonne aux sujets (on reçoit des valeurs)
      client.subscribe(volet_topic2);
      client.subscribe(lum_topic);
      client.subscribe(rad_topic);
    } else {
      Serial.print("Erreur de connexion, car: ");
      Serial.print(client.state());
      Serial.println("Reconnexion...");
      delay(2000);
    }
  }
}

void sendEtatVolet() {
  // Envoyer l'état des volets en réponse
  if (pcf8574.digitalRead(inputPinM) == HIGH && pcf8574.digitalRead(inputPinD) == LOW) {
    client.publish(etatvolet_topic, "0");
  }
  else if (pcf8574.digitalRead(inputPinM) == LOW && pcf8574.digitalRead(inputPinD) == HIGH) {
    client.publish(etatvolet_topic, "1");
  } 
  delay(10);
}
void sendEtatVolet2() {
  // Envoyer l'état des volets en réponse
  if (pcf8574.digitalRead(inputPinM) == HIGH && pcf8574.digitalRead(inputPinD) == LOW) {
    client.publish(etatvolet_topic2, "0");
  }
  else if (pcf8574.digitalRead(inputPinM) == LOW && pcf8574.digitalRead(inputPinD) == HIGH) {
    client.publish(etatvolet_topic2, "1");
  } 
  delay(10);
}

void sendVie() {
  unsigned long currentMillis1 = millis();  // Obtient le temps actuel
  // Vérifie si l'interval_bit_viele de temps est écoulé
  if (currentMillis1 - previousMillis1 >= interval_bit_vie) {
    // Incrémente le compteur et ajuste à 0 si nécessaire
    counter = (counter + 1) % 256;
    // Envoi du numéro via MQTT
    client.publish(vie_topic, String(counter).c_str());
    // Mise à jour du temps du dernier envoi
    previousMillis1 = currentMillis1;
  }
}
void sendLux(){
  unsigned long currentMillis5 = millis();  // Obtient le temps actuel
  // Vérifie si l'interval_bit_viele de temps est écoulé
  if (currentMillis5 - previousMillis5 >= interval_lux) {
    //int lumino = random(7500);
    int sensorValue = analogRead(TEMT6000_PIN);
    // Convertir la valeur analogique en une valeur de tension
    float voltage = sensorValue * (3.3 / 4095.0);
    // Convertir la tension en courant
    float currentMicroamps = (voltage / 10000.0) * 1e6;  // Convertir la tension en courant (µA)
    // Convertir le courant en lux
    float lux = 2 * currentMicroamps;  // Formule: lux = 2 * microamps
    client.publish(lux_topic, String(lux).c_str());
    // Mise à jour du temps du dernier envoi
    previousMillis5 = currentMillis5;
  }
}
void sendRetourLum(){
  bool etat = digitalRead(inputPinL);
  //Serial.println("Pin retour lumière: ");
  //Serial.println(etat);
  client.publish(retour_lum_topic, String(etat).c_str());
}
void basculerLum() {
  if (changer_etat_lum) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis7 >= intervalLum) {
      digitalWrite(outputPinL, LOW); // Éteindre la lumière après l'intervalle
      changer_etat_lum = false; // Réinitialiser le drapeau
    }
  }
}
void sendHum(){
  unsigned long currentMillis2 = millis();  // Obtient le temps actuel
  // Vérifie si l'intervale de temps est écoulé
  if (currentMillis2 - previousMillis2 >= interval_hum) {
    ///int humi = random(99);
	float humi = dht20.getHumidity()*100;
	client.publish(hum_topic, String(humi).c_str());
	// Mise à jour du temps du dernier envoi
	previousMillis2 = currentMillis2;
  }
}
void sendTemp(){
  unsigned long currentMillis3 = millis();  // Obtient le temps actuel
  // Vérifie si l'interval_bit_viele de temps est écoulé
  if (currentMillis3 - previousMillis3 >= interval_temp) {
     //int tempe = random(50);
     float tempe = dht20.getTemperature();
     client.publish(temp_topic, String(tempe).c_str());
     // Mise à jour du temps du dernier envoi
     previousMillis3 = currentMillis3;
  }
} 
void sendPosVolet(){
  unsigned long currentMillis4 = millis();  // Obtient le temps actuel
  // Vérifie si l'interval_bit_viele de temps est écoulé
  if (currentMillis4 - previousMillis4 >= interval_volet) {
    // Incrémente le compteur et ajuste à 0 si nécessaire
    pos = (pos + 1) % 100;
    // Envoi du numéro via MQTT
    client.publish(posvolet_topic, String(pos).c_str());
    // Mise à jour du temps du dernier envoi
    previousMillis4 = currentMillis4;
  }
}
void sendPosVolet2(){
  unsigned long currentMillis6 = millis();  // Obtient le temps actuel
  // Vérifie si l'interval_bit_viele de temps est écoulé
  if (currentMillis6 - previousMillis6 >= interval_volet) {
    // Incrémente le compteur et ajuste à 0 si nécessaire
    pos2 = (pos2 + 1) % 100;
    // Envoi du numéro via MQTT
    client.publish(posvolet_topic2, String(pos).c_str());
    // Mise à jour du temps du dernier envoi
    previousMillis6 = currentMillis6;
  }
}

void setup() {
  Serial.begin(115200);
  pcf8574.pinMode(inputPinD, INPUT);
  pcf8574.pinMode(outputPinD, OUTPUT);
  pcf8574.pinMode(inputPinM, INPUT);
  pcf8574.pinMode(outputPinM, OUTPUT);
  pcf8574.pinMode(inputPinD2, INPUT);
  pcf8574.pinMode(outputPinD2, OUTPUT);
  pcf8574.pinMode(inputPinM2, INPUT);
  pcf8574.pinMode(outputPinM2, OUTPUT);
  pinMode(outputPinL, OUTPUT);
  pinMode(inputPinL, INPUT_PULLUP);
  pinMode(TEMT6000_PIN, INPUT);
  pinMode(PinRad, OUTPUT);
  Wire.begin(SDA_DHT20, SCL_DHT20); // DHT20, SDA puis SCL
  pcf8574.begin();
  dht20.begin();

  connectWiFi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback([](char* topic, byte* payload, unsigned int length) {
    Serial.print("Message reçu: ");
    Serial.println(topic);
    if (strcmp(topic, volet_topic) == 0) {
      if (payload[0] == '1') {
        pcf8574.digitalWrite(outputPinM, HIGH);
        pcf8574.digitalWrite(outputPinD, LOW);
        Serial.println("En cours de montée");
      } else if (payload[0] == '0') {
        pcf8574.digitalWrite(outputPinM, LOW);
        pcf8574.digitalWrite(outputPinD, LOW);
        Serial.println("Volets stoppés");
      } else if (payload[0] == '2') {
        pcf8574.digitalWrite(outputPinM, LOW);
        pcf8574.digitalWrite(outputPinD, HIGH);
        Serial.println("En cours de descente");
      }
    }
    else if (strcmp(topic, volet_topic2) == 0) {
      if (payload[0] == '1') {
        pcf8574.digitalWrite(outputPinM2, HIGH);
        pcf8574.digitalWrite(outputPinD2, LOW);
        Serial.println("En cours de montée");
      } else if (payload[0] == '0') {
        pcf8574.digitalWrite(outputPinM2, LOW);
        pcf8574.digitalWrite(outputPinD2, LOW);
        Serial.println("Volets stoppés");
      } else if (payload[0] == '2') {
        pcf8574.digitalWrite(outputPinM2, LOW);
        pcf8574.digitalWrite(outputPinD2, HIGH);
        Serial.println("En cours de descente");
      }
    }  
    else if (strcmp(topic, lum_topic) == 0) {
      if (payload[0] == '1' || payload[0] == '0') {
        changer_etat_lum = true; // Indiquer qu'il faut changer l'état de la lumière
        previousMillis7 = millis(); // Enregistrer le temps actuel
        digitalWrite(outputPinL, HIGH);
        Serial.println("Changer état de la lumière");
      }
    }
    else if (strcmp(topic, rad_topic) == 0) {
      if (payload[0] == '0') {
        Serial.println("Eteindre radiateur");
        digitalWrite(PinRad, LOW);
      }
      else if (payload[0] == '1'){
        Serial.println("Allumer radiateur");
        digitalWrite(PinRad, HIGH);
      }
    }
  });
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();
  sendVie();          // Envoi bit de vie 
  //sendEtatVolet();    // Envoi état commande volet
  //sendEtatVolet2();
  //sendPosVolet();     // Envoi position volet
  //sendPosVolet2();
  sendRetourLum();
  basculerLum(); // Gérer le changement d'état de la lumière
  sendLux();          // Envoi luminosité
  sendHum();          // Envoi de l'humidité
  sendTemp();         // Envoi de la température ambiante
  delay(5);           // Attendre 5 ms (Evite de saturer le CPU)
}


