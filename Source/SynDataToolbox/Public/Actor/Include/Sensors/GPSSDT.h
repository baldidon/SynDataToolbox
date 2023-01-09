// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"
#include "Runtime/Core/Public/Misc/OutputDeviceNull.h" 
#include "SensorSDT.h"
#include "GPSSDT.generated.h"

UCLASS(Blueprintable)
class SYNDATATOOLBOX_API AGPSSDT : public AActor, public ISensorSDT
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGPSSDT();

	FVector StartLocation;
	FRotator StartRotation;
	bool bIsBusy = false;
	uint8* LastObservation;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh; //useful for move sensor all'over the level

	UPROPERTY(EditAnywhere, Category = "SensorProperties | Observation")
		bool enableIndependentTick = true; //

	UPROPERTY(EditAnywhere, Category = "SensorProperties | Observation")
		float TimeSampling = 0.0f; //
	
	UPROPERTY(VisibleAnywhere, Category = "SensorProperties | Observation")
		FString LastObservationTimestamp; //

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void TakePeriodicObs();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual const uint32 GetObservationSize() override;
	virtual const bool InitSensor() override;
	virtual const bool ChangeSensorSettings(const TArray<FString>& Settings) override;
	virtual const bool TakeObs() override;
	virtual const bool GetLastObs(uint8* Buffer) override;
	virtual const FString GetSensorName() override;

	const void SetTickMode(bool Value);

};
