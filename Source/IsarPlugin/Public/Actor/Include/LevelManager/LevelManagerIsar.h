#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "StructIsar.h"
#include "LevelManagerIsar.generated.h"


/*
This class is runtime fetched ad istanciated (in this case, when game instance runs) automatically
*/

UCLASS()
class ISARPLUGIN_API ULevelManagerIsar: public UGameInstanceSubsystem {

    GENERATED_BODY()


public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    void SetWorldReference(UWorld* World);
    void SetBaseMaterial(UMaterialInterface* Mat);
    int GetLevelMode();
    void SetLevelMode(int Mode);
    bool IsBusy();
    void SetObjectsToIdentify(TMap<FString, int> ObjectMap);
    TArray<AActor*> GetActorsToHide();
  
    UPROPERTY()
        TArray<FComponentsStructIsar> DefaultMaterials;
    UPROPERTY()
        TArray<FComponentsStructIsar> SegmentatedMaterials;


protected:
    bool bIsBusy;
    int LevelMode;
    UWorld* WorldPtr;
    bool ToLoadMeshesNewActor = false;
    void AppendNewActor(AActor* NewActor);
    void LoadMeshesNewActors();
    FString GetNameFromLabel(FString ActorLabel);
    void SetMaterials(int Mode);
    void FillActorList();
    void LoadSegmentatedMaterials(AActor* Actor);
    void LoadOriginalMaterials(AActor* Actor);
    TArray<AActor*> PendingActors;
    TMap<FString, int> ObjectToIdentify; //oppure un array di float. se array hanno una sola componente, grayscale
      
    TArray<AActor*> ActorList;
    TArray<AActor*> ActorsToHide;
    
    TMap<int, UMaterialInterface*> ClassToColorMap;
    UMaterialInterface* BaseMaterial;

};