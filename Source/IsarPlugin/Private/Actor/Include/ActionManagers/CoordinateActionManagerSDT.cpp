#include "Actor/Include/ActionManagers/CoordinateActionManagerSDT.h"

ACoordinateActionManagerSDT::ACoordinateActionManagerSDT()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; //with respect to a sensor, an action manager can move only if "python sends" a command

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	SetRootComponent(Mesh);
	ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshAsset(TEXT("/SynDataToolbox/CoordinateActionManagerMesh"));  //TODO: USARE UNA MESH SPECIFICA
	Mesh->SetStaticMesh(StaticMeshAsset.Object);
	Mesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	CameraSpringArm->SetupAttachment(Mesh);
	CameraComponent->SetupAttachment(CameraSpringArm);
	CameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-45.0f, 0.0f, 0.0f));
	CameraSpringArm->TargetArmLength = 300.f;

	// Create hit
	ActorHit = new FHitResult();	// Initialize the hit info object
}

void ACoordinateActionManagerSDT::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

}

const FString ACoordinateActionManagerSDT::GetActionManagerName() const
{
	return "CoordinateActionManager("+GetActorLabel() + ")";
}

const FString ACoordinateActionManagerSDT::GetActionManagerSetup() const
{
	return FString("MOVETO@{}");
}

const int ACoordinateActionManagerSDT::ActionToID(const FString& Action) const
{
	if (Action == "MOVETO") return MOVETO;
	else return UNKNOWN;
}

const bool ACoordinateActionManagerSDT::InitSettings(const TArray<FString>& Settings)
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

const int8_t ACoordinateActionManagerSDT::PerformAction(TArray<FString>& ActionAndParameters)
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

void ACoordinateActionManagerSDT::Possess()
{
	UE_LOG(LogTemp, Warning, TEXT("Controlling: %s"), *GetActionManagerName())
	GetWorld()->GetFirstPlayerController()->Possess(this);
}

void ACoordinateActionManagerSDT::UnPossess()
{
	GetWorld()->GetFirstPlayerController()->UnPossess();
}

const int8_t ACoordinateActionManagerSDT::MoveTo(TArray<FString>& Parameters)
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

void ACoordinateActionManagerSDT::BeginPlay()
{
	Super::BeginPlay();
}

void ACoordinateActionManagerSDT::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
