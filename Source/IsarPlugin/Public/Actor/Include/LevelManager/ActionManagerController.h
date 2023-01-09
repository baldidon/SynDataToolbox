// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include <Actor/Include/ActionManagers/ActionManagerSDT.h>
#include "ActionManagerController.generated.h"

UCLASS()
class SYNDATATOOLBOX_API AActionManagerController : public APlayerController
{
	GENERATED_BODY()


public:
	// ~Overrides: APlayerController
	virtual void SetupInputComponent() override;

	//callback functions
	void SwitchUp();
	void SwitchDown();
	void RestoreDefaultPawn();
	void SetupActorListToPossess(TArray<IActionManagerSDT*> ActionManagerList);

protected:
	int IndexCurrentActionManager = -1;
	TArray<IActionManagerSDT*> ActionManagers;
	APawn* DefaultPawn;
};