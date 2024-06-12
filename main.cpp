#include "SensorClass.h"
#include "Arduino.h"

String UpdatePresence(Sensor &self);
String UpdateRFID(Sensor &self);
String UpdateDH20(Sensor &self);
String UpdateTEMT6000(Sensor &self);
String UpdateWATRHTR(Sensor &self);

void UpdateSensors(Sensor (&SensorArray)[5]);
Sensor DHT20("DHT20",&UpdateDH20,2000);
Sensor WTRHTR("DS18B20",&UpdateWATRHTR,2000);
Sensor TEMT6000Sensor("TEMT6000",&UpdateTEMT6000,2000);
Sensor RFIDreader("MRFC522",&UpdateRFID,10);
Sensor HCSR04("HCSR04",&UpdatePresence,10);
Sensor SensorArray[5]={DHT20,WTRHTR,TEMT6000Sensor,RFIDreader,HCSR04};


void setup()
{

}
void loop()
{
    UpdateSensors(SensorArray);
}
