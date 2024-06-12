
#include "SensorClass.h"
#include "OneWire.h"
#include "DallasTemperature.h"
const int oneWireBus = 10;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
bool initbitWTRHTR=false;

String UpdateWATRHTR(Sensor &self)
{
  String State;
  float WaterTemperature;
  while(!initbitWTRHTR)
  {
    self.SetDatasetsName(0,"Temperature chauffe-eau");
    self.SetDatasetsUnits(0,"°C");
    sensors.begin();
    initbitWTRHTR=true;
  }
  if (sensors.getDeviceCount()==0)
  {
    State= "SETUP FAILED";
  }
  else
  {
    sensors.requestTemperatures();
    WaterTemperature=sensors.getTempCByIndex(0);
    self.SetDatasetsValue(0,WaterTemperature);
    State="WATER TEMPERATURE SUCCESSFULLY UPDATED";
  }
  return State;
}