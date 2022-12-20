#pragma once
#include "SensorIsar.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class USensorIsar: public UInterface
{
    GENERATED_BODY()
};

class ISARPLUGIN_API ISensorIsar
{
    GENERATED_BODY()

public:
    /** Add interface function declarations here */
   virtual const FString GetSensorName() ;
   virtual const FString GetSensorSetup();
   virtual const uint32 GetObservationSize();
   virtual const bool IsSensorName(const FString& Name);
   virtual const bool InitSensor();
   virtual const bool ChangeSensorSettings(const TArray<FString>& settings);
   virtual const bool GetLastObs(uint8* Buffer);
   virtual const bool TakeObs(); //the previous GetSensorObs

protected:

};