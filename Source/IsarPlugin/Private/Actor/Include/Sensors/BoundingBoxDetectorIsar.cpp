#include "Actor/Include/Sensors/BoundingBoxDetectorIsar.h"

ABoundingBoxDetectorIsar::ABoundingBoxDetectorIsar()
{
	PrimaryActorTick.bCanEverTick = enableIndependentTick;
	Camera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("BoundingBoxDetectorIsar"));
	Camera->SetupAttachment(GetRootComponent());
	
	if (!RenderTarget) {
		ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> RenderTargetAsset(TEXT("/IsarPlugin/SegmentationRenderTarget"));
		RenderTarget = RenderTargetAsset.Object;
	}
	if (!BaseMaterial) {
		ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("/IsarPlugin/Materials/Segmentation_base_material"));
		BaseMaterial = MaterialAsset.Object;
	}
	LevelManager = nullptr;
	LastObservation = nullptr;
	ActorsToHide.Empty();
	ClassesToBound.Add("Others",0);
}

void ABoundingBoxDetectorIsar::BeginPlay()
{
	Super::BeginPlay();

	if (!LevelManager) {
		LevelManager = GetGameInstance()->GetSubsystem<ULevelManagerIsar>();
		LevelManager->SetBoundingBox(true);
		if (!LevelManager->IsWorldReferenceDefined())
			LevelManager->SetWorldReference(GetWorld());
		if (!LevelManager->IsMaterialDefined())
			LevelManager->SetBaseMaterial(BaseMaterial);
		LevelManager->SetBoundingBoxObjectsToIdentify(ClassesToBound);
		//create Map for saving bbox 
		for (auto& Pair : ClassesToBound) {
			if (!Pair.Key.Equals("Others")) {
				BoundingBoxes.Add(Pair.Value, FBoundingBoxCollectionIsar());
			}
		}
	}

	InitSensor();

	if (TimeSampling != 0.0f) {
		SetTickMode(false); //do not tick!
		FTimerHandle ObservationTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(ObservationTimerHandle, this, &ABoundingBoxDetectorIsar::TakePeriodicObs, TimeSampling, true, 0.0f);
	}
}

void ABoundingBoxDetectorIsar::TakePeriodicObs()
{
	TakeObs();
}

FString ABoundingBoxDetectorIsar::ClassesMapToString()
{
	FString stringyfiedMap = "";
	for (TPair<FString, int>& kvp : ClassesToBound) {
		if(!kvp.Key.Equals("Others")){
			stringyfiedMap += "" + kvp.Key + ":" + FString::FromInt(kvp.Value) + ",";
		}
	}
	//remove last comma
	stringyfiedMap.RemoveFromEnd(TEXT(","));
	return stringyfiedMap;
}

void ABoundingBoxDetectorIsar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (TimeSampling == 0.0f) {
		TakeObs();
	}
}

const uint32 ABoundingBoxDetectorIsar::GetObservationSize()
{
	return uint32(MaxActors*10);
}

const bool ABoundingBoxDetectorIsar::InitSensor()
{
	if (!Width || !Height || !FOV || ClassesToBound.IsEmpty() || !BaseMaterial) {
		UE_LOG(LogTemp, Error, TEXT("Invalid Settings."))
			return false;
	}

	Camera->TextureTarget = RenderTarget;	// Set the render target for the camera
	Camera->CaptureSource = SCS_FinalColorLDR;
	Camera->bCaptureEveryFrame = false;
	Camera->bCaptureOnMovement = false;
	Camera->FOVAngle = FOV;
	Camera->ShowFlags.SetFog(false);
	Camera->ShowFlags.SetPostProcessing(false);
	Camera->ShowFlags.SetAtmosphere(false); //flat world!
	RenderTarget->InitCustomFormat(Width, Height, EPixelFormat::PF_FloatRGBA, true);
	LastObservation = new uint8[GetObservationSize()]; //set up dimension
	return true;
}

const FString ABoundingBoxDetectorIsar::GetSensorSetup()
{
	//because i must use a camera!
	return "{Width:" + FString::FromInt(Width) + "," +
		"Height:" + FString::FromInt(Height) + "," +
		"FOV:" + FString::FromInt(FOV) + "," +
		"MaxActors:"+FString::FromInt(MaxActors) + "," +
		"Classes:{" + ABoundingBoxDetectorIsar::ClassesMapToString() + "}" +
		"}";
}

const bool ABoundingBoxDetectorIsar::ChangeSensorSettings(const TArray<FString>& Settings)
{
	if (Settings.Num() == 4)
	{
		while (LevelManager->IsBusy() || bIsBusy) {}
			bIsBusy = true;
			Width = FCString::Atoi(*Settings[0]);
			Height = FCString::Atoi(*Settings[1]);
			FOV = FCString::Atoi(*Settings[2]);
			Camera->FOVAngle = FOV;
			RenderTarget->ResizeTarget(Width, Height);
			RenderTarget->UpdateResourceImmediate();
			LastObservation = new uint8[GetObservationSize()];
			ClassesToBound.Empty();
			ClassesToBound = ParseDictFromString(*Settings[3]);
			for (auto& Pair : ClassesToBound) {
				if (!Pair.Key.Equals("Others")) {
					BoundingBoxes.Add(Pair.Value, FBoundingBoxCollectionIsar());
				}
			}
			LevelManager->SetBoundingBoxObjectsToIdentify(ClassesToBound);//handle change classes, into LevelManager
			Modified = true;
			bIsBusy = false;
			return true;
	}
	else if (Settings.Num() == 3)
	{
		//CASE WITH NO CLASSES GIVEN
		Width = FCString::Atoi(*Settings[0]);
		Height = FCString::Atoi(*Settings[1]);
		FOV = FCString::Atoi(*Settings[2]);
		bIsBusy = true;
		Camera->FOVAngle = FOV;
		RenderTarget->ResizeTarget(Width, Height);
		RenderTarget->UpdateResourceImmediate();
		LastObservation = new uint8[GetObservationSize()];
		Modified = true;
		bIsBusy = false;
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Settings."))
			return false;
	}
}

const bool ABoundingBoxDetectorIsar::GetLastObs(uint8* Buffer)
{
	//result stored into LastObservation
	while (bIsBusy) {}
	if (LastObservation != nullptr) {
		if (Modified) {
			TakeObs();
			Modified = false;
		}
		memcpy(Buffer, LastObservation, GetObservationSize());
		return true;
	}
	return false;
}

const bool ABoundingBoxDetectorIsar::TakeObs()
{
	bool ReadIsSuccessful = true;
	while (bIsBusy) {/*Waiting*/ }
	if (LevelManager->GetLevelMode() != 2) {
		while (LevelManager->IsBusy()) {
			UE_LOG(LogTemp, Error, TEXT("Level manager is busy..."))
		}
		LevelManager->SetLevelMode(2);
	}
	ActorsToHide = LevelManager->GetActorsToHide();
	if (ActorsToHide.Num() != 0) {
		Camera->HiddenActors = ActorsToHide;
	}

	//get level snapshot
	Camera->CaptureScene();
	LastObservationTimestamp = FDateTime::Now().ToString();
	TArray<FColor> SurfData; //stores data loaded from renderTarget
	ReadIsSuccessful = RenderTarget->GameThread_GetRenderTargetResource()->ReadPixels(SurfData);
	uint32 Row = 0;
	uint32 Col = 0;
	uint32 DefaultPixelValue = 0;
	uint32 PreviousPixelValue = 0;
	
	bIsBusy = true; //Writing
	for (auto Pixel = SurfData.CreateIterator(); Pixel; ++Pixel)
	{
		if (Col != 0 && Col%Height == 0) {
			Row++;
			Col = 0;
		}
		uint8 RIndex = Pixel->R;
		uint8 GIndex = Pixel->G;
		if (RIndex != 0) {
			//check if coord exists
			if (BoundingBoxes.Find(RIndex)->GetCoordinates(Pixel->G)) {
				
				//check if can change something like x1,x2,y2 (no y1)
				if (Row > BoundingBoxes.Find(RIndex)->GetY2(Pixel->G)) {
					BoundingBoxes.Find(RIndex)->UpdateY2(Row,Pixel->G);
				}
				if (Col < BoundingBoxes.Find(RIndex)->GetX1(Pixel->G)) {
					BoundingBoxes.Find(RIndex)->UpdateX1(Col,Pixel->G);
				}
				else if (Col> BoundingBoxes.Find(RIndex)->GetX2(Pixel->G)) {
					BoundingBoxes.Find(RIndex)->UpdateX2(Col, Pixel->G);
				}
			}
			else {
				//create new one
				BoundingBoxes.Find(RIndex)->Add(Pixel->G, FVector4(Col, Row, Col, Row));
			}
		}
		Col++;
	}
	bIsBusy = false;

	//write result formatted
	int Index = 0;
	for (auto& InstancesOfClass : BoundingBoxes) {
		for (uint8 I : InstancesOfClass.Value.GetIndexes()) {
			//because max image dimension is a uint16, pixel position are uint16 values
			//I neeed to split into two uint8 elements
			if (Index <= (MaxActors * 10) - 10) {
				LastObservation[Index] = InstancesOfClass.Key;
				Index++;
				LastObservation[Index] = I;
				Index++;
				LastObservation[Index] = (uint16)InstancesOfClass.Value.GetX1(I) & 0xff;
				Index++;
				LastObservation[Index] = ((uint16)InstancesOfClass.Value.GetX1(I) >> 8);
				Index++;
				LastObservation[Index] = (uint16)InstancesOfClass.Value.GetY1(I) & 0xff;
				Index++;
				LastObservation[Index] = ((uint16)InstancesOfClass.Value.GetY1(I) >> 8);
				Index++;
				LastObservation[Index] = (uint16)InstancesOfClass.Value.GetX2(I) & 0xff;
				Index++;
				LastObservation[Index] = ((uint16)InstancesOfClass.Value.GetX2(I) >> 8);
				Index++;
				LastObservation[Index] = (uint16)InstancesOfClass.Value.GetY2(I) & 0xff;
				Index++;
				LastObservation[Index] = ((uint16)InstancesOfClass.Value.GetY2(I) >> 8);
				Index++;
			}
		}
	}

	while(Index <= MaxActors*10) {
		//fill with zeroes
		LastObservation[Index] = (uint8)0; //escape value
		Index++;
	}

	for (auto& el : BoundingBoxes) {
		el.Value.Clean();
	}
	return ReadIsSuccessful;
}

const FString ABoundingBoxDetectorIsar::GetSensorName()
{
	return "BoundingBox("+GetActorLabel()+")";
}

const void ABoundingBoxDetectorIsar::SetTickMode(bool Value)
{
	enableIndependentTick = Value;
	PrimaryActorTick.bCanEverTick = enableIndependentTick;
}

TMap<FString, int> ABoundingBoxDetectorIsar::ParseDictFromString(FString Str)
{
	TMap<FString, int> NewDict = TMap<FString, int>();
	NewDict.Add("Others", 0);
	int len = Str.Len();
	FString Delim = ":";
	TArray<FString> KeyValueStringArray = TArray<FString>();
	Str.ParseIntoArray(KeyValueStringArray, TEXT(","));
	for (FString kvs : KeyValueStringArray) {
		TArray<FString> KeyValue = TArray<FString>();
		kvs.ParseIntoArray(KeyValue, TEXT(":"));
		int Index = FCString::Atoi(*KeyValue[1]);
		NewDict.Add(KeyValue[0], Index);
	}

	return NewDict;
}
