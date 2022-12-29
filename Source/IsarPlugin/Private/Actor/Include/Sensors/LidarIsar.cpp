// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Include/Sensors/LidarIsar.h"

// Sets default values
ALidarIsar::ALidarIsar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

// Called when the game starts or when spawned
void ALidarIsar::BeginPlay()
{
	Super::BeginPlay();
	if (TimeSampling != 0.0f) {
		SetTickMode(false);
		FTimerHandle ObservationTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(ObservationTimerHandle, this, &ALidarIsar::TakePeriodicObs, TimeSampling, true);
	}

	InitSensor();
}

void ALidarIsar::TakePeriodicObs()
{
	TakeObs();
}

// Called every frame
void ALidarIsar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (TimeSampling == 0.0f) {
		TakeObs();
	}
}

const uint32 ALidarIsar::GetObservationSize()
{
	return (uint32((EndAngleX - StartAngleX) / DistanceAngleX) + 1) * (uint32((EndAngleY - StartAngleY) / DistanceAngleY) + 1) * 4 * 3;
}

const bool ALidarIsar::InitSensor()
{
	RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_LaserTrace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	LastObservation = new uint8[GetObservationSize()];
	return true;
}

const FString ALidarIsar::GetSensorSetup() {
	return "{StartAngleX:" + FString::SanitizeFloat(StartAngleX) + "," +
		"EndAngleX:" + FString::SanitizeFloat(EndAngleX) + "," +
		"DistanceAngleX:" + FString::SanitizeFloat(DistanceAngleX) + "," +
		"StartAngleY:" + FString::SanitizeFloat(StartAngleY) + "," +
		"EndAngleY:" + FString::SanitizeFloat(EndAngleY) + "," +
		"DistanceAngleY:" + FString::SanitizeFloat(DistanceAngleY) + "," +
		"LaserRange:" + FString::SanitizeFloat(LaserRange) + "}";
}

const bool ALidarIsar::ChangeSensorSettings(const TArray<FString>& Settings)
{
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

const bool ALidarIsar::GetLastObs(uint8* Buffer)
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

const bool ALidarIsar::TakeObs()
{
	while(bIsBusy){}
	const FVector CurrentActorLocation = GetActorLocation();
	const FRotator CurrentActorRotation = GetActorRotation();
	const FRotator& InverseRotation = CurrentActorRotation.GetInverse();

	FVector ImpactLocation;
	int Index = 0;

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
				RV_TraceParams
			);

			if (RV_Hit.bBlockingHit) //did hit something? (bool)
			{
				ImpactLocation = InverseRotation.RotateVector(RV_Hit.ImpactPoint - CurrentActorLocation);

				if ((Render == 1) || (Render == 2))
				{
					DrawDebugLine(GetWorld(), CurrentActorLocation, End, FColor(255, 0, 0), false, 0.1f);
				}
			}
			else
			{
				ImpactLocation = FVector(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());

				if ((Render == 0) || (Render == 2))
				{
					DrawDebugLine(GetWorld(), CurrentActorLocation, End, FColor(0, 255, 0), false, 0.1f);
				}
			}

			uint8 const* FloatNumber;
			LastObservationTimestamp = FDateTime::UtcNow().ToString();
			bIsBusy = true;
			FloatNumber = reinterpret_cast<uint8 const*>(&ImpactLocation.X);
			LastObservation[Index] = FloatNumber[0];
			Index++;
			LastObservation[Index] = FloatNumber[1];
			Index++;
			LastObservation[Index] = FloatNumber[2];
			Index++;
			LastObservation[Index] = FloatNumber[3];
			Index++;

			FloatNumber = reinterpret_cast<uint8 const*>(&ImpactLocation.Y);
			LastObservation[Index] = FloatNumber[0];
			Index++;
			LastObservation[Index] = FloatNumber[1];
			Index++;
			LastObservation[Index] = FloatNumber[2];
			Index++;
			LastObservation[Index] = FloatNumber[3];
			Index++;

			FloatNumber = reinterpret_cast<uint8 const*>(&ImpactLocation.Z);
			LastObservation[Index] = FloatNumber[0];
			Index++;
			LastObservation[Index] = FloatNumber[1];
			Index++;
			LastObservation[Index] = FloatNumber[2];
			Index++;
			LastObservation[Index] = FloatNumber[3];
			Index++;
			bIsBusy = false;
		}
	}

	return true;
}

const FString ALidarIsar::GetSensorName()
{
	return "Lidar("+GetActorLabel() + ")";
}

const void ALidarIsar::SetTickMode(bool Value)
{
	enableIndependentTick = Value;
	PrimaryActorTick.bCanEverTick = enableIndependentTick;
}
