// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "ActionManagerIsar.h"
#include "GameFramework/Actor.h"
#include "CoordinateActionManagerIsar.generated.h"


UCLASS()
class ISARPLUGIN_API ACoordinateActionManagerIsar : public AActor, public IActionManagerIsar
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACoordinateActionManagerIsar();


	virtual const FString GetActionManagerName() const override;
	virtual const int ActionToID(const FString& Action) const override;
	virtual const bool InitSettings(const TArray<FString>& Settings) override;
	virtual const int8_t PerformAction(TArray<FString>& ActionAndParameters) override;

	const int8_t MoveTo(TArray<FString> &Parameters);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	static const int UNKNOWN = -1;
	static const int MOVETO = 0;

	FHitResult* ActorHit;

	UPROPERTY()
		UStaticMeshComponent* Mesh;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};