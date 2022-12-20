#include "Actor/Include/ActionManagers/CoordinateActionManagerIsar.h"

ACoordinateActionManagerIsar::ACoordinateActionManagerIsar()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; //with respect to a sensor, an action manager can move only if "python sends" a command

	//qui ha senso definire una shape di base
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("/IsarPlugin/SensorMesh"));  //TODO: USARE UNA MESH SPECIFICA
	Mesh->SetStaticMesh(DuplicateObject(StaticMesh.Object, NULL));
	SetRootComponent(Mesh);

	//create hit
	ActorHit = new FHitResult();  //init the info object
}

const FString ACoordinateActionManagerIsar::GetActionManagerName() const
{
	return "CoordinateActionManagerIsar("+GetActorLabel() + ")";
}

const int ACoordinateActionManagerIsar::ActionToID(const FString& Action) const
{
	if (Action == "MOVETO") return MOVETO;
	else return UNKNOWN;
}

const bool ACoordinateActionManagerIsar::InitSettings(const TArray<FString>& Settings)
{
	if ((Settings.Num() == 0))
	{
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Settings."));
		return false;
	}
}

const int8_t ACoordinateActionManagerIsar::PerformAction(TArray<FString>& ActionAndParameters)
{
	int8_t PerformActionCode;

	FString ActionName = ActionAndParameters[0];
	ActionAndParameters.RemoveAt(0);

	const int ActionID = ActionToID(ActionName);

	switch (ActionID)
	{
	case MOVETO:
	{
		PerformActionCode = MoveTo(ActionAndParameters);
	} break;
	default:
	{
		PerformActionCode = -1;
		UE_LOG(LogTemp, Error, TEXT("Unknown action: %s"), *ActionName);
	} break;
	}

	return PerformActionCode;
}

const int8_t ACoordinateActionManagerIsar::MoveTo(TArray<FString>& Parameters)
{
	const float X = FCString::Atof(*Parameters[0]);
	Parameters.RemoveAt(0);
	const float Y = FCString::Atof(*Parameters[0]);
	Parameters.RemoveAt(0);
	const float Z = FCString::Atof(*Parameters[0]);
	Parameters.RemoveAt(0);
	const float Pitch = FCString::Atof(*Parameters[0]);
	Parameters.RemoveAt(0);
	const float Yaw = FCString::Atof(*Parameters[0]);
	Parameters.RemoveAt(0);
	const float Roll = FCString::Atof(*Parameters[0]);
	Parameters.RemoveAt(0);

	const FVector NewLocation = FVector(X, Y, Z);
	const FRotator NewRotation = FRotator(Pitch, Yaw, Roll);
	SetActorLocationAndRotation(NewLocation, NewRotation, true, ActorHit);

	if (ActorHit->bBlockingHit)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void ACoordinateActionManagerIsar::BeginPlay()
{
	Super::BeginPlay();
}

void ACoordinateActionManagerIsar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
