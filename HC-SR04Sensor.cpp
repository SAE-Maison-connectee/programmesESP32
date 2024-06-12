#include "Ultrasonic.h"
#include "Arduino.h"
#include "SensorClass.h"
uint32_t PresHistory[10];
int PresencePin = 7;
Ultrasonic SRO4(PresencePin);
bool initbitSRO4=false;
uint32_t Average,Sum;
float Threshold=30;
String UpdatePresence(Sensor &self)
{
  String state;
  bool Presence=false;
  uint32_t CurrentDist=0;
  
  while(!initbitSRO4)
  {
    self.SetDatasetsName(0,"Présence");
    self.SetDatasetsUnits(0,"personne détectée");
  }
  Sum=PresHistory[9];
  for (int i =0;i<9;i++)
  {
    Sum+=PresHistory[9-i-1];
    PresHistory[9-i]=PresHistory[9-i-1];
  }
  CurrentDist=SRO4.MeasureInCentimeters();
  PresHistory[0]=CurrentDist;
  Average=(float)Sum/10;
  if (Average-(float)CurrentDist>=Threshold)
  {
    Presence=true;
  }
  else
  {
    Presence=false;
  }
  return state="PRESENCE SUCCESSFULLY UPDATED";
}