#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "StructSDT.h"
#include "LevelManagerSDT.generated.h"


/*
* This class is runtime fetched ad istanciated (in this case, when game instance runs) automatically
*/

UCLASS()
class SYNDATATOOLBOX_API ULevelManagerSDT: public UGameInstanceSubsystem {

    GENERATED_BODY()


public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    void SetWorldReference(UWorld* World);
    void SetBaseMaterial(UMaterialInterface* Mat);
    int GetLevelMode();
    void SetLevelMode(int Mode);
    bool IsBusy();
    void SetSegmentation(bool Modality, bool SegmentationType);
    uint32 GetActorListLength();
    void SetBoundingBox(bool Modality);
    bool IsMaterialDefined();
    bool IsWorldReferenceDefined();
    void SetSegmentationObjectsToIdentify(TMap<FString, int> ObjectMap);
    void SetBoundingBoxObjectsToIdentify(TMap<FString, int> ObjectMap);
    TArray<AActor*> GetActorsToHide();
  
    UPROPERTY()
        TArray<FComponentsStructSDT> DefaultMaterials;
    UPROPERTY()
        TArray<FComponentsStructSDT> SegmentatedMaterials;
    UPROPERTY()
        TArray<FComponentsStructSDT> BoundingMaterials;

protected:
    bool bIsBusy;
    bool bSegmentationMode;
    bool bBoundingBox;
    bool bInstanceSegmentation;
    int LevelMode;    /*
    Level Mode can be:
        0: DefaultWorld
        1: SegmentatedWorld
        2: BoundedWorld
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

    TMap<int, int> BoundingClassInstanceCounter;
    TMap<int, int> SegmentationClassInstanceCounter;
};