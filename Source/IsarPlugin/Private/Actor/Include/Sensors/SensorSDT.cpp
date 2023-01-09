#include "Actor/Include/Sensors/SensorSDT.h"

const FString ISensorSDT::GetSensorName()
{
    return FString();
}

const FString ISensorSDT::GetSensorSetup()
{
    //default SensorSetup
    return FString("{}");
}

const uint32 ISensorSDT::GetObservationSize()
{
    return uint32();
}

const bool ISensorSDT::IsSensorName(const FString& Name)
{
    if (Name == GetSensorName())
    {
        return true;
    }
    else
    {
        return false;
    }
}

const bool ISensorSDT::InitSensor()
{
    return false;
}

const bool ISensorSDT::ChangeSensorSettings(const TArray<FString>& settings)
{
    return false;
}

const bool ISensorSDT::TakeObs()
{
    return false;
}

const bool ISensorSDT::GetLastObs(uint8* Buffer)
{
    return uint8();
}

/*const FString ISensorIsar::GetLastObsTimestamp();*/