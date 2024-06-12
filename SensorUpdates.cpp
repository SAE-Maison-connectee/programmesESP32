#include "SensorClass.h"
#include "Arduino.h"

void UpdateSensors(Sensor (&SensorArray)[5])
{
    int CurrentTime;
    int PreviousTime=0;
    for (Sensor ISensor: SensorArray)
    {
        CurrentTime=millis();
        if ((CurrentTime-PreviousTime)>ISensor.GetUpdateTime())
        {
            ISensor.UpdateData();
            Serial.println(ISensor.GetCurrentState());
        }
    }
}