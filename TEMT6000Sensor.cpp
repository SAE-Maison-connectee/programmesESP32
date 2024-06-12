#include "SensorClass.h"
#include <TEMT6000.h>
int TEMT6000pin=2 ;
bool initbitTEMT6000=false;
TEMT6000Class TEMT6000;
String UpdateTEMT6000(Sensor &self)
{
  int lightlevel;
  String State;
  while (!initbitTEMT6000)
  {
    self.SetDatasetsName(0,"Luminosité");
    self.SetDatasetsUnits(0,"Lux");
    TEMT6000.TEMT6000_init(TEMT6000pin);
    initbitTEMT6000=true;
  }
  lightlevel=TEMT6000.TEMT6000_getLight();
  self.SetDatasetsValue(0,(float)lightlevel);
  State="LIGHTLEVEL SUCCESSFULLY UPDATED";
  return State;
}
