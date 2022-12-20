// Fill out your copyright notice in the Description page of Project Settings.
#include "Actor/Include/Sensors/VisibilityIsar.h"

// Sets default values
AVisibilityIsar::AVisibilityIsar()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = enableIndependentTick;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

}

// Called when the game starts or when spawned
void AVisibilityIsar::BeginPlay()
{
	Super::BeginPlay();
	if (TimeSampling != 0.0f) {
		SetTickMode(false);
		FTimerHandle ObservationTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(ObservationTimerHandle, this, &AVisibilityIsar::TakePeriodicObs, TimeSampling, true);
	}

	InitSensor();
}

// Called every frame
void AVisibilityIsar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (TimeSampling == 0.0f) {
		TakeObs();
	}
}

void AVisibilityIsar::TakePeriodicObs()
{
	TakeObs();
}

const uint32 AVisibilityIsar::GetObservationSize()
{
	return (uint32((EndAngleX - StartAngleX) / DistanceAngleX) + 1) * (uint32((EndAngleY - StartAngleY) / DistanceAngleY) + 1) * 4 * 2 * CellsRange * 3;
	// n-yray, n-xray, 4byte, 2coordinates, n-visibilitycells, 3ray-types
}

const bool AVisibilityIsar::InitSensor()
{
	RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_VisibilityTrace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	LastObservation = new uint8[GetObservationSize()];
	return true;
}

const FString AVisibilityIsar::GetSensorSetup()
{
	return "{StartAngleX:" + FString::SanitizeFloat(StartAngleX) + "," +
		"EndAngleX:" + FString::SanitizeFloat(EndAngleX) + "," +
		"DistanceAngleX:" + FString::SanitizeFloat(DistanceAngleX) + "," +
		"StartAngleY:" + FString::SanitizeFloat(StartAngleY) + "," +
		"EndAngleY:" + FString::SanitizeFloat(EndAngleY) + "," +
		"DistanceAngleY:" + FString::SanitizeFloat(DistanceAngleY) + "," +
		"LaserRange:" + FString::SanitizeFloat(LaserRange) +","+
		"CellsRange:"+FString::FromInt(CellsRange) + "}";
}

const bool AVisibilityIsar::ChangeSensorSettings(const TArray<FString>& Settings)
{
	while (bIsBusy) {}
	if (Settings.Num() == 10)
	{
		bIsBusy = true;

		StartAngleX = FCString::Atof(*Settings[0]);
		EndAngleX = FCString::Atof(*Settings[1]);
		DistanceAngleX = FCString::Atof(*Settings[2]);
		StartAngleY = FCString::Atof(*Settings[3]);
		EndAngleY = FCString::Atof(*Settings[4]);
		DistanceAngleY = FCString::Atof(*Settings[5]);
		LaserRange = FCString::Atof(*Settings[6]);
		CellsRange = FCString::Atoi(*Settings[7]);
		Render = FCString::Atoi(*Settings[8]);
		TimeSampling = FCString::Atof(*Settings[9]);

		StartLocation = GetActorLocation();
		StartRotation = GetActorRotation();
		LastObservation = new uint8[GetObservationSize()];

		bIsBusy = false;

		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Settings."))
			return false;
	}
}

const bool AVisibilityIsar::GetLastObs(uint8* Buffer)
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

const FString AVisibilityIsar::GetSensorName()
{
	return "VisibilityIsar(" + GetActorLabel() + ")";
}

const void AVisibilityIsar::SetTickMode(bool Value)
{
	enableIndependentTick = Value;
	PrimaryActorTick.bCanEverTick = enableIndependentTick;
}

const bool AVisibilityIsar::TakeObs()
{
	while (bIsBusy) {/*Waiting*/ }
	bIsBusy = true;
	const FVector CurrentActorLocation = GetActorLocation();
	const FRotator CurrentActorRotation = GetActorRotation();

	const FRotator InvRotation = StartRotation.GetInverse();

	float CoordX;	// Unreal GPS Space
	float CoordY;	// Unreal GPS Space
	int Index = 0;
	TArray<FHitResult> HitList;
	TArray<FHitResult> HitListTarget;
	TArray<FHitResult> HitListMatch;

	FVector LaserEnd;

	for (float Y = StartAngleY; Y <= EndAngleY; Y += DistanceAngleY)
	{
		for (float X = StartAngleX; X <= EndAngleX; X += DistanceAngleX)
		{
			//Re-initialize hit info
			HitList.Reset();
			HitListTarget.Reset();
			HitListMatch.Reset();

			const FRotator Rotation = CurrentActorRotation + FRotator(Y, X, 0);
			const FVector End = CurrentActorLocation + Rotation.RotateVector(FVector(1, 0, 0)) * LaserRange;
			const FVector Start = FVector(CurrentActorLocation.X, CurrentActorLocation.Y, CurrentActorLocation.Z - 5.);

			GetWorld()->LineTraceMultiByChannel(
				HitList,				// hit array
				Start,					// start
				End,					// end
				ECC_GameTraceChannel5,	// collision channel [Visibility_channel]
				RV_TraceParams			// params
			);

			GetWorld()->LineTraceMultiByChannel(
				HitListTarget,			// hit array
				Start,					// start
				End,					// end
				ECC_GameTraceChannel6,	// collision channel [Tvisibility_channel]
				RV_TraceParams			// params
			);

			GetWorld()->LineTraceMultiByChannel(
				HitListMatch,			// hit array
				Start,					// start
				End,					// end
				ECC_GameTraceChannel7,	// collision channel [Match_channel]
				RV_TraceParams			// params
			);

			/*
			cells -> V: overlap, TV: overlap, M: overlap
			obstacle -> V: block, TV: ignore, M: block
			target -> V: block, TV: block, M: overlap
			*/

			// +++++++++++++++++++++++++++++++++++++++Send senors's data++++++++++++++++++++++++++++++++++++++
			if (HitList.Num() > 0) {

				// Draw debug line
				if (Render == 1) {
					if (HitList.Num() <= CellsRange)
					{
						LaserEnd = HitList.Last().Location;
						if (HitList.Last().bBlockingHit)
							DrawDebugLine(GetWorld(), CurrentActorLocation, LaserEnd, FColor(255, 0, 0), false, 0.1f);
						else
							DrawDebugLine(GetWorld(), CurrentActorLocation, LaserEnd, FColor(0, 255, 0), false, 0.1f);
					}
					else
					{
						LaserEnd = HitList[CellsRange].Location;
						if (HitList[CellsRange].bBlockingHit)
							DrawDebugLine(GetWorld(), CurrentActorLocation, LaserEnd, FColor(255, 0, 0), false, 0.1f);
						else
							DrawDebugLine(GetWorld(), CurrentActorLocation, LaserEnd, FColor(0, 255, 0), false, 0.1f);
					}

				}
				LastObservationTimestamp = FDateTime::UtcNow().ToString();
				for (int i = 0; i < CellsRange; i++)	// HitList may be longer than CellsRange
				{

					if (HitList[i].bBlockingHit)	  // Obstacle encountered
					{
						CoordX = -1.0;
						CoordY = -1.0;
						for (int j = i; j < CellsRange; j++)	  // No visibility beyond the obstacle
						{
							uint8 const* FloatNumberX;
							FloatNumberX = reinterpret_cast<uint8 const*>(&CoordX);
							LastObservation[Index] = FloatNumberX[0];
							Index++;
							LastObservation[Index] = FloatNumberX[1];
							Index++;
							LastObservation[Index] = FloatNumberX[2];
							Index++;
							LastObservation[Index] = FloatNumberX[3];
							Index++;

							uint8 const* FloatNumberY;
							FloatNumberY = reinterpret_cast<uint8 const*>(&CoordY);
							LastObservation[Index] = FloatNumberY[0];
							Index++;
							LastObservation[Index] = FloatNumberY[1];
							Index++;
							LastObservation[Index] = FloatNumberY[2];
							Index++;
							LastObservation[Index] = FloatNumberY[3];
							Index++;
						}
						break;
					}
					else    // No obstacle encountered
					{
						FVector RelativeLocation = InvRotation.RotateVector(HitList[i].Location - StartLocation);
						CoordX = RelativeLocation.X;
						CoordY = RelativeLocation.Y;

						uint8 const* FloatNumberX;
						FloatNumberX = reinterpret_cast<uint8 const*>(&CoordX);
						LastObservation[Index] = FloatNumberX[0];
						Index++;
						LastObservation[Index] = FloatNumberX[1];
						Index++;
						LastObservation[Index] = FloatNumberX[2];
						Index++;
						LastObservation[Index] = FloatNumberX[3];
						Index++;

						uint8 const* FloatNumberY;
						FloatNumberY = reinterpret_cast<uint8 const*>(&CoordY);
						LastObservation[Index] = FloatNumberY[0];
						Index++;
						LastObservation[Index] = FloatNumberY[1];
						Index++;
						LastObservation[Index] = FloatNumberY[2];
						Index++;
						LastObservation[Index] = FloatNumberY[3];
						Index++;
					}

					if (i == HitList.Num() - 1)	// Less HitResults than VisualLenght (why?)
					{
						CoordX = -1.0;
						CoordY = -1.0;
						for (int k = i + 1; k < CellsRange; k++)
						{
							uint8 const* FloatNumberX;
							FloatNumberX = reinterpret_cast<uint8 const*>(&CoordX);
							LastObservation[Index] = FloatNumberX[0];
							Index++;
							LastObservation[Index] = FloatNumberX[1];
							Index++;
							LastObservation[Index] = FloatNumberX[2];
							Index++;
							LastObservation[Index] = FloatNumberX[3];
							Index++;

							uint8 const* FloatNumberY;
							FloatNumberY = reinterpret_cast<uint8 const*>(&CoordY);
							LastObservation[Index] = FloatNumberY[0];
							Index++;
							LastObservation[Index] = FloatNumberY[1];
							Index++;
							LastObservation[Index] = FloatNumberY[2];
							Index++;
							LastObservation[Index] = FloatNumberY[3];
							Index++;
						}
						break;
					}
				}
			}
			else	// HitList is empty
			{
				CoordX = -1.0;
				CoordY = -1.0;
				for (int w = 0; w < CellsRange; w++)
				{
					uint8 const* FloatNumberX;
					FloatNumberX = reinterpret_cast<uint8 const*>(&CoordX);
					LastObservation[Index] = FloatNumberX[0];
					Index++;
					LastObservation[Index] = FloatNumberX[1];
					Index++;
					LastObservation[Index] = FloatNumberX[2];
					Index++;
					LastObservation[Index] = FloatNumberX[3];
					Index++;

					uint8 const* FloatNumberY;
					FloatNumberY = reinterpret_cast<uint8 const*>(&CoordY);
					LastObservation[Index] = FloatNumberY[0];
					Index++;
					LastObservation[Index] = FloatNumberY[1];
					Index++;
					LastObservation[Index] = FloatNumberY[2];
					Index++;
					LastObservation[Index] = FloatNumberY[3];
					Index++;
				}
			}

			// ++++++++++++++++++++++++++++++++++++++++TargetLocalization+++++++++++++++++++++++++++++++++++
			if (HitListTarget.Num() > 0) {

				for (int i = 0; i < CellsRange; i++)
				{

					if (HitListTarget[i].bBlockingHit)
					{
						FVector RelativeLocation = InvRotation.RotateVector(HitListTarget[i].Location - StartLocation);
						CoordX = RelativeLocation.X;
						CoordY = RelativeLocation.Y;

						uint8 const* FloatNumberX;
						FloatNumberX = reinterpret_cast<uint8 const*>(&CoordX);
						LastObservation[Index] = FloatNumberX[0];
						Index++;
						LastObservation[Index] = FloatNumberX[1];
						Index++;
						LastObservation[Index] = FloatNumberX[2];
						Index++;
						LastObservation[Index] = FloatNumberX[3];
						Index++;

						uint8 const* FloatNumberY;
						FloatNumberY = reinterpret_cast<uint8 const*>(&CoordY);
						LastObservation[Index] = FloatNumberY[0];
						Index++;
						LastObservation[Index] = FloatNumberY[1];
						Index++;
						LastObservation[Index] = FloatNumberY[2];
						Index++;
						LastObservation[Index] = FloatNumberY[3];
						Index++;


						CoordX = -1.0;
						CoordY = -1.0;

						for (int j = i + 1; j < CellsRange; j++)	  // No visibility beyond the obstacle
						{
							FloatNumberX = reinterpret_cast<uint8 const*>(&CoordX);
							LastObservation[Index] = FloatNumberX[0];
							Index++;
							LastObservation[Index] = FloatNumberX[1];
							Index++;
							LastObservation[Index] = FloatNumberX[2];
							Index++;
							LastObservation[Index] = FloatNumberX[3];
							Index++;

							FloatNumberY = reinterpret_cast<uint8 const*>(&CoordY);
							LastObservation[Index] = FloatNumberY[0];
							Index++;
							LastObservation[Index] = FloatNumberY[1];
							Index++;
							LastObservation[Index] = FloatNumberY[2];
							Index++;
							LastObservation[Index] = FloatNumberY[3];
							Index++;
						}
						break;

					}
					else    // No obstacle encountered
					{
						CoordX = -1.0;
						CoordY = -1.0;

						uint8 const* FloatNumberX;
						FloatNumberX = reinterpret_cast<uint8 const*>(&CoordX);
						LastObservation[Index] = FloatNumberX[0];
						Index++;
						LastObservation[Index] = FloatNumberX[1];
						Index++;
						LastObservation[Index] = FloatNumberX[2];
						Index++;
						LastObservation[Index] = FloatNumberX[3];
						Index++;

						uint8 const* FloatNumberY;
						FloatNumberY = reinterpret_cast<uint8 const*>(&CoordY);
						LastObservation[Index] = FloatNumberY[0];
						Index++;
						LastObservation[Index] = FloatNumberY[1];
						Index++;
						LastObservation[Index] = FloatNumberY[2];
						Index++;
						LastObservation[Index] = FloatNumberY[3];
						Index++;
					}


					if (i == HitListTarget.Num() - 1)	// Less HitResults than VisualLenght (why?)
					{
						CoordX = -1.0;
						CoordY = -1.0;
						for (int k = i + 1; k < CellsRange; k++)
						{
							uint8 const* FloatNumberX;
							FloatNumberX = reinterpret_cast<uint8 const*>(&CoordX);
							LastObservation[Index] = FloatNumberX[0];
							Index++;
							LastObservation[Index] = FloatNumberX[1];
							Index++;
							LastObservation[Index] = FloatNumberX[2];
							Index++;
							LastObservation[Index] = FloatNumberX[3];
							Index++;

							uint8 const* FloatNumberY;
							FloatNumberY = reinterpret_cast<uint8 const*>(&CoordY);
							LastObservation[Index] = FloatNumberY[0];
							Index++;
							LastObservation[Index] = FloatNumberY[1];
							Index++;
							LastObservation[Index] = FloatNumberY[2];
							Index++;
							LastObservation[Index] = FloatNumberY[3];
							Index++;
						}
						break;
					}

				}
			}
			else	// HitList is empty
			{
				CoordX = -1.0;
				CoordY = -1.0;
				for (int w = 0; w < CellsRange; w++)
				{
					uint8 const* FloatNumberX;
					FloatNumberX = reinterpret_cast<uint8 const*>(&CoordX);
					LastObservation[Index] = FloatNumberX[0];
					Index++;
					LastObservation[Index] = FloatNumberX[1];
					Index++;
					LastObservation[Index] = FloatNumberX[2];
					Index++;
					LastObservation[Index] = FloatNumberX[3];
					Index++;

					uint8 const* FloatNumberY;
					FloatNumberY = reinterpret_cast<uint8 const*>(&CoordY);
					LastObservation[Index] = FloatNumberY[0];
					Index++;
					LastObservation[Index] = FloatNumberY[1];
					Index++;
					LastObservation[Index] = FloatNumberY[2];
					Index++;
					LastObservation[Index] = FloatNumberY[3];
					Index++;
				}
			}


			// ++++++++++++++++++++++++++++++++++++++Match+++++++++++++++++++++++++++++++++++++++++++
			if (HitListMatch.Num() > 0) {

				for (int i = 0; i < CellsRange; i++)	// HitList may be longer than CellsRange
				{

					if (HitListMatch[i].bBlockingHit)	  // Obstacle encountered
					{
						CoordX = -1.0;
						CoordY = -1.0;
						for (int j = i; j < CellsRange; j++)	  // No visibility beyond the obstacle
						{
							uint8 const* FloatNumberX;
							FloatNumberX = reinterpret_cast<uint8 const*>(&CoordX);
							LastObservation[Index] = FloatNumberX[0];
							Index++;
							LastObservation[Index] = FloatNumberX[1];
							Index++;
							LastObservation[Index] = FloatNumberX[2];
							Index++;
							LastObservation[Index] = FloatNumberX[3];
							Index++;

							uint8 const* FloatNumberY;
							FloatNumberY = reinterpret_cast<uint8 const*>(&CoordY);
							LastObservation[Index] = FloatNumberY[0];
							Index++;
							LastObservation[Index] = FloatNumberY[1];
							Index++;
							LastObservation[Index] = FloatNumberY[2];
							Index++;
							LastObservation[Index] = FloatNumberY[3];
							Index++;
						}
						break;
					}
					else    // No obstacle encountered
					{
						FVector RelativeLocation = InvRotation.RotateVector(HitListMatch[i].Location - StartLocation);
						CoordX = RelativeLocation.X;
						CoordY = RelativeLocation.Y;

						uint8 const* FloatNumberX;
						FloatNumberX = reinterpret_cast<uint8 const*>(&CoordX);
						LastObservation[Index] = FloatNumberX[0];
						Index++;
						LastObservation[Index] = FloatNumberX[1];
						Index++;
						LastObservation[Index] = FloatNumberX[2];
						Index++;
						LastObservation[Index] = FloatNumberX[3];
						Index++;

						uint8 const* FloatNumberY;
						FloatNumberY = reinterpret_cast<uint8 const*>(&CoordY);
						LastObservation[Index] = FloatNumberY[0];
						Index++;
						LastObservation[Index] = FloatNumberY[1];
						Index++;
						LastObservation[Index] = FloatNumberY[2];
						Index++;
						LastObservation[Index] = FloatNumberY[3];
						Index++;
					}

					if (i == HitListMatch.Num() - 1)	// Less HitResults than VisualLenght (why?)
					{
						CoordX = -1.0;
						CoordY = -1.0;
						for (int k = i + 1; k < CellsRange; k++)
						{
							uint8 const* FloatNumberX;
							FloatNumberX = reinterpret_cast<uint8 const*>(&CoordX);
							LastObservation[Index] = FloatNumberX[0];
							Index++;
							LastObservation[Index] = FloatNumberX[1];
							Index++;
							LastObservation[Index] = FloatNumberX[2];
							Index++;
							LastObservation[Index] = FloatNumberX[3];
							Index++;

							uint8 const* FloatNumberY;
							FloatNumberY = reinterpret_cast<uint8 const*>(&CoordY);
							LastObservation[Index] = FloatNumberY[0];
							Index++;
							LastObservation[Index] = FloatNumberY[1];
							Index++;
							LastObservation[Index] = FloatNumberY[2];
							Index++;
							LastObservation[Index] = FloatNumberY[3];
							Index++;
						}
						break;
					}
				}
			}
			else	// HitList is empty
			{
				CoordX = -1.0;
				CoordY = -1.0;
				for (int w = 0; w < CellsRange; w++)
				{
					uint8 const* FloatNumberX;
					FloatNumberX = reinterpret_cast<uint8 const*>(&CoordX);
					LastObservation[Index] = FloatNumberX[0];
					Index++;
					LastObservation[Index] = FloatNumberX[1];
					Index++;
					LastObservation[Index] = FloatNumberX[2];
					Index++;
					LastObservation[Index] = FloatNumberX[3];
					Index++;

					uint8 const* FloatNumberY;
					FloatNumberY = reinterpret_cast<uint8 const*>(&CoordY);
					LastObservation[Index] = FloatNumberY[0];
					Index++;
					LastObservation[Index] = FloatNumberY[1];
					Index++;
					LastObservation[Index] = FloatNumberY[2];
					Index++;
					LastObservation[Index] = FloatNumberY[3];
					Index++;
				}
			}


		}
	}
	bIsBusy = false;
	return true;
}


