#include <WiFi.h>             // Com WiFi
#include <PubSubClient.h>     // MQTT
#include <DFRobot_DHT20.h>    // DHT20
#include <SPI.h>              // Comm SPI pour RFID
#include <MFRC522.h>          // RFID
#include <Ultrasonic.h>       // Ultrason


#define SS_PIN  19            // Config SPI RFID
#define RST_PIN 18
// Rappel PIN RFID: MOSI pin 6 	MISO pin 5 	SCK pin 4
const int TEMT6000_PIN = 0;  // Entree GPIO pour capteur luminosité
const int PinVerrou = 2;      // Sortie GPIO pour commande verrou porte
const int SDA_DHT20 = 8;      // GPIO pour le DHT20 (I²C)
const int SCL_DHT20 = 9;
const int PinPres = 7;        // GPIO pour le capteur à ultrason

unsigned long previousMillis1 = 0;  // Variable pour stocker le temps du dernier envoi
const long interval_bit_vie = 1000; // Envoi bit de vie tous les 1s
unsigned long previousMillis2 = 0;
const long interval_hum = 2000;     // Envoi humidite tous les 2s
unsigned long previousMillis3 = 0;
const long interval_temp = 2000;    // Envoi temperature tous les 2s
unsigned long previousMillis5 = 0;
const long interval_lux = 1000;     // Envoi luminosite ambiante tous les 1s
unsigned long previousMillis6 = 0;
const long interval_pres = 1000;    // Envoi presence ultrasons tous les 1s
int counter = 0;                    // Bit de vie


MFRC522 rfid(SS_PIN, RST_PIN);      // Objet RFID
DFRobot_DHT20 dht20;                // Objet DHT20
Ultrasonic ultrasonic(PinPres);     // Déclaration de l'objet ultrason

const char* ssid = "SAE_DomoIQ";
const char* wifi_password = "Valerieestlaplusbelle59";
const char* mqtt_server = "192.168.200.114";
const int mqtt_port = 1883;
const char* lux_topic = "home/Ext/lux"; // Cpt luminosite
const char* vie_topic = "home/Ext/bit"; // Bit de vie
const char* temp_topic = "home/Ext/temp"; // Température
const char* hum_topic = "home/Ext/hum"; // Humidité
const char* RFID_topic = "home/Ext/rfid"; // UUID RFID
const char* verrou_topic = "home/Ext/verrou"; // Déverouillage porte
const char* retour_verrou_topic = "home/Ext/retourverrou";
const char* presence_topic= "home/Ext/pres"; // Presence personne par ultrasons


const char* mqtt_username = "Valerie";
const char* mqtt_password = "Damidomotique";
const char* clientID = "ESP-EXT";
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
  	client.subscribe(verrou_topic); // On s'abonne aux sujets (on reçoit des valeurs)
  } else {
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
  // Vérifie si l'intervale de temps est écoulé
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
  // Vérifie si l'interval de temps est écoulé
  if (currentMillis3 - previousMillis3 >= interval_temp) {
	//int tempe = random(50);
	float tempe = dht20.getTemperature();
	client.publish(temp_topic, String(tempe).c_str());
	// Mise à jour du temps du dernier envoi
	previousMillis3 = currentMillis3;
  }
}
void sendVerrou(){
  bool etat_verrou = digitalRead(PinVerrou);
  client.publish(retour_verrou_topic, String(etat_verrou).c_str());
}
void sendRFID(){
  if (rfid.PICC_IsNewCardPresent()) { // carte RFID disponible
    if (rfid.PICC_ReadCardSerial()) { // Lecture d’un code RFID via le bus SPI

      // Crée une chaîne de caractères pour stocker les valeurs hexadécimales
      String code_carte = "UID:";
      for (int i = 0; i < rfid.uid.size; i++) {
        // Ajoute chaque octet hexadécimal à la chaîne de caractères "code_carte"
        code_carte += (rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        code_carte += String(rfid.uid.uidByte[i], HEX);
      }

      // Affiche le code de la carte RFID
      Serial.println(code_carte);

      // Compare code_carte à une chaîne prédéfinie
      if (strcmp(code_carte.c_str(), "UID: 35 04 bc 54") == 0) {
        Serial.println("Code de carte valide !");
        client.publish(RFID_topic, String("Carte valide").c_str());
        delay(300);
        client.publish(RFID_topic, String("Pas de carte").c_str());
      } else {
        Serial.println("Pas de carte");
        client.publish(RFID_topic, String("Pas de carte").c_str());
      }
    }
  }
}
void sendPresence(){
  unsigned long currentMillis6 = millis();
  int seuil = 18; // distance avant déclenchement (en cm)
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

void setup() {
  Serial.begin(115200);

  ledcSetup(0, 5000, 8); // Configuration du signal PWM: Voie 0, freq = 5kHz, 8 bit de résolution
  ledcAttachPin(PinVerrou, 0); // Autoriser le PWM sur la broche en utilisant la voie 0

  pinMode(TEMT6000_PIN, INPUT);
  pinMode(PinVerrou, OUTPUT);
  Wire.begin(SDA_DHT20,SCL_DHT20); // DHT20, SDA puis SCL
  dht20.begin();
  SPI.begin();
  rfid.PCD_Init();

  connectWiFi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback([](char* topic, byte* payload, unsigned int length) {
	Serial.print("Message reçu: ");
	Serial.println(topic);
	if (strcmp(topic, verrou_topic) == 0) {  // Contrôler porte en fonction du message reçu
  	if (payload[0] == '1') {
    	ledcWrite(0, 70);  // PWM sur la voie 0 à 70/255 pour limiter le courant à 1A
    	Serial.println("Porte fermée");
  	} else if (payload[0] == '0') {
    	ledcWrite(0, 0);  // PWM sur la voie 0 à 0%, pas de signal
    	Serial.println("Porte ouverte");
  	}
	}
  });
}

void loop() {
  if (!client.connected()) {
	connectMQTT();
  }
  client.loop();    // Gestion du service MQTT
  sendVie();      	// Envoi bit de vie
  sendLux();      	// Envoi luminosité
  sendHum();        // Envoi de l'humidité ambiante
  sendTemp();       // Envoi de la température ambiante
  sendVerrou();     // Envoi de l'état du verrou
  sendRFID();       // Envoi de l'UUID RFID
  sendPresence();   // Envoi de la présence personne
  delay(5);  // Attendre 5 ms (Evite de saturer le CPU)
}


