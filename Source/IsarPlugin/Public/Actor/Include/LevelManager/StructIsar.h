#pragma once

#include "StructIsar.generated.h"

USTRUCT(BlueprintType)
struct FStructIsar
{
	//TODO: Controllare inutile il campo MeshI?
	GENERATED_BODY()

	// default constructor -- without parameters, so it will work inside other structures 
	// that need default initializer, such as TArray
	FStructIsar():
		MeshI(nullptr),Materials(TArray<UMaterialInterface*>()){}

	UPROPERTY()
		UMeshComponent* MeshI;

	UPROPERTY()
		TArray<UMaterialInterface*> Materials;

	void Add(UMeshComponent* MeshIP, TArray<UMaterialInterface*> MaterialsP) {
		MeshI = MeshIP;
		Materials = MaterialsP;
	}

};

USTRUCT(BlueprintType)
struct FComponentsStructIsar
{
	GENERATED_BODY()

		FComponentsStructIsar() :
		MeshesMaterialsActor(TArray<FStructIsar>()) {}

	UPROPERTY()
		TArray<FStructIsar> MeshesMaterialsActor;

	void Add(FStructIsar aus) {
		MeshesMaterialsActor.Add(aus);
	}

};