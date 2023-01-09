// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Include/Sensors/SegmentationCameraSDT.h"

ASegmentationCameraSDT::ASegmentationCameraSDT()
{
	PrimaryActorTick.bCanEverTick = enableIndependentTick;
	Camera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SegmentationCameraSDT"));
	Camera->SetupAttachment(GetRootComponent());
	if (!RenderTarget) {
		ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> RenderTargetAsset(TEXT("/SynDataToolbox/SegmentationRenderTarget"));
		RenderTarget = DuplicateObject(RenderTargetAsset.Object, RenderTarget);
	}
	if (!BaseMaterial) {
		ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("/SynDataToolbox/Materials/Segmentation_base_material"));
		BaseMaterial = MaterialAsset.Object;
	}
	LevelManager = nullptr;
	LastObservation = nullptr;
	ActorsToHide.Empty();
	Classes.Add("Others", 0);
}

void ASegmentationCameraSDT::BeginPlay()
{
	Super::BeginPlay();

	if (!LevelManager) {
		LevelManager = GetGameInstance()->GetSubsystem<ULevelManagerSDT>();
		LevelManager->SetSegmentation(true,bInstanceSegmentation);
		if (!LevelManager->IsWorldReferenceDefined())
			LevelManager->SetWorldReference(GetWorld());
		if(!LevelManager->IsMaterialDefined())
			LevelManager->SetBaseMaterial(BaseMaterial);
		LevelManager->SetSegmentationObjectsToIdentify(Classes);
	}
	InitSensor();

	if (TimeSampling != 0.0f) {
		SetTickMode(false); //do not tick!
		FTimerHandle ObservationTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(ObservationTimerHandle, this, &ASegmentationCameraSDT::TakePeriodicObs, TimeSampling, true, 0.0f);
	}
}

void ASegmentationCameraSDT::TakePeriodicObs()
{
	TakeObs();
}


FString ASegmentationCameraSDT::ClassesMapToString()
{
	FString stringyfiedMap = "";
	for (TPair<FString, int>& kvp : Classes) {
		stringyfiedMap += ""+kvp.Key+":"+FString::FromInt(kvp.Value)+",";
	}
	//remove last comma
	stringyfiedMap.RemoveFromEnd(TEXT(","));
	return stringyfiedMap;
}

void ASegmentationCameraSDT::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (TimeSampling == 0.0f) {
		TakeObs();
	}
}

const uint32 ASegmentationCameraSDT::GetObservationSize()
{
	//Mask is a simple matrix (a grayscale image)
    return (bInstanceSegmentation) ? Width*Height*2 : Width*Height;
}

const bool ASegmentationCameraSDT::InitSensor()
{
    if (!Width || !Height || !FOV || Classes.IsEmpty() || !BaseMaterial) {
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

const FString ASegmentationCameraSDT::GetSensorSetup()
{
	return "{Width:" + FString::FromInt(Width) + ","+
		"Height:" + FString::FromInt(Height) + ","+
		"FOV:" + FString::FromInt(FOV) + ","+
		"InstanceSegmentation:" + FString::FromInt((bInstanceSegmentation) ? 1:0) + ","+
		"Classes:{"+ ASegmentationCameraSDT::ClassesMapToString()+"}"+
		"}";
}

const bool ASegmentationCameraSDT::ChangeSensorSettings(const TArray<FString>& Settings)
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
		Classes.Empty();
		Classes = ParseDictFromString(*Settings[3]);
		LevelManager->SetSegmentationObjectsToIdentify(Classes);//handle change classes, into LevelManager
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
		Camera->FOVAngle = FOV;
		RenderTarget->ResizeTarget(Width, Height);
		RenderTarget->UpdateResourceImmediate();
		LastObservation = new uint8[GetObservationSize()];
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Settings."))
			return false;
	}
}

const bool ASegmentationCameraSDT::GetLastObs(uint8* Buffer)
{
	//result stored into LastObservation
	while (bIsBusy) {}
	if (LastObservation!=nullptr) {
		if (Modified) {
			TakeObs();
			Modified = false;
		}
		bIsBusy = true;
		memcpy(Buffer, LastObservation, GetObservationSize());
		bIsBusy = false;
		return true;
	}
	return false;
}

const bool ASegmentationCameraSDT::TakeObs()
{
	bool ReadIsSuccessful = true;
	while (bIsBusy) {/*Waiting*/ }
	if (LevelManager->GetLevelMode() != 1) {
		while (LevelManager->IsBusy()) {
			UE_LOG(LogTemp, Error, TEXT("Level manager is busy..."))
		}
		LevelManager->SetLevelMode(1);
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
	uint32 Index = 0;
	bIsBusy = true; //Writing
	for (auto Pixel = SurfData.CreateIterator(); Pixel; ++Pixel)	// TODO: Is there a more efficient way?
	{
		LastObservation[Index] = Pixel->R; //check only one 
		Index++;
		if (bInstanceSegmentation) {
			LastObservation[Index] = Pixel->G;
			Index++;
		}
	}
	bIsBusy = false;
	return ReadIsSuccessful;
}

const FString ASegmentationCameraSDT::GetSensorName()
{
	return "SegmentationCamera("+GetActorLabel() + ")";
}

const void ASegmentationCameraSDT::SetTickMode(bool Value)
{
	enableIndependentTick = Value;
	PrimaryActorTick.bCanEverTick = enableIndependentTick;
}

TMap<FString, int> ASegmentationCameraSDT::ParseDictFromString(FString Str)
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