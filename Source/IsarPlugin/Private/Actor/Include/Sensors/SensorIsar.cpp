#include "Actor/Include/Sensors/SensorIsar.h"

const FString ISensorIsar::GetSensorName()
{
    return FString();
}

const FString ISensorIsar::GetSensorSetup()
{
    //default SensorSetup
    return FString("{}");
}

const uint32 ISensorIsar::GetObservationSize()
{
    return uint32();
}

const bool ISensorIsar::IsSensorName(const FString& Name)
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

const bool ISensorIsar::InitSensor()
{
    return false;
}

const bool ISensorIsar::ChangeSensorSettings(const TArray<FString>& settings)
{
    return false;
}

const bool ISensorIsar::TakeObs()
{
    return false;
}

const bool ISensorIsar::GetLastObs(uint8* Buffer)
{
    return uint8();
}

/*const FString ISensorIsar::GetLastObsTimestamp();*/