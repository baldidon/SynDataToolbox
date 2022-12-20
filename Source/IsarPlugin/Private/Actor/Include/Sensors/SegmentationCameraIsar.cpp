// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Include/Sensors/SegmentationCameraIsar.h"

ASegmentationCameraIsar::ASegmentationCameraIsar()
{
	PrimaryActorTick.bCanEverTick = enableIndependentTick;
	Camera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SegmentationCameraIsar"));
	Camera->SetupAttachment(GetRootComponent());
	if (!RenderTarget) {
		ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> RenderTargetAsset(TEXT("/IsarPlugin/SegmentationRenderTarget"));
		RenderTarget = DuplicateObject(RenderTargetAsset.Object, RenderTarget);
	}
	LevelManager = nullptr;
	LastObservation = nullptr;
	ActorsToHide.Empty();
	Classes.Add("Others", 0);
}

void ASegmentationCameraIsar::BeginPlay()
{
	Super::BeginPlay();

	if (!LevelManager) {
		LevelManager = GetGameInstance()->GetSubsystem<ULevelManagerIsar>();
		LevelManager->SetWorldReference(GetWorld());
		LevelManager->SetBaseMaterial(BaseMaterial);
		LevelManager->SetObjectsToIdentify(Classes);
	}
	InitSensor();

	if (TimeSampling != 0.0f) {
		SetTickMode(false); //do not tick!
		FTimerHandle ObservationTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(ObservationTimerHandle, this, &ASegmentationCameraIsar::TakePeriodicObs, TimeSampling, true, 0.0f);
	}
}

void ASegmentationCameraIsar::TakePeriodicObs()
{
	TakeObs();
}

FString ASegmentationCameraIsar::ClassesMapToString()
{
	FString stringyfiedMap = "";
	for (TPair<FString, int>& kvp : Classes) {
		stringyfiedMap += ""+kvp.Key+":"+FString::FromInt(kvp.Value)+",";
	}
	//remove last comma
	stringyfiedMap.RemoveFromEnd(TEXT(","));
	return stringyfiedMap;
}

void ASegmentationCameraIsar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (TimeSampling == 0.0f) {
		TakeObs();
	}
}

const uint32 ASegmentationCameraIsar::GetObservationSize()
{
	//Mask is a simple matrix (a grayscale image)
    return Width*Height;
}

const bool ASegmentationCameraIsar::InitSensor()
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

const FString ASegmentationCameraIsar::GetSensorSetup()
{
	return "{Width:" + FString::FromInt(Width) + ","+
		"Height:" + FString::FromInt(Height) + ","+
		"FOV:" + FString::FromInt(FOV) + ","+
		"Classes:{"+ASegmentationCameraIsar::ClassesMapToString()+"}"+
		"}";
}

const bool ASegmentationCameraIsar::ChangeSensorSettings(const TArray<FString>& Settings)
{
    return false;
}

const bool ASegmentationCameraIsar::GetLastObs(uint8* Buffer)
{
	//result stored into LastObservation
	while (bIsBusy) {}
	if (LastObservation!=nullptr) {
		memcpy(Buffer, LastObservation, GetObservationSize());
		return true;
	}
	return false;
}

const bool ASegmentationCameraIsar::TakeObs()
{
	bool ReadIsSuccessful = true;
	while (bIsBusy) {/*Waiting*/ }
	if (LevelManager->GetLevelMode() == 0) {
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
	}
	bIsBusy = false;
	return ReadIsSuccessful;
}

const FString ASegmentationCameraIsar::GetSensorName()
{
	return "SegmentationCamera("+GetActorLabel() + ")";
}

const void ASegmentationCameraIsar::SetTickMode(bool Value)
{
	enableIndependentTick = Value;
	PrimaryActorTick.bCanEverTick = enableIndependentTick;
}