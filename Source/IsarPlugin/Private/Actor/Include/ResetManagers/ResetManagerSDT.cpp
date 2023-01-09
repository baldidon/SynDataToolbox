#include "Actor/Include/ResetManagers/ResetManagerSDT.h"

const FString IResetManagerSDT::GetResetManagerName() const
{
    return FString();
}

const bool IResetManagerSDT::IsResetManagerName(const FString& Name) const
{
    return false;
}

const bool IResetManagerSDT::PerformReset(TArray<FString>& FieldArray)
{
    return false;
}

const bool IResetManagerSDT::ChangeResetSettings(const TArray<FString>& Setting)
{
    return false;
}
