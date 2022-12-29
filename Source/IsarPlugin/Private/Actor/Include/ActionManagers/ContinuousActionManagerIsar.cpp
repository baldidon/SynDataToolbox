// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Include/ActionManagers/ContinuousActionManagerIsar.h"

// Sets default values
AContinuousActionManagerIsar::AContinuousActionManagerIsar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; //with respect to a sensor, an action manager can move only if "python sends" a command
	
	//qui ha senso definire una shape di base
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	SetRootComponent(Mesh);
	ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshAsset(TEXT("/IsarPlugin/SensorMesh"));
	Mesh->SetStaticMesh(StaticMeshAsset.Object);
	Mesh->SetRelativeScale3D(FVector(0.5f,0.5f,0.5f));
	CameraSpringArm->SetupAttachment(Mesh);
	CameraComponent->SetupAttachment(CameraSpringArm);
	
	// Create hit
	ActorHit = new FHitResult();	// Initialize the hit info object
	
	CameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-45.0f, 0.0f, 0.0f));
	CameraSpringArm->TargetArmLength = 300.f;
}

// Called when the game starts or when spawned
void AContinuousActionManagerIsar::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AContinuousActionManagerIsar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AContinuousActionManagerIsar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PrimaryActorTick.bCanEverTick = true; //with respect to a sensor, an action manager can move only if "python sends" a command
	//attatch all bind axis
	if (PlayerInputComponent) {
		//Hook up every-frame handling for our four axes
		PlayerInputComponent->BindAxis("STRAIGHT", this, &AContinuousActionManagerIsar::StraightCallback);
		PlayerInputComponent->BindAxis("TURN", this, &AContinuousActionManagerIsar::TurnCallback);
		PlayerInputComponent->BindAxis("UP", this, &AContinuousActionManagerIsar::UpCallback);
	}
}


const FString AContinuousActionManagerIsar::GetActionManagerName() const
{
	return "ContinuousActionManager("+GetActorLabel() + ")";
}

const FString AContinuousActionManagerIsar::GetActionManagerSetup() const
{
	return "TURN,STRAIGHT,GOUP@{}";
}

const int AContinuousActionManagerIsar::ActionToID(const FString& Action) const
{
	return 0;	if (Action == "TURN") return TURN;
	else if (Action == "STRAIGHT") return STRAIGHT;
	else if (Action == "GOUP") return GOUP;
	else return UNKNOWN;
}

const bool AContinuousActionManagerIsar::InitSettings(const TArray<FString>& Settings)
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

const int8_t AContinuousActionManagerIsar::PerformAction(TArray<FString>& Action)
{
	int8_t PerformActionCode;

	FString ActionName = Action[0];
	Action.RemoveAt(0);

	const int ActionID = ActionToID(ActionName);

	switch (ActionID)
	{
	case TURN:
	{
		PerformActionCode = Turn(Action);
	} break;
	case STRAIGHT:
	{
		PerformActionCode = GoStraight(Action);
	} break;
	case GOUP:
	{
		PerformActionCode = GoUp(Action);
	} break;
	default:
	{
		PerformActionCode = -1;
		UE_LOG(LogTemp, Error, TEXT("Unknown action: %s"), *ActionName);
	} break;
	}

	return PerformActionCode;
}

const int8_t AContinuousActionManagerIsar::Turn(TArray<FString>& ActionSettings)
{
	const float RotationSpeed = FCString::Atof(*ActionSettings[0]);
	ActionSettings.RemoveAt(0);

	FRotator CurrentActorRotation = GetActorRotation();
	CurrentActorRotation.Yaw = fmod(CurrentActorRotation.Yaw + RotationSpeed, 360);
	bool result = SetActorRotation(CurrentActorRotation);

	return 0;
}

void AContinuousActionManagerIsar::TurnCallback(float AxisValue)
{
	FRotator CurrentActorRotation = GetActorRotation();
	CurrentActorRotation.Yaw = fmod(CurrentActorRotation.Yaw + AxisValue, 360);
	SetActorRotation(CurrentActorRotation);
}

const int8_t AContinuousActionManagerIsar::GoStraight(TArray<FString>& ActionSettings)
{
	const float Speed = FCString::Atof(*ActionSettings[0]);
	ActionSettings.RemoveAt(0);

	FVector CurrentActorLocation = GetActorLocation();
	FRotator CurrentActorRotation = GetActorRotation();
	CurrentActorLocation.X += Speed * cos(CurrentActorRotation.Yaw * UKismetMathLibrary::GetPI() / 180.0) * 2;
	CurrentActorLocation.Y += Speed * sin(CurrentActorRotation.Yaw * UKismetMathLibrary::GetPI() / 180.0) * 2;
	SetActorLocation(CurrentActorLocation, true, ActorHit);

	if (ActorHit->bBlockingHit)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void AContinuousActionManagerIsar::StraightCallback(float AxisValue)
{
	FVector CurrentActorLocation = GetActorLocation();
	FRotator CurrentActorRotation = GetActorRotation();
	CurrentActorLocation.X += AxisValue * cos(CurrentActorRotation.Yaw * UKismetMathLibrary::GetPI() / 180.0) * 2;
	CurrentActorLocation.Y += AxisValue * sin(CurrentActorRotation.Yaw * UKismetMathLibrary::GetPI() / 180.0) * 2;
	SetActorLocation(CurrentActorLocation, true, ActorHit);
}

const int8_t AContinuousActionManagerIsar::GoUp(TArray<FString>& ActionSettings)
{
	const float Speed = FCString::Atof(*ActionSettings[0]);
	ActionSettings.RemoveAt(0);

	FVector CurrentActorLocation = GetActorLocation();
	CurrentActorLocation.Z += Speed;
	SetActorLocation(CurrentActorLocation, true, ActorHit);

	if (ActorHit->bBlockingHit)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void AContinuousActionManagerIsar::UpCallback(float AxisValue)
{
	FVector CurrentActorLocation = GetActorLocation();
	CurrentActorLocation.Z += AxisValue;
	SetActorLocation(CurrentActorLocation, true, ActorHit);
}

void AContinuousActionManagerIsar::Possess()
{
	GetWorld()->GetFirstPlayerController()->Possess(this);
}

void AContinuousActionManagerIsar::UnPossess()
{
	GetWorld()->GetFirstPlayerController()->UnPossess();
}
