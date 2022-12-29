// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "Runtime/Engine/Classes/Components/SceneCaptureComponent2D.h"
#include "SensorIsar.h"
#include "../LevelManager/LevelManagerIsar.h"
#include "BoundingBoxDetectorIsar.generated.h"


UCLASS()
class ISARPLUGIN_API ABoundingBoxDetectorIsar : public ACameraActor, public ISensorIsar 
{
	GENERATED_BODY()

	public:
		ABoundingBoxDetectorIsar();
	
	protected:
		virtual void BeginPlay() override;
		void TakePeriodicObs();
		FString ClassesMapToString();
		bool bIsBusy = false; // reader-Writer sync
		uint8* LastObservation;
		ULevelManagerIsar* LevelManager;
		TMap<FString, int> ParseDictFromString(FString Str);
	
	public:
		// Called every frame. From AActor
		virtual void Tick(float DeltaTime) override;

		//from SensorIsarInterface
		virtual const uint32 GetObservationSize() override;
		virtual const bool InitSensor() override;
		virtual const FString GetSensorSetup() override;
		virtual const bool ChangeSensorSettings(const TArray<FString>& Settings) override;
		virtual const bool GetLastObs(uint8* Buffer) override;
		virtual const bool TakeObs() override;
		virtual const FString GetSensorName() override; // This is the declaration of the implementation

		const void SetTickMode(bool Value); //Actor hero can sospend tick of this actor?

		UTextureRenderTarget2D* RenderTarget;
		UMaterialInterface* BaseMaterial;

		UPROPERTY(EditAnywhere, Category = "SensorProperties")
			int MaxActors = 100;		//max actor per observation

		UPROPERTY()
			bool Modified = false;

		UPROPERTY(VisibleAnywhere, Category = "SensorProperties|Observation")
			USceneCaptureComponent2D* Camera;
		UPROPERTY(EditAnywhere, Category = "SensorProperties|Observation")
			bool enableIndependentTick = true; //
		UPROPERTY(EditAnywhere, Category = "SensorProperties|Observation")
			float TimeSampling = 0.0f; //if null, by default every tick Actor get relative observation
		UPROPERTY(VisibleAnywhere, Category = "SensorProperties|Observation")
			FString LastObservationTimestamp; //
		UPROPERTY(EditAnywhere, Category = "SensorProperties | Camera")
			uint32 Width = 64;
		UPROPERTY(EditAnywhere, Category = "SensorProperties | Camera")
			uint32 Height = 64;
		UPROPERTY(EditAnywhere, Category = "SensorProperties | Camera")
			int FOV = 90;
		//UPROPERTY(EditAnywhere, Category = "SensorProperties | Classes")
		//	int LimitBoxesPerObservation = 10;
		UPROPERTY(EditAnywhere, Category = "SensorProperties | Classes")
			TMap<FString,int> ClassesToBound;
		UPROPERTY()
			TArray<AActor*> ActorsToHide;
		
		UPROPERTY()
			TMap<int, FBoundingBoxCollectionIsar> BoundingBoxes;
};