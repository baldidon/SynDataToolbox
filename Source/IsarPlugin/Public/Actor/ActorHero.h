// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>
#include "Include/Sensors/CameraIsar.h"
#include "Include/Socket/UnrealSocketIsar.h"
#include "Include/Sensors/SensorIsar.h"
#include "Include/ActionManagers/ActionManagerIsar.h"
#include "Include/ResetManagers/ResetManagerIsar.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Include/LevelManager/ActionManagerController.h"
#include "EngineUtils.h"
#include "Components/SceneCaptureComponent2D.h"
#include "ActorHero.generated.h"




UCLASS()
class ISARPLUGIN_API AActorHero : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActorHero();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void FetchIsarObjects();


	static const int UNKNOWN = -1;
	static const int INIT = 0;
	static const int CHANGE = 1;
	static const int OBS = 2;
	static const int CLOSE = 3;
	static const int SENSORS = 4;
	static const int RENDER = 5;
	static const int ACTIONS = 6;
	static const int SETACTIONMAN = 7;
	static const int ACTION = 8;
	static const int RESETS = 9;
	static const int SETRESETMAN = 10;
	static const int RESET = 11;
	static const int FPS = 12;
	bool ShowFPS = false;
	int FPSCounter = 0;

	FTimerHandle TimerRepeatingFunction, TimerTCPConnectionListener;
	TSharedPtr<UnrealSocketIsar> HeroSocket;
	IActionManagerIsar* CurrentActionManager;
	IResetManagerIsar* CurrentResetManager;


	TArray<ISensorIsar*> SensorsList;
	TArray<IActionManagerIsar*> ActionManagersList;
	TArray<IResetManagerIsar*> ResetManagersList;

	UGameViewportClient* gameViewport;
	UStaticMeshComponent* Mesh;
	UInputComponent* InputComponent;
	APlayerController* PlayerController;

	int currentActionManagerIndex = -1;
	
	//if true, dev can choose which actionmanager use and move it around level
	UPROPERTY(EditAnywhere, Category = "ActionManager Isar")
		bool DebugActionManager = true;

	UPROPERTY(EditAnywhere, Category = "TopCategory")
		int port = 9734;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TopCategory")
	//	FString CarClassName = "Sedan";

	UPROPERTY(EditAnywhere, Category = "TopCategory")
		uint32 showRender = 0;

	//if true, all Isar objects depends to ActorHero
	UPROPERTY(EditAnywhere, Category = "Control")
		bool CentralizedControl = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void TCPConnectionListener();


	const bool CloseConnection();
	void CreateConnection();
	void RepeatingFunction();
	void ShowFrameRateOnScreen();
	const bool SendSensorsList();
	const bool SendActionManagersList();
	const bool SendResetManagersList();
	const bool SetCurrentActionManager(TArray<FString>& FieldArray);
	const bool SetCurrentResetManager(TArray<FString>& FieldArray);
	const bool ChangeSensorsSettings(TArray<FString>& FieldArray) const;
	void ParseCommand(FString PythonCommand);
	const bool GetSensorsObs(TArray<FString>& FieldArray);
	const bool PerformAction(TArray<FString>& FieldArray);
	const bool PerformReset(TArray<FString>& FieldArray);
	const int CommandToID(const FString& command) const;

};
