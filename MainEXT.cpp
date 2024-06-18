/// Inclusion des headers des biblliothèques
#include <WiFi.h>             // Com WiFi
#include <PubSubClient.h>     // MQTT
#include <DFRobot_DHT20.h>    // DHT20
#include <SPI.h>              // Comm SPI pour RFID
#include <MFRC522.h>          // RFID
#include <Ultrasonic.h>       // Ultrason

/// Définiton des pins utilisés

#define SS_PIN  19            // Config SPI RFID
#define RST_PIN 18 // Rappel PIN RFID: MOSI pin 6 	MISO pin 5 	SCK pin 4
const int TEMT6000_PIN = 0;  // Entree GPIO pour capteur luminosité
const int PinVerrou = 2;      // Sortie GPIO pour commande verrou porte
const int SDA_DHT20 = 8;      // GPIO pour le DHT20 (I²C)
const int SCL_DHT20 = 9;
const int PinPres = 7;        // GPIO pour le capteur à ultrason

/// Initialisation des variables pour les Timers

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

///instantiation des objets "capteurs" et réseau

MFRC522 rfid(SS_PIN, RST_PIN);      // Objet RFID
DFRobot_DHT20 dht20;                // Objet DHT20
Ultrasonic ultrasonic(PinPres);     // Déclaration de l'objet ultrason
WiFiClient wifiClient;
PubSubClient client(wifiClient);

/// @brief logs Wifi
const char* ssid = "SAE_DomoIQ";
const char* wifi_password = "Valerieestlaplusbelle59";
const char* mqtt_server = "192.168.200.114";
const int mqtt_port = 1883;

/// @brief  logs MQTT
const char* mqtt_username = "Valerie";
const char* mqtt_password = "Damidomotique";
const char* clientID = "ESP-EXT";

/// @brief Topics utilisés
const char* lux_topic = "home/Ext/lux"; // Cpt luminosite
const char* vie_topic = "home/Ext/bit"; // Bit de vie
const char* temp_topic = "home/Ext/temp"; // Température
const char* hum_topic = "home/Ext/hum"; // Humidité
const char* RFID_topic = "home/Ext/rfid"; // UUID RFID
const char* verrou_topic = "home/Ext/verrou"; // Déverouillage porte
const char* retour_verrou_topic = "home/Ext/verrouconf";
const char* presence_topic= "home/Ext/pres"; // Presence personne par ultrasons



/// @brief Connexion au wifi
void connectWiFi() {
  Serial.println("Connexion au WIFI...");
  WiFi.begin(ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("En cours...");
  }
  Serial.println("Connecté au WIFI !");
}

/// @brief Connexion au serveur MQTT
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

////////////// Envoi bit de vie //////////////////////
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

////////////////////// Lecture et envoi humidité //////////////////////
void sendLux() {
  unsigned long currentMillis5 = millis();  // Obtient le temps actuel
  // Vérifie si l'intervale de temps est écoulé
  if (currentMillis5 - previousMillis5 >= interval_lux) {
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
//////////////////////// Lecture et envoi Humidité /////////////////////
void sendHum() {
  unsigned long currentMillis2 = millis();  // Obtient le temps actuel
  // Vérifie si l'intervale de temps est écoulé
  if (currentMillis2 - previousMillis2 >= interval_hum) {
    float humi = dht20.getHumidity() * 100;
    client.publish(hum_topic, String(humi).c_str());
    // Mise à jour du temps du dernier envoi
    previousMillis2 = currentMillis2;
  }
}
//////////////////////// Lecture et envoi Température /////////////////////
void sendTemp() {
  unsigned long currentMillis3 = millis();  // Obtient le temps actuel
  // Vérifie si l'intervale de temps est écoulé
  if (currentMillis3 - previousMillis3 >= interval_temp) {
    float tempe = dht20.getTemperature();
    client.publish(temp_topic, String(tempe).c_str());
    // Mise à jour du temps du dernier envoi
    previousMillis3 = currentMillis3;
  }
}
///////////////////// Envoi état verrou ///////////////////////////////
void sendVerrou() { 
  bool etat_verrou = digitalRead(PinVerrou);
  client.publish(retour_verrou_topic, String(etat_verrou).c_str());
}

///////////////////// Lecture et envoi du RFID /////////////////////////
void sendRFID() {
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
      client.publish(RFID_topic, String(code_carte).c_str());
      // Compare code_carte à une chaîne prédéfinie
    }
  }
}
/// @brief envoi de la présence 
void sendPresence() {
  unsigned long currentMillis6 = millis();
  int seuil = 18; // distance avant déclenchement (en cm)
  int presence = 0;
  int distance = 0;
  // Cette fonction ne détecte que si la distance dépasse un seuil
  // pour avoir une vraie détection de présence, 
  if (currentMillis6 - previousMillis6 >= interval_pres) {
    distance = ultrasonic.read();
    if (distance <= seuil) { /// Détection de la distance
      presence = 1;
    } else {
      presence = 0;
    }


    client.publish(presence_topic, String(presence).c_str());
    previousMillis6 = currentMillis6;
  }
}

// fonction théorique testée et fonctionnelle, indépedemment du reste:
// void sendPresence() {
//   unsigned long currentMillis6 = millis();
//     extern int  presenceHistory[10]; // initialisation du tableau en global avec extern
//     bool presence; //état de la présence
//     int sum=0;
//     float Avg=0; // initialisation de la somme et de la moyenne
//     int threshold=30; // seuil de 30 cm;
//   // Cette fonction ne détecte que si la distance dépasse un seuil
//   // pour avoir une vraie détection de présence, 
//   if (currentMillis6 - previousMillis6 >= interval_pres) {

//     int distance = ultrasonic.read(); // lecture de la distance mesurée avec l'ultrason
//     sum=presenceHistory[0]; // presenceHistory[0] est réécrit par la suite, on le stocke donc maintenant

//     for (int i;i<9;i++) //boucle for pour décaler d'une place les distances mesurées précédentes 
//     {
//         sum+=presenceHistory[9-i];// addition de toutes les valeurs à sum
//         presenceHistory[9-i]=presenceHistory[9-i-1]; //décalage
//     }
//     Avg=sum/10.0; // moyenne calculée en float
//     if ((Avg-distance>=threshold)||(Avg-distance<=threshold))// si seuil dépassé, présence détectée
//     {
//         presence=1;
//     }
//     else
//     {
//         presence=0;
//     }
//     presenceHistory[0]=distance;
//     client.publish(presence_topic, String(presence).c_str());
//     previousMillis6 = currentMillis6;
//   }
// }


//// fonction de paramétrage 
void setup() {
  Serial.begin(115200); //bus Série initialisé à 115200 baud/s

  ledcSetup(0, 5000, 8); // Configuration du signal PWM: Voie 0, freq = 5kHz, 8 bit de résolution
  ledcAttachPin(PinVerrou, 0); // Autoriser le PWM sur la broche en utilisant la voie 0

  pinMode(TEMT6000_PIN, INPUT);
  //pinMode(PinVerrou, INPUT); NE PAS METTRE INPUT OU OUTPUT EN LEDC SINON CA MARCHE PAS !!!!
  Wire.begin(SDA_DHT20, SCL_DHT20); // DHT20, SDA puis SCL
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
        Serial.println("Recevoir commande de fermeture");
        ledcWrite(0, 255);  // PWM sur la voie 0 à 200/255 pour 80% duty cycle
        Serial.println("Porte fermée");
      } else if (payload[0] == '0') {
        Serial.println("Recevoir commande d'ouverture");
        ledcWrite(0, 0);  // PWM sur la voie 0 à 0%, pas de signal
        Serial.println("Porte ouverte");
      }
    }
  });
}


/////////////////////////// Boucle Principale ////////////////////////////////////
void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();    // Gestion du service MQTT
  sendVie();        // Envoi bit de vie
  sendLux();        // Envoi luminosité
  sendHum();        // Envoi de l'humidité ambiante
  sendTemp();       // Envoi de la température ambiante
  //sendVerrou();     // Envoi de l'état du verrou NE FONCTIONNE PAS CAR CONFLIT AVEC PWM
  sendRFID();       // Envoi de l'UUID RFID
  sendPresence();   // Envoi de la présence personne
  delay(5);  // Attendre 5 ms (Evite de saturer le CPU)
}
