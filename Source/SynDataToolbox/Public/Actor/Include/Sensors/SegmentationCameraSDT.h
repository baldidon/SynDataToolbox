// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "Runtime/Engine/Classes/Components/SceneCaptureComponent2D.h"
#include "SensorSDT.h"
#include "../LevelManager/LevelManagerSDT.h"
#include "SegmentationCameraSDT.generated.h"

/**
 * 
 */
UCLASS()
class SYNDATATOOLBOX_API ASegmentationCameraSDT : public ACameraActor, public ISensorSDT
{
	GENERATED_BODY()
	
	public:
		ASegmentationCameraSDT();

	protected:
		virtual void BeginPlay() override;
		void TakePeriodicObs();
		TMap<FString, int> ParseDictFromString(FString str);
		FString ClassesMapToString();
		ULevelManagerSDT* LevelManager;
		uint8* LastObservation;
		bool bIsBusy = false; // reader-Writer sync

	public:
		// Called every frame. From AActor
		virtual void Tick(float DeltaTime) override;

		//from SensorSDTInterface
		virtual const uint32 GetObservationSize() override;
		virtual const bool InitSensor() override;
		virtual const FString GetSensorSetup() override;
		virtual const bool ChangeSensorSettings(const TArray<FString>& Settings) override;
		virtual const bool GetLastObs(uint8* Buffer) override;
		virtual const bool TakeObs() override;
		virtual const FString GetSensorName() override; // This is the declaration of the implementation

		const void SetTickMode(bool Value); //Actor hero can sospend tick of this actor?


		UTextureRenderTarget2D* RenderTarget;
		
		UPROPERTY(VisibleAnywhere, Category="SensorProperties | Observation")
			USceneCaptureComponent2D* Camera;
		UPROPERTY(EditAnywhere, Category = "SensorProperties | Observation")
			bool enableIndependentTick = true; //
		UPROPERTY(EditAnywhere, Category = "SensorProperties | Observation")
			float TimeSampling = 0.0f; //if null, by default every tick Actor get relative observation
		UPROPERTY(VisibleAnywhere, Category = "SensorProperties | Observation")
			FString LastObservationTimestamp; //
		UPROPERTY() 
			TArray<AActor*> ActorsToHide;
		UPROPERTY()
			bool Modified = false;
		UPROPERTY(EditAnywhere, Category = "SensorProperties | Camera")
			uint32 Width = 64;
		UPROPERTY(EditAnywhere, Category = "SensorProperties | Camera")
			uint32 Height = 64;
		UPROPERTY(EditAnywhere, Category = "SensorProperties | Camera")
			int FOV = 90;
		UPROPERTY(EditAnywhere, Category = "SensorProperties | Classes")
			TMap<FString, int> Classes;
		UPROPERTY(VisibleAnywhere, Category = "SensorProperties | Classes")
			UMaterialInterface* BaseMaterial;
		UPROPERTY(EditAnywhere, Category = "SensorProperties | SegmentationType")
			bool bInstanceSegmentation = false; //if true, will be enabled instance segmentation. Only Class segmentation Otherwise
};
