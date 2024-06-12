#include "SensorClass.h"
#include "Arduino.h"

// améliorations: gérer les erreurs de sorties d'array (index trop grand)

//-----------------------------------SETTERS--------------------------------------//
void Sensor::SetSensorName(String NewName)
{
  this->SensorName=NewName;
}

void Sensor::SetDatasetsName(int DatasetIndex, String DatasetNewName)
{
  this->DatasetsNames[DatasetIndex]=DatasetNewName;
}

void Sensor::SetDatasetsValue(int DatasetIndex, float DatasetNewValue)
{
  this->DatasetsValues[DatasetIndex]=DatasetNewValue;
}

void Sensor::SetDatasetsUnits(int DatasetIndex,String DatasetNewUnit)
{
  this->DatasetsUnits[DatasetIndex]=DatasetNewUnit;
}

void Sensor::SetUpdateTime(int NewUpdateTime)
{
  if (NewUpdateTime<MinUpdateTime)
  {
    this->UpdateTime=MinUpdateTime;
  }
  else
  {
  this->UpdateTime=NewUpdateTime;
  }
    
}

void Sensor::SetCurrentState(String NewCurrentState)
{
  this->CurrentState=NewCurrentState;
}

void Sensor::SetUpdatefunc(String (*NewUpdateFunc)(Sensor &self))
{
  this->UpdateFunc=NewUpdateFunc;
}

void Sensor::SetMinUpdateTime(int MinUpdateTime)
{
  this->MinUpdateTime=MinUpdateTime;
}
//-----------------------------------GETTERS--------------------------------------//

String Sensor::GetSensorName()
{
  return SensorName;
}

String Sensor::GetDatasetsName(int DatasetIndex)
{
  return DatasetsNames[DatasetIndex];
}

float Sensor::GetDatasetsValue(int DatasetIndex)
{
  return DatasetsValues[DatasetIndex];
}

int Sensor::GetUpdateTime()
{
  return UpdateTime;
}

String Sensor::GetCurrentState()
{
  return CurrentState;
}

void Sensor::UpdateData()
{
  CurrentState=UpdateFunc(*this); //Mise à jour de l'objet de la classe (Valeurs, unités,...)
}
