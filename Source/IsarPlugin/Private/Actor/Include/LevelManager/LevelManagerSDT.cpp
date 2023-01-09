#include "Actor/Include/LevelManager/LevelManagerSDT.h"

void ULevelManagerSDT::Initialize(FSubsystemCollectionBase& Collection)
{
	bIsBusy = false;
	bSegmentationMode = false;
	bBoundingBox = false;
	LevelMode = 0;
	ActorList =  TArray<AActor*>();
	ObjectToIdentify = TMap<FString, int>();
	BoundingObjectToIdentify = TMap<FString, int>();
	ActorsToHide = TArray<AActor*>();
}


void ULevelManagerSDT::SetWorldReference(UWorld* World)
{
	//create an spawn handler for manage new Actor's meshes
	WorldPtr = World;
	FOnActorSpawned::FDelegate ActorSpawnedDelegate = FOnActorSpawned::FDelegate::CreateUObject(this, &ULevelManagerSDT::AppendNewActor);
	WorldPtr->AddOnActorSpawnedHandler(ActorSpawnedDelegate);
}

void ULevelManagerSDT::SetBaseMaterial(UMaterialInterface* Mat)
{
	// if set something, inizio a popolare le "mappe"
	BaseMaterial = Mat;
}

int ULevelManagerSDT::GetLevelMode()
{
	return LevelMode;
}

void ULevelManagerSDT::SetLevelMode(int Mode)
{
	if (Mode == 1 || Mode == 2 || Mode == 0) {
		LevelMode = Mode;
		SetMaterials(LevelMode);
	}
}

bool ULevelManagerSDT::IsBusy()
{
	return bIsBusy;
}

void ULevelManagerSDT::SetSegmentationObjectsToIdentify(TMap<FString, int> ObjectMap)
{
	bIsBusy = true;
	if (!ObjectToIdentify.IsEmpty()) {
		ObjectToIdentify.Empty();
		ClassToColorMap.Empty();
		SegmentatedMaterials.Empty();
		if (bInstanceSegmentation) {
			SegmentationClassInstanceCounter.Empty();
		}
	}
	ObjectToIdentify = ObjectMap;
	if (bInstanceSegmentation) {
		SegmentationClassInstanceCounter = TMap<int, int>();
	}
	//fill ClassToColorMap
	for (const TPair<FString, int>& pair : ObjectToIdentify)
	{
		UMaterialInstanceDynamic* NewDynMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, NULL);

		NewDynMaterial->SetVectorParameterValue("Color", 
			FColor::FColor((uint8)pair.Value, 0, 0, 0));
		ClassToColorMap.Add(pair.Value, NewDynMaterial);
		if (bInstanceSegmentation && pair.Value != 0) { //useless for "others" class
			SegmentationClassInstanceCounter.Add(pair.Value, 0);
		}
	}
	if (ActorList.IsEmpty()) {
		FillActorList();
	}
	//Prepare Segmentated Meshes
	PrepareSegmentatedMaterials();
	bIsBusy = false;
}

void ULevelManagerSDT::SetBoundingBoxObjectsToIdentify(TMap<FString, int> ObjectMap)
{
	bIsBusy = true;
	if (!BoundingObjectToIdentify.IsEmpty()) {
		BoundingObjectToIdentify.Empty();
		BoundingClassToColorMap.Empty();
		BoundingMaterials.Empty();
		BoundingClassInstanceCounter.Empty();
	}

	BoundingObjectToIdentify = ObjectMap;
	BoundingClassInstanceCounter = TMap<int, int>();
	//fill ClassToColorMap
	for (const TPair<FString, int>& pair : BoundingObjectToIdentify)
	{
		UMaterialInstanceDynamic* NewDynMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, NULL);
		NewDynMaterial->SetVectorParameterValue("Color",
			FColor::FColor((uint8)pair.Value, 0, 0, 0));
		BoundingClassToColorMap.Add(pair.Value, NewDynMaterial);
		if (pair.Value != 0) { //useless for other class
			BoundingClassInstanceCounter.Add(pair.Value, 0);
		}
	}
	if (ActorList.IsEmpty()) {
		FillActorList();
	}
	//Prepare BoundingBox Meshes
	PrepareBoundingBoxMaterials();
	bIsBusy = false;
}

void ULevelManagerSDT::PrepareSegmentatedMaterials()
{
	for (AActor* Actor : ActorList) {
		LoadSegmentatedMaterials(Actor);
	}
}

void ULevelManagerSDT::PrepareBoundingBoxMaterials()
{
	for (AActor* Actor : ActorList) {
		LoadBoundingMaterials(Actor);
	}
}

TArray<AActor*> ULevelManagerSDT::GetActorsToHide()
{
	return ActorsToHide;
}

void ULevelManagerSDT::AppendNewActor(AActor* NewActor)
{
	if (!ActorList.IsEmpty()) 
	{
		while (bIsBusy) { UE_LOG(LogTemp, Error, TEXT("Waiting")) }
		bIsBusy = false;
		TArray<UMeshComponent*> PaintableComponents;
		NewActor->GetComponents<UMeshComponent>(PaintableComponents);
		if (PaintableComponents.Num() > 0 && !NewActor->GetName().Contains("Isar")) 
		{
			ToLoadMeshesNewActor = true;
			PendingActors.Add(NewActor);
		}
		bIsBusy = false;
	}
}

void ULevelManagerSDT::LoadMeshesNewActors()
{
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
		if(bSegmentationMode)
			LoadSegmentatedMaterials(ActorI);
		if(bBoundingBox) {
			LoadBoundingMaterials(ActorI);
		}
	}
	//flush array for new added Actors
	PendingActors.Empty();
	ToLoadMeshesNewActor = false;
}

FString ULevelManagerSDT::GetNameFromLabel(FString ActorLabel)
{
	TArray<FString> Words;
	ActorLabel.FString::ParseIntoArray(Words, TEXT("_"), false);
	return Words[0];
}

void ULevelManagerSDT::FillActorList()
{
	bIsBusy = true;
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
		}
	}
	bIsBusy = false;
}

void ULevelManagerSDT::SetMaterials(int Mode)
{
	bIsBusy = true;
	if (ToLoadMeshesNewActor && PendingActors.Num() > 0) {
		LoadMeshesNewActors();
	}
	for (int i = 0; i < ActorList.Num(); i++)
	{
		AActor* ActorIt = ActorList[i];
		FComponentsStructSDT MaterialMeshes;

		if (Mode == 1) {
			MaterialMeshes = SegmentatedMaterials[i]; 	//Set Segmentation materials
		}
		else if(Mode==0){
			MaterialMeshes = DefaultMaterials[i]; //Set original materials
		}
		else {
			MaterialMeshes = BoundingMaterials[i];
		}

		for (FStructSDT MatMesh : MaterialMeshes.MeshesMaterialsActor) {
			for (int j = 0; j < MatMesh.Materials.Num(); j++) {
				MatMesh.MeshI->SetMaterial(j, MatMesh.Materials[j]);
			}
		}

	}
	bIsBusy = false;
}

void ULevelManagerSDT::LoadOriginalMaterials(AActor* Actor)
{
	TArray<UMeshComponent*> PaintableMeshes;
	Actor->GetComponents<UMeshComponent>(PaintableMeshes);
	FComponentsStructSDT ComponentsActorI; //iesima "struct" contenente un array di "coppie" mesh-materials
	for (UMeshComponent* MeshI : PaintableMeshes) 
	{
		FStructSDT Aux; //coppia mesh-materials
		TArray<UMaterialInterface*> Materials = MeshI->GetMaterials();
		Aux.Add(MeshI, Materials);
		ComponentsActorI.Add(Aux);
	}
	DefaultMaterials.Add(ComponentsActorI);
}

void ULevelManagerSDT::LoadSegmentatedMaterials(AActor* Actor)
{
	FString ClassNameCurrentActor = ULevelManagerSDT::GetNameFromLabel(Actor->GetActorLabel());
	int* ClassIndexPtr = ObjectToIdentify.Find(ClassNameCurrentActor);
	int ClassIndex;
	if (ClassIndexPtr == nullptr){
		ClassIndex = 0; //if nullptr will be "others" class
	}else{
		ClassIndex = *ClassIndexPtr;
	}

	TArray<UMeshComponent*> PaintableMeshes;
	Actor->GetComponents<UMeshComponent>(PaintableMeshes);
	FComponentsStructSDT MeshesMaterialsActorI;
	for (UMeshComponent* MeshI : PaintableMeshes)
	{
		FStructSDT Aux;
		TArray<UMaterialInterface*> Materials = MeshI->GetMaterials();
		for (int j = 0; j < Materials.Num(); j++) {
			Materials[j] = *ClassToColorMap.Find(ClassIndex);
		}
		if (bInstanceSegmentation && ClassIndex != 0) {
			//update green channel for instance counter
			UMaterialInstanceDynamic* SegmentatedDynMaterial = Cast<UMaterialInstanceDynamic>(
				DuplicateObject(*ClassToColorMap.Find(ClassIndex), NULL));
			int counter = *SegmentationClassInstanceCounter.Find(ClassIndex) + 1;
			SegmentatedDynMaterial->SetVectorParameterValue("Color",
				FColor::FColor((uint8)ClassIndex, (uint8)counter, 0, 0));
			ClassToColorMap.Add(ClassIndex, SegmentatedDynMaterial);
		}

		Aux.Add(MeshI, Materials);
		MeshesMaterialsActorI.Add(Aux);
	}
	SegmentatedMaterials.Add(MeshesMaterialsActorI);
}

void ULevelManagerSDT::LoadBoundingMaterials(AActor* Actor) {
	FString ClassNameCurrentActor = ULevelManagerSDT::GetNameFromLabel(Actor->GetActorLabel());
	int* ClassIndexPtr = BoundingObjectToIdentify.Find(ClassNameCurrentActor);
	int ClassIndex;
	if (ClassIndexPtr == nullptr) {
		ClassIndex = 0; //if nullptr will be "others" class
	}
	else {
		ClassIndex = *ClassIndexPtr;
	}

	TArray<UMeshComponent*> PaintableMeshes;
	Actor->GetComponents<UMeshComponent>(PaintableMeshes);
	FComponentsStructSDT MeshesMaterialsActorI;
	for (UMeshComponent* MeshI : PaintableMeshes)
	{
		FStructSDT Aux;
		TArray<UMaterialInterface*> Materials = MeshI->GetMaterials();
		for (int j = 0; j < Materials.Num(); j++) {
			UMaterialInterface* BoundedMaterial = *BoundingClassToColorMap.Find(ClassIndex);
			Materials[j] = BoundedMaterial;
		}
		//update Green channel, unique for each instance of same red colour
		if (ClassIndex != 0) {
			//only for useful classes
			UMaterialInstanceDynamic* BoundedDynMaterial = Cast<UMaterialInstanceDynamic>(DuplicateObject(*BoundingClassToColorMap.Find(ClassIndex), NULL));
			int counter = *BoundingClassInstanceCounter.Find(ClassIndex) + 1;
			BoundedDynMaterial->SetVectorParameterValue("Color",
				FColor::FColor((uint8)ClassIndex, (uint8)counter, 0, 0));
			BoundingClassToColorMap.Add(ClassIndex, BoundedDynMaterial);
		}
		Aux.Add(MeshI, Materials);
		MeshesMaterialsActorI.Add(Aux);
	}
	BoundingMaterials.Add(MeshesMaterialsActorI);
}



void ULevelManagerSDT::SetSegmentation(bool Modality, bool SegmentationType)
{
	bSegmentationMode = Modality;
	bInstanceSegmentation = SegmentationType;
}

uint32 ULevelManagerSDT::GetActorListLength()
{
	return uint32(ActorList.Num());
}

void ULevelManagerSDT::SetBoundingBox(bool Modality)
{
	bBoundingBox = Modality;
}

bool ULevelManagerSDT::IsMaterialDefined()
{
	return BaseMaterial != nullptr;
}

bool ULevelManagerSDT::IsWorldReferenceDefined()
{
	return WorldPtr != nullptr;
}
