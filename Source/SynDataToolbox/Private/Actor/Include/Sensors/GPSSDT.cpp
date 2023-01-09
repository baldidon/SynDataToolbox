// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Include/Sensors/GPSSDT.h"

// Sets default values
AGPSSDT::AGPSSDT()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = enableIndependentTick;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

// Called when the game starts or when spawned
void AGPSSDT::BeginPlay()
{
	Super::BeginPlay();
	if (TimeSampling != 0.0f) {
		SetTickMode(false);
		FTimerHandle ObservationTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(ObservationTimerHandle, this, &AGPSSDT::TakePeriodicObs, TimeSampling, true, 0.0f);
	}
	InitSensor();
	
}

void AGPSSDT::TakePeriodicObs()
{
	TakeObs();
}

// Called every frame
void AGPSSDT::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (TimeSampling == 0.0f) {
		TakeObs();
	}
}

const uint32 AGPSSDT::GetObservationSize()
{
	return 48;
}

const bool AGPSSDT::InitSensor()
{
	StartLocation = GetActorLocation();
	StartRotation = GetActorRotation();
	LastObservation = new uint8[GetObservationSize()];
	return true;
}

const bool AGPSSDT::ChangeSensorSettings(const TArray<FString>& Settings)
{
	if (Settings.Num() == 0)
	{
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Settings."))
			return false;
	}
}

const bool AGPSSDT::TakeObs()
{
	while(bIsBusy){}

	bIsBusy = true;
	const FRotator& CurrentAbsoluteRotation = GetActorRotation();
	const FVector& CurrentAbsoluteLocation = GetActorLocation();

	const FRotator& CurrentRelativeRotation = CurrentAbsoluteRotation - StartRotation;
	const FRotator& Rotation = StartRotation.GetInverse();
	const FVector& CurrentRelativeLocation = Rotation.RotateVector(CurrentAbsoluteLocation - StartLocation);

	uint8 const* FloatNumber;
	LastObservationTimestamp = FDateTime::UtcNow().ToString();
	FloatNumber = reinterpret_cast<uint8 const*>(&(CurrentAbsoluteLocation.X));
	LastObservation[0] = FloatNumber[0];
	LastObservation[1] = FloatNumber[1];
	LastObservation[2] = FloatNumber[2];
	LastObservation[3] = FloatNumber[3];

	FloatNumber = reinterpret_cast<uint8 const*>(&(CurrentAbsoluteLocation.Y));
	LastObservation[4] = FloatNumber[0];
	LastObservation[5] = FloatNumber[1];
	LastObservation[6] = FloatNumber[2];
	LastObservation[7] = FloatNumber[3];

	FloatNumber = reinterpret_cast<uint8 const*>(&(CurrentAbsoluteLocation.Z));
	LastObservation[8] = FloatNumber[0];
	LastObservation[9] = FloatNumber[1];
	LastObservation[10] = FloatNumber[2];
	LastObservation[11] = FloatNumber[3];

	FloatNumber = reinterpret_cast<uint8 const*>(&(CurrentAbsoluteRotation.Pitch));
	LastObservation[12] = FloatNumber[0];
	LastObservation[13] = FloatNumber[1];
	LastObservation[14] = FloatNumber[2];
	LastObservation[15] = FloatNumber[3];

	FloatNumber = reinterpret_cast<uint8 const*>(&(CurrentAbsoluteRotation.Yaw));
	LastObservation[16] = FloatNumber[0];
	LastObservation[17] = FloatNumber[1];
	LastObservation[18] = FloatNumber[2];
	LastObservation[19] = FloatNumber[3];

	FloatNumber = reinterpret_cast<uint8 const*>(&(CurrentAbsoluteRotation.Roll));
	LastObservation[20] = FloatNumber[0];
	LastObservation[21] = FloatNumber[1];
	LastObservation[22] = FloatNumber[2];
	LastObservation[23] = FloatNumber[3];

	FloatNumber = reinterpret_cast<uint8 const*>(&(CurrentRelativeLocation.X));
	LastObservation[24] = FloatNumber[0];
	LastObservation[25] = FloatNumber[1];
	LastObservation[26] = FloatNumber[2];
	LastObservation[27] = FloatNumber[3];

	FloatNumber = reinterpret_cast<uint8 const*>(&(CurrentRelativeLocation.Y));
	LastObservation[28] = FloatNumber[0];
	LastObservation[29] = FloatNumber[1];
	LastObservation[30] = FloatNumber[2];
	LastObservation[31] = FloatNumber[3];

	FloatNumber = reinterpret_cast<uint8 const*>(&(CurrentRelativeLocation.Z));
	LastObservation[32] = FloatNumber[0];
	LastObservation[33] = FloatNumber[1];
	LastObservation[34] = FloatNumber[2];
	LastObservation[35] = FloatNumber[3];

	FloatNumber = reinterpret_cast<uint8 const*>(&(CurrentRelativeRotation.Pitch));
	LastObservation[36] = FloatNumber[0];
	LastObservation[37] = FloatNumber[1];
	LastObservation[38] = FloatNumber[2];
	LastObservation[39] = FloatNumber[3];

	FloatNumber = reinterpret_cast<uint8 const*>(&(CurrentRelativeRotation.Yaw));
	LastObservation[40] = FloatNumber[0];
	LastObservation[41] = FloatNumber[1];
	LastObservation[42] = FloatNumber[2];
	LastObservation[43] = FloatNumber[3];

	FloatNumber = reinterpret_cast<uint8 const*>(&(CurrentRelativeRotation.Roll));
	LastObservation[44] = FloatNumber[0];
	LastObservation[45] = FloatNumber[1];
	LastObservation[46] = FloatNumber[2];
	LastObservation[47] = FloatNumber[3];
	bIsBusy = false;
	return true;
}

const bool AGPSSDT::GetLastObs(uint8* Buffer)
{
	while (bIsBusy) {}
	if (LastObservation) {
		bIsBusy = true;
		Buffer = LastObservation;
		bIsBusy = false;
		return true;
	}
	return false;
}

const FString AGPSSDT::GetSensorName()
{
	return "GPS("+GetActorLabel() + ")";
}

const void AGPSSDT::SetTickMode(bool Value)
{
	enableIndependentTick = Value;
	PrimaryActorTick.bCanEverTick = enableIndependentTick;
}

