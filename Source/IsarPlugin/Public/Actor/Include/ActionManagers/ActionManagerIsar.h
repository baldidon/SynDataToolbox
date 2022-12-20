#pragma once
#include "ActionManagerIsar.generated.h"

UINTERFACE(MinimalAPI,Blueprintable)
class UActionManagerIsar : public UInterface
{
    GENERATED_BODY()
};

class ISARPLUGIN_API IActionManagerIsar
{
    GENERATED_BODY()

public:
    /** Add interface function declarations here */
    virtual const FString GetActionManagerName() const;
    virtual const bool IsActionManagerName(const FString& Name) const;
    virtual const bool InitSettings(const TArray<FString>& Settings); //useless with new api version
    virtual const int8_t PerformAction(TArray<FString>& Action);
    virtual const int ActionToID(const FString& Action) const;
    virtual void Possess();
    virtual void UnPossess();


};