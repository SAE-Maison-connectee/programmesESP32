#include "DFRobot_DHT20.h"
#include  "SensorClass.h"
DFRobot_DHT20 dht20;
const int SDA_DHT20 = 18;     // GPIO pour le DHT20 (I²C)
const int SCL_DHT20 = 19;
bool initBitDHT20=false;
String UpdateDH20(Sensor &self )
{
  String State;
  float Temperature,Humidity;
  while(!initBitDHT20)
  {
  self.SetDatasetsName(0,"Temperature");
  self.SetDatasetsName(1,"Humidity");
  self.SetDatasetsUnits(0,"°C");
  self.SetDatasetsUnits(1,"%");

  Wire.begin(SDA_DHT20,SCL_DHT20);

  initBitDHT20=true;
  }
  if(dht20.begin())
  {
    State="SETUP FAILED";
    delay(1000);
  }
  else
  {
  Temperature = dht20.getTemperature();
  Humidity = dht20.getHumidity();
  self.SetDatasetsValue(0,Temperature);
  self.SetDatasetsValue(1,Humidity);
  State="TEMP & HUM SUCCESSFULLY UPDATED";
  }
  return State;
}