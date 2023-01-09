// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/Include/Sensors/CameraSDT.h"

// Sets default values
ACameraSDT::ACameraSDT()
{
 	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = enableIndependentTick;
	Camera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CameraIsar"));
	Camera->SetupAttachment(GetRootComponent());

	if (!RenderTarget) {
		ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> RenderTargetAsset(TEXT("/IsarPlugin/CameraRenderTarget"));
		RenderTarget = DuplicateObject(RenderTargetAsset.Object, NULL);
	}

	LevelManager = nullptr;
	LastObservation = nullptr;
	ActorsToHide.Empty();
}

// Called when the game starts or when spawned
void ACameraSDT::BeginPlay()
{
	Super::BeginPlay();
	if (!LevelManager) {
		LevelManager = GetGameInstance()->GetSubsystem<ULevelManagerSDT>();
		LevelManager->SetWorldReference(GetWorld());
	}
	InitSensor();
	if (TimeSampling != 0.0f) {
		SetTickMode(false); //do not tick!
		FTimerHandle ObservationTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(ObservationTimerHandle, this, &ACameraSDT::TakePeriodicObs, TimeSampling, true,0.0f);
	}
}

void ACameraSDT::TakePeriodicObs()
{
	TakeObs();
}

// Called every frame
void ACameraSDT::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (TimeSampling == 0.0f) {
		TakeObs();
	}
}

const FString ACameraSDT::GetSensorName()
{
	return "RGBCamera(" + GetActorLabel() +")";

}

const uint32 ACameraSDT::GetObservationSize()
{
	return 3 * Width * Height;
}

const FString ACameraSDT::GetSensorSetup()
{
	return "{Width:"+FString::FromInt(Width)+","
		+"Height:"+FString::FromInt(Height) +","
		+"FOV:" + FString::FromInt(FOV)+"}";
}

const bool ACameraSDT::InitSensor()
{
	if (!Width || !Height || !FOV) {
		UE_LOG(LogTemp, Error, TEXT("Invalid Settings."))
		return false;
	}
	Camera->TextureTarget = RenderTarget;	// Set the render target for the camera
	Camera->CaptureSource = SCS_FinalColorLDR;
	Camera->bCaptureEveryFrame = false;
	Camera->bCaptureOnMovement = false;
	Camera->FOVAngle = FOV;
	RenderTarget->InitCustomFormat(Width, Height, EPixelFormat::PF_FloatRGBA, true);
	RenderTarget->TargetGamma = 3.0;
	RenderTarget->UpdateResourceImmediate();
	LastObservation = new uint8[GetObservationSize()];

	return true;
}

const bool ACameraSDT::ChangeSensorSettings(const TArray<FString>& Settings)
{
	//in this case, Settings are given by python
	if (Settings.Num() == 3)
	{
		while(LevelManager->IsBusy() || bIsBusy){}
		bIsBusy = true;
		Width = FCString::Atoi(*Settings[0]);
		Height = FCString::Atoi(*Settings[1]);
		FOV = FCString::Atoi(*Settings[2]);
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

const bool ACameraSDT::GetLastObs(uint8* Buffer)
{
	while(bIsBusy){}
	if (LastObservation != nullptr){
		if (Modified) {
			TakeObs();
			Modified = false;
		}
		bIsBusy = true;
		memcpy(Buffer,LastObservation,GetObservationSize());
		bIsBusy = false;
		return true;
	}
	return false;
}

const bool ACameraSDT::TakeObs()
{
	bool ReadIsSuccessful;
	while (bIsBusy) {/*Waiting*/ }
	if (LevelManager->GetLevelMode() != 0) {
		while (LevelManager->IsBusy()) {
			UE_LOG(LogTemp, Error, TEXT("Level manager is busy..."))
		}
		LevelManager->SetLevelMode(0);
	}
	ActorsToHide = LevelManager->GetActorsToHide();
	if (ActorsToHide.Num() != 0) {
		Camera->HiddenActors = ActorsToHide;
	}
	Camera->CaptureScene();
	LastObservationTimestamp = FDateTime::Now().ToString();
	TArray<FFloat16Color> SurfData; //stores data loaded from renderTarget
	ReadIsSuccessful = RenderTarget->GameThread_GetRenderTargetResource()->ReadFloat16Pixels(SurfData);
	uint32 Index = 0;
	bIsBusy = true;
	for (auto Pixel = SurfData.CreateIterator(); Pixel; ++Pixel)
	{
		LastObservation[Index] = Pixel->R * 255;
		Index++;
		LastObservation[Index] = Pixel->G * 255;
		Index++;
		LastObservation[Index] = Pixel->B * 255;
		Index++;
	}

	bIsBusy = false;
	return ReadIsSuccessful;
}

const void ACameraSDT::SetTickMode(bool Value)
{
	enableIndependentTick = Value;
	PrimaryActorTick.bCanEverTick = enableIndependentTick;
}

