#include <WiFi.h>             // Com WiFi
#include <PubSubClient.h>     // MQTT
#include <DFRobot_DHT20.h>    // DHT20
#include <Ultrasonic.h>       // Ultrason
#include <OneWire.h>          // Sonde DS18B20
#include <DallasTemperature.h>

const int outputPinL = 6;     // GPIO  pour la sortie lumière
const int inputPinL = 7;      // GPIO relais retour lumière
const int PinRad = 5;        // GPIO pour commande chauffage
const int TEMT6000_PIN = 4;   // GPIO de l'entrée pour le capteur de luminosité (ADC)
const int PinCe = 3;          // GPIO de commande PWM chauffe-Eau
const int PinTempCe = 10;     // GPIO de la sonde DS18B20 chauffe-eau
const int PinVmc = 2;	        // GPIO de commande PWM VMC
const int SDA_DHT20 = 18;     // GPIO pour le DHT20 (I²C)
const int SCL_DHT20 = 19;
const int PinPres = 8;        // GPIO pour le capteur à ultrason

unsigned long previousMillis1 = 0;  // Variable pour stocker le temps du dernier envoi
const long interval_bit_vie = 1000; // Envoi bit de vie tous les 1 s
unsigned long previousMillis2 = 0;
const long interval_hum = 2000;     // Envoi humidite ambiante tous les 2s
unsigned long previousMillis3 = 0;
const long interval_temp = 2000;    // Envoi temperature ambiante tous les 2s
unsigned long previousMillis5 = 0;
const long interval_lux = 1000;     // Envoi luminosite ambiante tous les 1s
unsigned long previousMillis6 = 0;
const long interval_pres = 1000;    // Envoi presence ultrasons tous les 1s
unsigned long previousMillis7 = 0;
const long interval_tempce = 1000;  // Envoi temperature chauffe-eau tous les 1s
int counter = 0;                    // Bit de vie

DFRobot_DHT20 dht20;                // Déclaration de l'objet DHT20
OneWire oneWire (PinTempCe);        // Déclaration de l'objet DS18B20
DallasTemperature masonde (&oneWire);
Ultrasonic ultrasonic(PinPres);     // Déclaration de l'objet ultrason

const char* ssid = "SAE_DomoIQ";
const char* wifi_password = "Valerieestlaplusbelle59";
const char* mqtt_server = "192.168.200.114";
const int mqtt_port = 1883;
const char* vie_topic = "home/Sdb/bit";
const char* lux_topic = "home/Sdb/lux";
const char* lum_topic = "home/Sdb/lum";
const char* retour_lum_topic = "home/Sdb/lumconf";
const char* hum_topic = "home/Sdb/hum";
const char* temp_topic = "home/Sdb/temp";
const char* presence_topic= "home/Sdb/pres"; // Présence personne par ultrasons
const char* rad_topic = "home/Sdb/rad";
const char* ce_topic = "home/Sdb/ce";
const char* tempce_topic = "home/Sdb/tempce";
const char* vmc_topic = "home/Sdb/vmc";

const char* mqtt_username = "Valerie";
const char* mqtt_password = "Damidomotique";
const char* clientID = "ESP-SDB";
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
      client.subscribe(lum_topic); // On s'abonne aux sujets (on reçoit des valeurs)
      client.subscribe(rad_topic);
      client.subscribe(ce_topic);
      client.subscribe(vmc_topic);
    }  
    else {
      Serial.print("Erreur de connexion, car: ");
      Serial.print(client.state());
      Serial.println("Reconnexion...");
      delay(2000);
    }
  }
}

void sendVie() {
  unsigned long currentMillis1 = millis();  // Obtient le temps actuel
  // Vérifie si l'intervale de temps est écoulé
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
void sendHum(){
  unsigned long currentMillis2 = millis();  // Obtient le temps actuel
  // Vérifie si l'interval_bit_viele de temps est écoulé
  if (currentMillis2 - previousMillis2 >= interval_hum) {
	  //int humi = random(99);
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
void sendPresence(){
  unsigned long currentMillis6 = millis();
  int seuil = 30; // distance avant déclenchement (en cm)
  int presence = 0;
  int distance = 0;

  if (currentMillis6 - previousMillis6 >= interval_pres) {
  	distance = ultrasonic.read();
	  if (distance <= seuil) {
  	  presence = 1;
    }
    else{
      presence = 0;
    }
    client.publish(presence_topic, String(presence).c_str());
	  previousMillis6 = currentMillis6;
	}
}
void sendTempCe(){
  unsigned long currentMillis7 = millis();  // Obtient le temps actuel
  // Vérifie si l'interval_bit_viele de temps est écoulé
  if (currentMillis7 - previousMillis7 >= interval_tempce) {
	  //int tempe = random(50);
    masonde.requestTemperatures(); // Demander au capteur DS18B20 de lire la température
    float temperatureC = masonde.getTempCByIndex(0);// Obtenir la température en Celsius
    //Serial.print("Temperature: ");
    //Serial.print(temperatureC);
    //Serial.println(" °C");
	  client.publish(tempce_topic, String(temperatureC).c_str());
	  // Mise à jour du temps du dernier envoi
	  previousMillis7 = currentMillis7;

  }
}
void sendRetourLum(){
  bool etat = digitalRead(inputPinL);
  //Serial.println("Pin retour lumière: ");
  //Serial.println(etat);
  client.publish(retour_lum_topic, String(etat).c_str());
  delay(500); // Petit delai permettant de maintenir la valeur, sinon on ne voit pas le changement au niveau MQTT
}

void setup() {
  Serial.begin(115200);
  
  ledcSetup(1, 5000, 8); // Configuration du signal PWM: Voie 1, freq = 1kHz, 8 bit de résolution
  ledcAttachPin(PinCe, 1); // Autoriser le PWM sur la broche en utilisant la voie 1
  ledcSetup(2, 5000, 8); // Configuration du signal PWM: Voie 2, freq = 5kHz, 8 bit de résolution
  ledcAttachPin(PinVmc, 2); // Autoriser le PWM sur la broche en utilisant la voie 2

  pinMode(outputPinL, OUTPUT); // Sortie vers télérupteur
  pinMode(inputPinL, INPUT_PULLUP); // Entrée retour de la commande telerupteur
  pinMode(TEMT6000_PIN, INPUT); // Entrée du cpt luminosité
  pinMode(PinCe, OUTPUT); // Sortie vers chauffe-eau
  pinMode(PinRad, OUTPUT); // Sortie radaiteur
  Wire.begin(SDA_DHT20, SCL_DHT20); // SDA puis SCL pour DHT20
  dht20.begin();  // Initialisation du capteur DHT20
  masonde.begin();  // Initialisation du capteur DS18B20


  connectWiFi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback([](char* topic, byte* payload, unsigned int length) {
    Serial.print("Message reçu: ");
    Serial.println(topic);
   if (strcmp(topic, lum_topic) == 0) {  // Contrôler la lumière en fonction du message reçu
      if (payload[0] == '1' || payload[0] == '0') {
        Serial.println("Changer état de la lumière");
        digitalWrite(outputPinL, HIGH);
        delay(300);
        digitalWrite(outputPinL, LOW);
      }
    }
    else if (strcmp(topic, rad_topic) == 0) {  // Contrôler le radiateur
      if (payload[0] == '0') {
        Serial.println("Eteindre radiateur");
        digitalWrite(PinRad, LOW);
      }
      else if (payload[0] == '1'){
        Serial.println("Allumer radiateur");
        digitalWrite(PinRad, HIGH);
      }
    }
    else if (strcmp(topic, ce_topic) == 0) {  // Contrôler le chauffe-eau en fonction du message reçu
      char payload_str[length + 1];
      memcpy(payload_str, payload, length);
      payload_str[length] = '\0';  // Ajoute le caractère de fin de chaîne

      int rapport_cyclique = atoi(payload_str);  // Convertir le payload en entier
      Serial.print("Rapport cyclique reçu pour chauffe-eau: ");
      Serial.println(rapport_cyclique);

      // Vérifiez que le rapport cyclique est dans la plage acceptable pour le PWM
      if (rapport_cyclique >= 0 && rapport_cyclique <= 255) {
          ledcWrite(1, rapport_cyclique);  // Appliquez le rapport cyclique
      }
    }
    else if (strcmp(topic, vmc_topic) == 0) {  // Contrôler la VMC
      char payload_str[length + 1];
      memcpy(payload_str, payload, length);
      payload_str[length] = '\0';  // Ajoute le caractère de fin de chaîne

      int rapport_cyclique = atoi(payload_str);  // Convertir le payload en entier
      Serial.print("Rapport cyclique reçu pour VMC: ");
      Serial.println(rapport_cyclique);

      // Vérifiez que le rapport cyclique est dans la plage acceptable pour le PWM
      if (rapport_cyclique >= 0 && rapport_cyclique <= 255) {
          ledcWrite(2, rapport_cyclique);  // Appliquez le rapport cyclique
      }
    }
  });
}

void loop() {
  if (!client.connected()) {    // Si on n'arrive pas à se connecter au MQTT, on réessaye
    connectMQTT();
  }
  client.loop();      // Gestion du service MQTT
  sendVie();          // Envoi bit de vie 
  sendLux();          // Envoi luminosité
  sendHum();          // Envoi de l'humidite
  sendTemp();         // Envoi de la température
  sendPresence();     // Envoi de la detection de presence
  sendTempCe();       // Envoi de la température du chauffe-eau
  sendRetourLum();    // Envoi de l'état de la commande télérupteur
  delay(5);           // Attendre 5 ms (évite de saturer le CPU)
}


