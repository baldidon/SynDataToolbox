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

USTRUCT(BlueprintType)
struct FBoundingBoxCollectionIsar
{
	GENERATED_BODY()

	FBoundingBoxCollectionIsar() : 
		BoundingBoxCoordinates(TMap<uint8, FVector4>()), Indexes(TArray<uint8>()){}

	//FVector contains float values, but in this case will be always integer values
	UPROPERTY()
		TMap<uint8,FVector4> BoundingBoxCoordinates;
	UPROPERTY()
		TArray<uint8> Indexes;
	
	void Add(uint8 GreenIndex, FVector4 Coordinates) {
		BoundingBoxCoordinates.Add(GreenIndex, Coordinates);
		Indexes.Add(GreenIndex);
	}
	
	float GetX1(uint8 GreenIndex) {
		return (BoundingBoxCoordinates.Find(GreenIndex))->X;
	}
	
	float GetY1(uint8 GreenIndex) {
		return (BoundingBoxCoordinates.Find(GreenIndex))->Y;
	}

	float GetX2(uint8 GreenIndex) {
		return (BoundingBoxCoordinates.Find(GreenIndex))->Z;
	}
	
	float GetY2(uint8 GreenIndex) {
		return (BoundingBoxCoordinates.Find(GreenIndex))->W;
	}
	
	void UpdateX1(float X1, uint8 GreenIndex) {
		FVector4* Vect = BoundingBoxCoordinates.Find(GreenIndex);
		Vect->Set(X1, Vect->Y, Vect->Z, Vect->W);

	}

	void UpdateX2(float X2, uint8 GreenIndex) {
		FVector4* Vect= BoundingBoxCoordinates.Find(GreenIndex);
		Vect->Set(Vect->X, Vect->Y, X2, Vect->W);
	}

	void UpdateY1(float Y1, uint8 GreenIndex) {
		FVector4* Vect = BoundingBoxCoordinates.Find(GreenIndex);
		Vect->Set(Vect->X, Y1, Vect->Z, Vect->W);
	}

	void UpdateY2(float Y2, uint8 GreenIndex) {
		FVector4* Vect = BoundingBoxCoordinates.Find(GreenIndex);
		Vect->Set(Vect->X, Vect->Y, Vect->Z, Y2);
	}

	FVector4* GetCoordinates(uint8 GreenIndex) {
		return BoundingBoxCoordinates.Find(GreenIndex);
	}

	TArray<uint8> GetIndexes() {
		return Indexes;
	}

	void Clean() {
		BoundingBoxCoordinates.Empty();
		Indexes.Empty();
	}

};