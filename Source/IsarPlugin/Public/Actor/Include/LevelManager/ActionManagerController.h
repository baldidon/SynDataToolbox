// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include <Actor/Include/ActionManagers/ActionManagerIsar.h>
#include "ActionManagerController.generated.h"

UCLASS()
class ISARPLUGIN_API AActionManagerController : public APlayerController
{
	GENERATED_BODY()


public:
	// ~Overrides: APlayerController
	virtual void SetupInputComponent() override;
	//virtual void BeginPlay() override;
	//fetchActionManagers

	//callback functions
	void SwitchUp();
	void SwitchDown();
	void RestoreDefaultPawn();
	void SetupActorListToPossess(TArray<IActionManagerIsar*> ActionManagerList);

protected:
	int IndexCurrentActionManager = -1;
	TArray<IActionManagerIsar*> ActionManagers;
	APawn* DefaultPawn;
};