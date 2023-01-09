// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SensorSDT.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "VisibilitySDT.generated.h"

UCLASS(Blueprintable)
class SYNDATATOOLBOX_API AVisibilitySDT : public AActor, public ISensorSDT
{
	GENERATED_BODY()

public:
	AVisibilitySDT();

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "SensorProperties | Observation")
		float TimeSampling = 0.0f; //if null, by default every tick Actor get relative observation

	UPROPERTY(EditAnywhere, Category = "SensorProperties | Observation")
		bool enableIndependentTick = true;

	UPROPERTY(VisibleAnywhere, Category = "SensorProperties | Observation")
		FString LastObservationTimestamp; //

	uint8* LastObservation;

	FCollisionQueryParams RV_TraceParams;
	FVector StartLocation;
	FRotator StartRotation;

	UPROPERTY(EditAnywhere, Category = "SensorProperties | LaserSettings")
		float StartAngleX = 0.0f;
	UPROPERTY(EditAnywhere, Category = "SensorProperties | LaserSettings")
		float EndAngleX = 0.0f;
	UPROPERTY(EditAnywhere, Category = "SensorProperties | LaserSettings")
		float DistanceAngleX = 0.0f;
	UPROPERTY(EditAnywhere, Category = "SensorProperties | LaserSettings")
		float StartAngleY = 0.0f;
	UPROPERTY(EditAnywhere, Category = "SensorProperties | LaserSettings")
		float EndAngleY = 0.0f;
	UPROPERTY(EditAnywhere, Category = "SensorProperties | LaserSettings")
		float DistanceAngleY = 0.0f;
	UPROPERTY(EditAnywhere, Category = "SensorProperties | LaserSettings")
		float LaserRange = 0.0f;
	UPROPERTY(EditAnywhere, Category = "SensorProperties | LaserSettings")
		int Render = 0;
	UPROPERTY(EditAnywhere, Category = "SensorProperties | LaserSettings")
		int CellsRange = 0;
	bool bIsBusy = false; // reader-Writer sync


	virtual const uint32 GetObservationSize() override;
	virtual const bool InitSensor() override;
	virtual const FString GetSensorSetup() override;
	virtual const bool ChangeSensorSettings(const TArray<FString>& Settings) override;
	virtual const bool GetLastObs(uint8* Buffer) override;
	virtual const bool TakeObs() override;
	virtual const FString GetSensorName() override; // This is the declaration of the implementation

	const void SetTickMode(bool Value);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void TakePeriodicObs();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};