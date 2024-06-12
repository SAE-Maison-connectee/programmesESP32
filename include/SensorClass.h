#ifndef SENSORCLASSTOKEN
#define SENSORCLASSTOKEN
#include <Arduino.h>
class Sensor
{
  private:
  String SensorName;
  int DataSetNbr;
  String DatasetsNames[5];
  float DatasetsValues[5];
  String DatasetsUnits[5];
  int UpdateTime;
  int MinUpdateTime;
  String CurrentState;
  String (*UpdateFunc)(Sensor &self);
  public:
    int caca;


    // Setters

    void SetSensorName(String NewName);
    void SetDatasetsName(int DatasetIndex, String DatasetNewName);
    void SetDatasetsValue(int DatasetIndex, float DatasetNewValue);
    void SetDatasetsUnits(int DatasetIndex, String DatasetNewUnits);
    void SetUpdateTime(int NewUpdateTime);
    void SetCurrentState(String NewCurrentState);
    void SetUpdatefunc(String (*NewUpdateFunc)(Sensor &self));
    void SetMinUpdateTime(int NewMinUpdateTime);

    // Getters

    String GetSensorName();
    String GetDatasetsName(int DatasetIndex);
    float GetDatasetsValue(int DatasetIndex);
    int GetUpdateTime();
    String GetCurrentState();
    void UpdateData();

    // Constructors
    Sensor(String NewSensorName,String (*NewUpdateFunc)(Sensor &self), int NewMinUpdateTime)
    {
      SensorName=NewSensorName;
      UpdateFunc=NewUpdateFunc;
      MinUpdateTime=NewMinUpdateTime;
    }
    Sensor(){}



};
#endif