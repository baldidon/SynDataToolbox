// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Include/Sensors/LaserSDT.h"

// Sets default values
ALaserSDT::ALaserSDT()
{
	PrimaryActorTick.bCanEverTick = enableIndependentTick;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

// Called when the game starts or when spawned
void ALaserSDT::BeginPlay()
{
	Super::BeginPlay();
	if (TimeSampling != 0.0f) {
		SetTickMode(false);
		FTimerHandle ObservationTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(ObservationTimerHandle, this, &ALaserSDT::TakePeriodicObs, TimeSampling, true,0.0f);
	}

	InitSensor();
}

// Called every frame
void ALaserSDT::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (TimeSampling == 0.0f) {
		TakeObs();
	}
}

void ALaserSDT::TakePeriodicObs()
{
	TakeObs();
}

const bool ALaserSDT::TakeObs()
{
	while(bIsBusy){/*Waiting*/}
	const FVector CurrentActorLocation = GetActorLocation();
	const FRotator CurrentActorRotation = GetActorRotation();

	float InverseNormalizedDistance;
	int Index = 0;
	bIsBusy = true;
	for (float Y = StartAngleY; Y <= EndAngleY; Y += DistanceAngleY)
	{
		for (float X = StartAngleX; X <= EndAngleX; X += DistanceAngleX)
		{
			//Re-initialize hit info
			FHitResult RV_Hit(ForceInit);
			const FRotator Rotation = CurrentActorRotation + FRotator(Y, X, 0);

			const FVector End = CurrentActorLocation + Rotation.RotateVector(FVector(1, 0, 0)) * LaserRange;

			GetWorld()->LineTraceSingleByChannel(
				RV_Hit,        			//result
				CurrentActorLocation,	//start
				End,					//end
				ECC_Pawn,				//collision channel
				RV_TraceParamsActor
			);

			if (RV_Hit.bBlockingHit) //did hit something? (bool)
			{
				InverseNormalizedDistance = 1.0f - (RV_Hit.Distance / float(LaserRange));

				if ((Render == 1) || (Render == 2))
				{
					DrawDebugLine(GetWorld(), CurrentActorLocation, End, FColor(255, 0, 0), false, 0.1f);
				}
			}
			else
			{
				InverseNormalizedDistance = 0.0f;

				if ((Render == 0) || (Render == 2))
				{
					DrawDebugLine(GetWorld(), CurrentActorLocation, End, FColor(0, 255, 0), false, 0.1f);
				}
			}

			uint8 const* FloatNumber;
			FloatNumber = reinterpret_cast<uint8 const*>(&InverseNormalizedDistance);
			LastObservationTimestamp = FDateTime::UtcNow().ToString();
			LastObservation[Index] = FloatNumber[0];
			Index++;
			LastObservation[Index] = FloatNumber[1];
			Index++;
			LastObservation[Index] = FloatNumber[2];
			Index++;
			LastObservation[Index] = FloatNumber[3];
			Index++;
		}
	}
	bIsBusy = false;
	return true;
}

const uint32 ALaserSDT::GetObservationSize()
{
	return (uint32((EndAngleX - StartAngleX) / DistanceAngleX) + 1) * (uint32((EndAngleY - StartAngleY) / DistanceAngleY) + 1) * 4;
}

const bool ALaserSDT::InitSensor()
{
	RV_TraceParamsActor = FCollisionQueryParams(FName(TEXT("RV_LaserTrace")), true,this);
	RV_TraceParamsActor.bTraceComplex = true;
	RV_TraceParamsActor.bReturnPhysicalMaterial = false;
	LastObservation = new uint8[GetObservationSize()];
	return true;
}

const FString ALaserSDT::GetSensorSetup()
{
	return "{StartAngleX:" + FString::SanitizeFloat(StartAngleX) +","+
		"EndAngleX:" + FString::SanitizeFloat(EndAngleX) +","+	
		"DistanceAngleX:" + FString::SanitizeFloat(DistanceAngleX) +","+
		"StartAngleY:" + FString::SanitizeFloat(StartAngleY) + "," +
		"EndAngleY:" + FString::SanitizeFloat(EndAngleY) + "," +
		"DistanceAngleY:" + FString::SanitizeFloat(DistanceAngleY) + "," +
		"LaserRange:" + FString::SanitizeFloat(LaserRange) + "}" 
		;
}

const bool ALaserSDT::ChangeSensorSettings(const TArray<FString>& Settings)
{
	while(bIsBusy){}
	if (Settings.Num() == 8)
	{
		bIsBusy = true;
		StartAngleX = FCString::Atof(*Settings[0]);
		EndAngleX = FCString::Atof(*Settings[1]);
		DistanceAngleX = FCString::Atof(*Settings[2]);
		StartAngleY = FCString::Atof(*Settings[3]);
		EndAngleY = FCString::Atof(*Settings[4]);
		DistanceAngleY = FCString::Atof(*Settings[5]);
		LaserRange = FCString::Atof(*Settings[6]);
		Render = FCString::Atoi(*Settings[7]);
		LastObservation = new uint8[GetObservationSize()];
		bIsBusy = false;
		Modified = true;
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Settings."))
			return false;
	}
}

const bool ALaserSDT::GetLastObs(uint8* Buffer)
{
	while (bIsBusy) {}
	if (LastObservation) {
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



const FString ALaserSDT::GetSensorName()
{
	return "Laser("+GetActorLabel()+")";
}

const void ALaserSDT::SetTickMode(bool Value)
{
	enableIndependentTick = Value;
	PrimaryActorTick.bCanEverTick = enableIndependentTick;
}



