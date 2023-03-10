// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "ActionManagerSDT.h"
#include "GameFramework/Actor.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include "DiscreteActionManagerSDT.generated.h"


UCLASS()
class SYNDATATOOLBOX_API ADiscreteActionManagerSDT : public APawn, public IActionManagerSDT
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADiscreteActionManagerSDT();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	virtual const FString GetActionManagerName() const override;
	virtual const FString GetActionManagerSetup() const override;
	virtual const int ActionToID(const FString& Action) const override;
	virtual const bool InitSettings(const TArray<FString>& Settings) override;
	virtual const int8_t PerformAction(TArray<FString>& Action) override;
	virtual void Possess() override;
	virtual void UnPossess() override;


	const int8_t GoForward();
	void GoForwardCallback();
	const int8_t GoBackward();
	void GoBackwardCallback();
	const int8_t TurnRight();
	void TurnRightCallback();
	const int8_t TurnLeft();
	void TurnLeftCallback();
	const int8_t TurnRightGoForward();
	void TurnRightGoForwardCallback();
	const int8_t TurnLeftGoForward();
	void TurnLeftGoForwardCallback();
	const int8_t TurnRightGoBackward();
	void TurnRightGoBackwardCallback();
	const int8_t TurnLeftGoBackward();
	void TurnLeftGoBackwardCallback();

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
	
	UPROPERTY(EditAnywhere, Category = "Isar | DiscreteStep")
		float Speed = 50.0f;
	
	UPROPERTY(EditAnywhere, Category = "Isar | DiscreteStep")
		float RotationSpeed = 50.0f;

	USpringArmComponent* CameraSpringArm;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UCameraComponent* CameraComponent;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UStaticMeshComponent* Mesh;

//UPROPERTY(EditAnywhere, Category = "Isar | Meshes")
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UStaticMesh* StaticMesh;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};