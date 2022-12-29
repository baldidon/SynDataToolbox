// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "Runtime/Engine/Classes/Components/SceneCaptureComponent2D.h"
#include "SensorIsar.h"
#include "../LevelManager/LevelManagerIsar.h"
#include "PixelFormat.h"
#include "Camera/CameraActor.h"
#include "CameraIsar.generated.h"


UCLASS()
class ISARPLUGIN_API ACameraIsar: public ACameraActor, public ISensorIsar
{
	GENERATED_BODY()
	
public:	
	ACameraIsar();

protected:
	virtual void BeginPlay() override;
	void TakePeriodicObs();

public:	
	// Called every frame. From AActor
	virtual void Tick(float DeltaTime) override;

	//from SensorIsarInterface
	virtual const uint32 GetObservationSize() override;
	virtual const bool InitSensor() override;
	virtual const bool ChangeSensorSettings(const TArray<FString>& Settings) override;
	virtual const bool GetLastObs(uint8* Buffer) override;
	virtual const bool TakeObs() override;
	virtual const FString GetSensorSetup() override;
	virtual const FString GetSensorName() override; // This is the declaration of the implementation


	const void SetTickMode(bool Value); //Actor hero can sospend tick of this actor?
	//const void SetLevelManager(LevelManager* Manager);

	//FString ContentDir;
	ULevelManagerIsar* LevelManager;
	UPROPERTY() TArray<AActor*> ActorsToHide;
	uint8* LastObservation;
	
	bool bIsBusy = false; // reader-Writer sync
	UTextureRenderTarget2D* RenderTarget = nullptr;

	UPROPERTY(EditAnywhere,Category="SensorProperties|Observation")
		USceneCaptureComponent2D* Camera;
	UPROPERTY(EditAnywhere, Category = "SensorProperties|Observation")
		bool enableIndependentTick = true; //
	UPROPERTY() 
		UStaticMeshComponent* Mesh;
	UPROPERTY()
		bool Modified = false;
	UPROPERTY(EditAnywhere, Category = "SensorProperties|Camera")
		uint32 Width = 128;
	UPROPERTY(EditAnywhere, Category = "SensorProperties|Camera")
		uint32 Height = 128;
	UPROPERTY(EditAnywhere, Category = "SensorProperties|Camera")
		int FOV = 90;
	UPROPERTY(EditAnywhere, Category = "SensorProperties|Observation")
		float TimeSampling = 5.0f; //if null, by default every tick Actor get relative observation
	UPROPERTY(VisibleAnywhere, Category = "SensorProperties|Observation")
		FString LastObservationTimestamp; //
};
