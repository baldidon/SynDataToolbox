#pragma once
#include "ActionManagerSDT.generated.h"

UINTERFACE(MinimalAPI,Blueprintable)
class UActionManagerSDT : public UInterface
{
    GENERATED_BODY()
};

class SYNDATATOOLBOX_API IActionManagerSDT
{
    GENERATED_BODY()

public:
    /** Add interface function declarations here */
    virtual const FString GetActionManagerName() const;
    virtual const FString GetActionManagerSetup() const; 	//in this method are stored commands and other setup info
    virtual const bool IsActionManagerName(const FString& Name) const;
    virtual const bool InitSettings(const TArray<FString>& Settings); //useless with new api version
    virtual const int8_t PerformAction(TArray<FString>& Action);
    virtual const int ActionToID(const FString& Action) const;
    virtual void Possess();
    virtual void UnPossess();


};