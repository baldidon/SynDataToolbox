// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "ActionManagerIsar.h"
#include "DroneActionManagerIsar.generated.h"


UCLASS()
class ISARPLUGIN_API ADroneActionManagerIsar : public APawn, public IActionManagerIsar
{
	GENERATED_BODY()

public:
	//virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	// Sets default values for this actor's properties
	ADroneActionManagerIsar();



	virtual const FString GetActionManagerName() const override;
	virtual const int ActionToID(const FString& Action) const override;
	virtual const bool InitSettings(const TArray<FString>& Settings) override;
	virtual const int8_t PerformAction(TArray<FString>& Action) override;
	
	virtual void Possess() override;
	virtual void UnPossess() override;

	const int8_t Throttle(TArray<FString>& ActionSettings) const;
	//void ThrottleCallback(float Power);
	const int8_t Pitch(TArray<FString>& ActionSettings) const;
	//void PitchCallback(float PitchValue);
	const int8_t Yaw(TArray<FString>& ActionSettings) const;
	//void YawCallback(float YawValue);
	const int8_t Roll(TArray<FString>& ActionSettings) const;
	//void RollCallback(float RollValue);

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
	//AActor* Drone;
	UBlueprint* GeneratedBP;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};