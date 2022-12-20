// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "ActionManagerIsar.h"
#include "GameFramework/Actor.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include "DiscreteActionManagerIsar.generated.h"


UCLASS()
class ISARPLUGIN_API ADiscreteActionManagerIsar : public APawn, public IActionManagerIsar
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADiscreteActionManagerIsar();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	virtual const FString GetActionManagerName() const override;
	virtual const int ActionToID(const FString& Action) const override;
	virtual const bool InitSettings(const TArray<FString>& Settings) override;
	virtual const int8_t PerformAction(TArray<FString>& Action) override;
	virtual void Possess() override;
	virtual void UnPossess() override;




	const int8_t GoForward();
	void GoForwardCallback(float AxisValue);
	const int8_t GoBackward();
	void GoBackwardCallback(float AxisValue);
	const int8_t TurnRight();
	void TurnRightCallback(float AxisValue);
	const int8_t TurnLeft();
	void TurnLeftCallback(float AxisValue);
	const int8_t TurnRightGoForward();
	void TurnRightGoForwardCallback(float AxisValue);
	const int8_t TurnLeftGoForward();
	void TurnLeftGoForwardCallback(float AxisValue);
	const int8_t TurnRightGoBackward();
	void TurnRightGoBackwardCallback(float AxisValue);
	const int8_t TurnLeftGoBackward();
	void TurnLeftGoBackwardCallback(float AxisValue);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	static const int UNKNOWN = -1;
	static const int TURNL = 0;
	static const int TURNR = 1;
	static const int FORWARD = 2;
	static const int TURNLFORWARD = 3;
	static const int TURNRFORWARD = 4;
	static const int TURNLBACKWARD = 5;
	static const int TURNRBACKWARD = 6;
	static const int BACKWARD = 7;
	static const int IDLE = 8;

	FHitResult* ActorHit;
	
	
	UPROPERTY()
		UStaticMeshComponent* Mesh;
	
	//UPROPERTY(EditAnywhere, Category = "Isar | Meshes")
	UStaticMesh* StaticMesh;

	UPROPERTY(EditAnywhere, Category = "Isar | DiscreteStep")
		float Speed = 50.0f;
	
	UPROPERTY(EditAnywhere, Category = "Isar | DiscreteStep")
		float RotationSpeed = 50.0f;

	UPROPERTY(EditAnywhere)
		USpringArmComponent* CameraSpringArm;


	UPROPERTY(EditAnywhere)
		UCameraComponent* CameraComponent;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};