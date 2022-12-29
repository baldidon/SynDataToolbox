#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "StructIsar.h"
#include "LevelManagerIsar.generated.h"


/*
* This class is runtime fetched ad istanciated (in this case, when game instance runs) automatically
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
    void SetSegmentation(bool Modality);
    uint32 GetActorListLength();
    void SetBoundingBox(bool Modality);
    bool IsMaterialDefined();
    bool IsWorldReferenceDefined();
    void SetSegmentationObjectsToIdentify(TMap<FString, int> ObjectMap);
    void SetBoundingBoxObjectsToIdentify(TMap<FString, int> ObjectMap);
    TArray<AActor*> GetActorsToHide();
  
    UPROPERTY()
        TArray<FComponentsStructIsar> DefaultMaterials;
    UPROPERTY()
        TArray<FComponentsStructIsar> SegmentatedMaterials;
    UPROPERTY()
        TArray<FComponentsStructIsar> BoundingMaterials;

protected:
    bool bIsBusy;
    bool bSegmentationMode;
    bool bBoundingBox;
    int LevelMode;
   
    /*
    Level Mode can be:
        0: DefaultWorld
        1: SegmentatedWorld
        2: BoundedWorld
    */

    /*
    * 
    */

    UWorld* WorldPtr;
    FString GetNameFromLabel(FString ActorLabel);
    bool ToLoadMeshesNewActor = false;
    void AppendNewActor(AActor* NewActor);
    void LoadMeshesNewActors();
    void SetMaterials(int Mode);
    void FillActorList();
    void PrepareSegmentatedMaterials();
    void PrepareBoundingBoxMaterials();
    void LoadSegmentatedMaterials(AActor* Actor);
    void LoadBoundingMaterials(AActor* Actor);
    void LoadOriginalMaterials(AActor* Actor);



    TArray<AActor*> PendingActors;
    TArray<AActor*> BoundingPendingActors;  //could be unnecessary

    TMap<FString, int> ObjectToIdentify; 
    TMap<FString, int> BoundingObjectToIdentify;
     
    TArray<AActor*> ActorList;
    TArray<AActor*> ActorsToHide;
    
    TMap<int, UMaterialInterface*> ClassToColorMap;
    TMap<int, UMaterialInterface*> BoundingClassToColorMap;

    UMaterialInterface* BaseMaterial;
    //UMaterialInterface* BoundingBaseMaterial;

    TMap<int, int> BoundingCounterOccurrency;
};