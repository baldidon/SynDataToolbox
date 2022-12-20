#include "Actor/Include/LevelManager/LevelManagerIsar.h"

void ULevelManagerIsar::Initialize(FSubsystemCollectionBase& Collection)
{
	bIsBusy = false;
	LevelMode = 0;
	ActorList =  TArray<AActor*>();
	ObjectToIdentify = TMap<FString, int>();
	ActorsToHide = TArray<AActor*>();
}


void ULevelManagerIsar::SetWorldReference(UWorld* World)
{
	//create an spawn handler for manage new Actor's meshes
	WorldPtr = World;
	FOnActorSpawned::FDelegate ActorSpawnedDelegate = FOnActorSpawned::FDelegate::CreateUObject(this, &ULevelManagerIsar::AppendNewActor);
	WorldPtr->AddOnActorSpawnedHandler(ActorSpawnedDelegate);
}

void ULevelManagerIsar::SetBaseMaterial(UMaterialInterface* Mat)
{
	// if set something, inizio a popolare le "mappe"
	BaseMaterial = Mat;
}

int ULevelManagerIsar::GetLevelMode()
{
	return LevelMode;
}

void ULevelManagerIsar::SetLevelMode(int Mode)
{
	LevelMode = Mode;
	SetMaterials(LevelMode);
}

bool ULevelManagerIsar::IsBusy()
{
	return bIsBusy;
}

void ULevelManagerIsar::SetObjectsToIdentify(TMap<FString, int> ObjectMap)
{
	ObjectToIdentify = ObjectMap;
	//fill ClassToColorMap
	for (const TPair<FString, int>& pair : ObjectToIdentify)
	{
		UMaterialInstanceDynamic* NewDynMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, NULL);
		NewDynMaterial->SetVectorParameterValue("Color", 
			FColor::FColor((uint8)pair.Value, 0, 0, 0));
		ClassToColorMap.Add(pair.Value, NewDynMaterial);
	}
	FillActorList();
	int ciao = 3;
}

TArray<AActor*> ULevelManagerIsar::GetActorsToHide()
{
	return ActorsToHide;
}

void ULevelManagerIsar::AppendNewActor(AActor* NewActor)
{
	if (!ActorList.IsEmpty()) {
		while (bIsBusy) { UE_LOG(LogTemp, Error, TEXT("Waiting")) }
		bIsBusy = false;
		TArray<UMeshComponent*> PaintableComponents;
		NewActor->GetComponents<UMeshComponent>(PaintableComponents);
		if (PaintableComponents.Num() > 0 && !NewActor->GetName().Contains("Isar")) {
			ToLoadMeshesNewActor = true;
			PendingActors.Add(NewActor);
		}
		bIsBusy = false;
	}
}

void ULevelManagerIsar::LoadMeshesNewActors()
{
	//TODO: REFACTORING

	for (AActor* ActorI : PendingActors) {
		ActorList.Add(ActorI);
		FString ClassNameI = GetNameFromLabel(ActorI->GetActorLabel());
		int* ClassIndexPtr = ObjectToIdentify.Find(ClassNameI);
		int ClassIndex;
		if (ClassIndexPtr == nullptr) {
			ClassIndex = 0; //if nullptr will be "others" class
		}
		else { ClassIndex = *ClassIndexPtr; }
		LoadOriginalMaterials(ActorI);
		LoadSegmentatedMaterials(ActorI);
	}
	//flush array for new added Actors
	PendingActors.Empty();
	ToLoadMeshesNewActor = false;
}

FString ULevelManagerIsar::GetNameFromLabel(FString ActorLabel)
{
	TArray<FString> Words;
	ActorLabel.FString::ParseIntoArray(Words, TEXT("_"), false);
	return Words[0];
}

void ULevelManagerIsar::FillActorList()
{
	for (TActorIterator<AActor> ActorIt(WorldPtr); ActorIt; ++ActorIt)
	{
		TArray<UMeshComponent*> PaintableComponents;
		ActorIt->GetComponents<UMeshComponent>(PaintableComponents);
		FString ClassName = ActorIt->GetName();
		if (!PaintableComponents.IsEmpty() && !ClassName.Contains("ActorHero")) {
			if (ClassName.Contains("Isar") || ClassName.Contains("Pawn")){
				ActorsToHide.Add(*ActorIt); //for Cameras
				continue;
			}
			ActorList.Add(*ActorIt);
			LoadOriginalMaterials(*ActorIt);
			LoadSegmentatedMaterials(*ActorIt);
		}
	}
}

void ULevelManagerIsar::SetMaterials(int Mode)
{
	bIsBusy = true;
	if (ToLoadMeshesNewActor && PendingActors.Num() > 0) {
		LoadMeshesNewActors();
	}
	for (int i = 0; i < ActorList.Num(); i++)
	{
		AActor* ActorIt = ActorList[i];
		FComponentsStructIsar MaterialMeshes;

		if (Mode == 1) {
			MaterialMeshes = SegmentatedMaterials[i]; 	//Set Segmentation materials
		}
		else {
			MaterialMeshes = DefaultMaterials[i]; //Set original materials
		}
		for (FStructIsar MatMesh : MaterialMeshes.MeshesMaterialsActor) {
			for (int j = 0; j < MatMesh.Materials.Num(); j++) {
				MatMesh.MeshI->SetMaterial(j, MatMesh.Materials[j]);
			}
		}
	}
	bIsBusy = false;
}

void ULevelManagerIsar::LoadOriginalMaterials(AActor* Actor)
{
	TArray<UMeshComponent*> PaintableMeshes;
	Actor->GetComponents<UMeshComponent>(PaintableMeshes);
	FComponentsStructIsar ComponentsActorI; //iesima "struct" contenente un array di "coppie" mesh-materials
	for (UMeshComponent* MeshI : PaintableMeshes) 
	{
		FStructIsar Aux; //coppia mesh-materials
		TArray<UMaterialInterface*> Materials = MeshI->GetMaterials();
		Aux.Add(MeshI, Materials);
		ComponentsActorI.Add(Aux);
	}
	DefaultMaterials.Add(ComponentsActorI);
}

void ULevelManagerIsar::LoadSegmentatedMaterials(AActor* Actor)
{
	FString ClassNameCurrentActor = ULevelManagerIsar::GetNameFromLabel(Actor->GetActorLabel());
	int* ClassIndexPtr = ObjectToIdentify.Find(ClassNameCurrentActor);
	int ClassIndex;
	if (ClassIndexPtr == nullptr){
		ClassIndex = 0; //if nullptr will be "others" class
	}else{
		ClassIndex = *ClassIndexPtr;
	}

	TArray<UMeshComponent*> PaintableMeshes;
	Actor->GetComponents<UMeshComponent>(PaintableMeshes);
	FComponentsStructIsar MeshesMaterialsActorI;
	for (UMeshComponent* MeshI : PaintableMeshes)
	{
		FStructIsar Aux;
		TArray<UMaterialInterface*> Materials = MeshI->GetMaterials();
		for (int j = 0; j < Materials.Num(); j++) {
			Materials[j] = *ClassToColorMap.Find(ClassIndex);
		}
		Aux.Add(MeshI, Materials);
		MeshesMaterialsActorI.Add(Aux);
	}
	SegmentatedMaterials.Add(MeshesMaterialsActorI);
}
