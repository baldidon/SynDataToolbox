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
#include "ContinuousActionManagerIsar.generated.h"

UCLASS()
class ISARPLUGIN_API AContinuousActionManagerIsar : public APawn, public IActionManagerIsar
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AContinuousActionManagerIsar();


	virtual const FString GetActionManagerName() const override;
	virtual const int ActionToID(const FString& Action) const override;
	virtual const bool InitSettings(const TArray<FString>& Settings) override;
	virtual const int8_t PerformAction(TArray<FString>& Action) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Possess() override;
	virtual void UnPossess() override;

	const int8_t Turn(TArray<FString>& ActionSettings);
	void TurnCallback(float AxisValue);
	const int8_t GoStraight(TArray<FString>& ActionSettings);
	void StraightCallback(float AxisValue);
	const int8_t GoUp(TArray<FString>& ActionSettings);
	void UpCallback(float AxisValue);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	static const int UNKNOWN = -1;
	static const int TURN = 0;
	static const int STRAIGHT = 1;
	static const int GOUP = 2;

	FVector MovementVectorInput;

	FHitResult* ActorHit;

	UPROPERTY(EditAnywhere)
		USpringArmComponent* CameraSpringArm;

	UPROPERTY()
		UStaticMeshComponent* Mesh;

	//UPROPERTY(EditAnywhere, Category="Isar | Meshes")
		UStaticMesh* StaticMesh;
	
	UPROPERTY(EditAnywhere)
		UCameraComponent* CameraComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
