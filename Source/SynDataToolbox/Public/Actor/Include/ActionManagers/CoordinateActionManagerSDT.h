// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "ActionManagerSDT.h"
#include "GameFramework/Actor.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include "CoordinateActionManagerSDT.generated.h"


UCLASS()
class SYNDATATOOLBOX_API ACoordinateActionManagerSDT : public APawn, public IActionManagerSDT
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACoordinateActionManagerSDT();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual const FString GetActionManagerName() const override;
	virtual const FString GetActionManagerSetup() const override;
	virtual const int ActionToID(const FString& Action) const override;
	virtual const bool InitSettings(const TArray<FString>& Settings) override;
	virtual const int8_t PerformAction(TArray<FString>& ActionAndParameters) override;
	virtual void Possess() override;
	virtual void UnPossess() override;

	const int8_t MoveTo(TArray<FString> &Parameters);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	static const int UNKNOWN = -1;
	static const int MOVETO = 0;

	FHitResult* ActorHit;

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