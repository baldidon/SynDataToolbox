#pragma once
#include "ResetManagerIsar.generated.h"

// Class declaration
UINTERFACE(MinimalAPI, Blueprintable)
class UResetManagerIsar : public UInterface
{
	GENERATED_BODY()
};

class IResetManagerIsar{
	GENERATED_BODY()
	
public:
	virtual const FString GetResetManagerName() const;
	virtual const bool IsResetManagerName(const FString& Name) const;
	virtual const bool PerformReset(TArray<FString>& FieldArray);
	virtual const bool ChangeResetSettings(const TArray<FString>& Setting);
};
