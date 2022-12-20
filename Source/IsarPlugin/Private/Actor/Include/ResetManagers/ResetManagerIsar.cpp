#include "Actor/Include/ResetManagers/ResetManagerIsar.h"

const FString IResetManagerIsar::GetResetManagerName() const
{
    return FString();
}

const bool IResetManagerIsar::IsResetManagerName(const FString& Name) const
{
    return false;
}

const bool IResetManagerIsar::PerformReset(TArray<FString>& FieldArray)
{
    return false;
}

const bool IResetManagerIsar::ChangeResetSettings(const TArray<FString>& Setting)
{
    return false;
}
