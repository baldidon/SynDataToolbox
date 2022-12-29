// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SensorIsar.h"
#include "LidarIsar.generated.h"

UCLASS(Blueprintable)
class ISARPLUGIN_API ALidarIsar : public AActor, public ISensorIsar
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALidarIsar();

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Observation")
		float TimeSampling = 0.0f; //if null, by default every tick Actor get relative observation

	UPROPERTY(EditAnywhere, Category = "Observation")
		bool enableIndependentTick = true;
	UPROPERTY(VisibleAnywhere, Category = "Observation")
		FString LastObservationTimestamp; //

	uint8* LastObservation;
	FCollisionQueryParams RV_TraceParams;

	UPROPERTY(EditAnywhere, Category = "LaserSettings")
		float StartAngleX = 0.0f;
	UPROPERTY(EditAnywhere, Category = "LaserSettings")
		float EndAngleX = 0.0f;
	UPROPERTY(EditAnywhere, Category = "LaserSettings")
		float DistanceAngleX = 0.0f;
	UPROPERTY(EditAnywhere, Category = "LaserSettings")
		float StartAngleY = 0.0f;
	UPROPERTY(EditAnywhere, Category = "LaserSettings")
		float EndAngleY = 0.0f;
	UPROPERTY(EditAnywhere, Category = "LaserSettings")
		float DistanceAngleY = 0.0f;
	UPROPERTY(EditAnywhere, Category = "LaserSettings")
		float LaserRange = 0.0f;
	UPROPERTY(EditAnywhere, Category = "LaserSettings")
		int Render = 0;
	UPROPERTY()
		bool Modified = false;

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
