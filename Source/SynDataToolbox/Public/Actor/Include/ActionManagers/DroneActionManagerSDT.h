// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "ActionManagerSDT.h"
#include "DroneActionManagerSDT.generated.h"


UCLASS()
class SYNDATATOOLBOX_API ADroneActionManagerSDT : public APawn, public IActionManagerSDT
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADroneActionManagerSDT();


	virtual const FString GetActionManagerName() const override;
	virtual const FString GetActionManagerSetup() const override;
	virtual const int ActionToID(const FString& Action) const override;
	virtual const bool InitSettings(const TArray<FString>& Settings) override;
	virtual const int8_t PerformAction(TArray<FString>& Action) override;
	
	virtual void Possess() override;
	virtual void UnPossess() override;

	const int8_t Throttle(TArray<FString>& ActionSettings) const;
	const int8_t Pitch(TArray<FString>& ActionSettings) const;
	const int8_t Yaw(TArray<FString>& ActionSettings) const;
	const int8_t Roll(TArray<FString>& ActionSettings) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	static const int UNKNOWN = -1;
	static const int THROTTLE = 0;
	static const int PITCH = 1;
	static const int YAW = 2;
	static const int ROLL = 3;

	FHitResult* ActorHit;

	UPROPERTY()
		UStaticMeshComponent* Mesh;
	
	TSubclassOf<APawn> PawnToSpawn;
	APawn* Drone;
	UBlueprint* GeneratedBP;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};