// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Include/ActionManagers/DiscreteActionManagerIsar.h"

ADiscreteActionManagerIsar::ADiscreteActionManagerIsar()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; //with respect to a sensor, an action manager can move only if "python sends" a command

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	SetRootComponent(Mesh);
	ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshAsset(TEXT("/IsarPlugin/SensorMesh"));  //TODO: USARE UNA MESH SPECIFICA
	Mesh->SetStaticMesh(StaticMeshAsset.Object);
	Mesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	CameraSpringArm->SetupAttachment(Mesh);
	CameraComponent->SetupAttachment(CameraSpringArm);
	CameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-45.0f, 0.0f, 0.0f));
	CameraSpringArm->TargetArmLength = 300.f;

	// Create hit
	ActorHit = new FHitResult();	// Initialize the hit info object
}

void ADiscreteActionManagerIsar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PrimaryActorTick.bCanEverTick = false;
	if (PlayerInputComponent) {
		PlayerInputComponent->BindAction("TURNL",IE_Pressed, this, &ADiscreteActionManagerIsar::TurnLeftCallback);
		PlayerInputComponent->BindAction("TURNR",IE_Pressed, this, &ADiscreteActionManagerIsar::TurnRightCallback);
		PlayerInputComponent->BindAction("FORWARD", IE_Pressed, this, &ADiscreteActionManagerIsar::GoForwardCallback);
		PlayerInputComponent->BindAction("TURNLFORWARD", IE_Pressed, this, &ADiscreteActionManagerIsar::TurnLeftGoForwardCallback);
		PlayerInputComponent->BindAction("TURNRFORWARD", IE_Pressed, this, &ADiscreteActionManagerIsar::TurnRightGoForwardCallback);
		PlayerInputComponent->BindAction("TURNLBACKWARD", IE_Pressed, this, &ADiscreteActionManagerIsar::TurnLeftGoBackwardCallback);
		PlayerInputComponent->BindAction("TURNRBACKWARD", IE_Pressed, this, &ADiscreteActionManagerIsar::TurnRightGoBackwardCallback);
		PlayerInputComponent->BindAction("BACKWARD", IE_Pressed, this, &ADiscreteActionManagerIsar::GoBackwardCallback);
	}
}

void ADiscreteActionManagerIsar::BeginPlay()
{
	Super::BeginPlay();
}

void ADiscreteActionManagerIsar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


const FString ADiscreteActionManagerIsar::GetActionManagerName() const
{
	return "DiscreteActionManager("+GetActorLabel() + ")";
}

const FString ADiscreteActionManagerIsar::GetActionManagerSetup() const
{
	return "TURNL,TURNR,FORWARD,TURNRFORWARD,TURNLFORWARD,TURNRBACKWARD,TURNLBACKWARD,BACKWARD,IDLE@{Speed:"+FString::SanitizeFloat(Speed)+",RotationSpeed:"+ FString::SanitizeFloat(Speed)+"}";
}

const int ADiscreteActionManagerIsar::ActionToID(const FString& Action) const
{
	if (Action == "TURNL") return TURNL;
	else if (Action == "TURNR") return TURNR;
	else if (Action == "FORWARD") return FORWARD;
	else if (Action == "TURNRFORWARD") return TURNRFORWARD;
	else if (Action == "TURNLFORWARD") return TURNLFORWARD;
	else if (Action == "TURNRBACKWARD") return TURNRBACKWARD;
	else if (Action == "TURNLBACKWARD") return TURNLBACKWARD;
	else if (Action == "BACKWARD") return BACKWARD;
	else if (Action == "IDLE") return IDLE;
	else return UNKNOWN;
}

const bool ADiscreteActionManagerIsar::InitSettings(const TArray<FString>& Settings)
{
	if ((Settings.Num() == 2))
	{
		Speed = FCString::Atof(*Settings[0]);
		RotationSpeed = FCString::Atof(*Settings[1]);

		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Settings."));
		return false;
	}
}

const int8_t ADiscreteActionManagerIsar::PerformAction(TArray<FString>& Action)
{
	int8_t PerformActionCode;

	FString ActionName = Action[0];
	Action.RemoveAt(0);

	const int ActionID = ActionToID(ActionName);

	switch (ActionID)
	{
	case TURNL:
	{
		PerformActionCode = TurnLeft();
	} break;
	case TURNR:
	{
		PerformActionCode = TurnRight();
	} break;
	case FORWARD:
	{
		PerformActionCode = GoForward();
	} break;
	case BACKWARD:
	{
		PerformActionCode = GoBackward();
	} break;
	case TURNLFORWARD:
	{
		PerformActionCode = TurnLeftGoForward();
	} break;
	case TURNRFORWARD:
	{
		PerformActionCode = TurnRightGoForward();
	} break;
	case TURNLBACKWARD:
	{
		PerformActionCode = TurnLeftGoBackward();
	} break;
	case TURNRBACKWARD:
	{
		PerformActionCode = TurnRightGoBackward();
	} break;
	case IDLE:
	{
		PerformActionCode = 0;
	} break;
	default:
	{
		PerformActionCode = -1;
		UE_LOG(LogTemp, Error, TEXT("Unknown action: %s"), *ActionName);
	} break;
	}

	return PerformActionCode;
}



const int8_t ADiscreteActionManagerIsar::GoForward()
{
	FVector CurrentActorLocation = GetActorLocation();
	FRotator CurrentActorRotation = GetActorRotation();
	CurrentActorLocation.X += Speed * cos(CurrentActorRotation.Yaw * UKismetMathLibrary::GetPI() / 180.0);
	CurrentActorLocation.Y += Speed * sin(CurrentActorRotation.Yaw * UKismetMathLibrary::GetPI() / 180.0);
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

void ADiscreteActionManagerIsar::GoForwardCallback()
{
	FVector CurrentActorLocation = GetActorLocation();
	FRotator CurrentActorRotation = GetActorRotation();
	CurrentActorLocation.X += Speed * cos(CurrentActorRotation.Yaw * UKismetMathLibrary::GetPI() / 180.0);
	CurrentActorLocation.Y += Speed * sin(CurrentActorRotation.Yaw * UKismetMathLibrary::GetPI() / 180.0);
	SetActorLocation(CurrentActorLocation, true, ActorHit);
}

const int8_t ADiscreteActionManagerIsar::GoBackward()
{
	FVector CurrentActorLocation = GetActorLocation();
	FRotator CurrentActorRotation = GetActorRotation();
	CurrentActorLocation.X -= Speed * cos(CurrentActorRotation.Yaw * UKismetMathLibrary::GetPI() / 180.0);
	CurrentActorLocation.Y -= Speed * sin(CurrentActorRotation.Yaw * UKismetMathLibrary::GetPI() / 180.0);
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

void ADiscreteActionManagerIsar::GoBackwardCallback()
{
	GoBackward();
}


const int8_t ADiscreteActionManagerIsar::TurnRight()
{
	FRotator CurrentActorRotation = GetActorRotation();
	CurrentActorRotation.Yaw = fmod(CurrentActorRotation.Yaw + RotationSpeed, 360);
	SetActorRotation(CurrentActorRotation);

	return 0;
}

void ADiscreteActionManagerIsar::TurnRightCallback()
{
	TurnRight();
}

const int8_t ADiscreteActionManagerIsar::TurnLeft()
{
	FRotator CurrentActorRotation = GetActorRotation();
	CurrentActorRotation.Yaw = fmod(CurrentActorRotation.Yaw - RotationSpeed, 360);
	SetActorRotation(CurrentActorRotation);

	return 0;
}

void ADiscreteActionManagerIsar::TurnLeftCallback()
{
	FRotator CurrentActorRotation = GetActorRotation();
	CurrentActorRotation.Yaw = fmod(CurrentActorRotation.Yaw - RotationSpeed, 360);
	SetActorRotation(CurrentActorRotation);
}

const int8_t ADiscreteActionManagerIsar::TurnRightGoForward()
{
	FRotator CurrentActorRotation = GetActorRotation();
	CurrentActorRotation.Yaw = fmod(CurrentActorRotation.Yaw + RotationSpeed, 360);

	FVector CurrentActorLocation = GetActorLocation();
	CurrentActorLocation.X += Speed * cos(CurrentActorRotation.Yaw * UKismetMathLibrary::GetPI() / 180.0);
	CurrentActorLocation.Y += Speed * sin(CurrentActorRotation.Yaw * UKismetMathLibrary::GetPI() / 180.0);

	SetActorLocationAndRotation(CurrentActorLocation, CurrentActorRotation, true, ActorHit);

	if (ActorHit->bBlockingHit)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void ADiscreteActionManagerIsar::TurnRightGoForwardCallback()
{
	TurnRightGoForward();
}

const int8_t ADiscreteActionManagerIsar::TurnLeftGoForward()
{
	FRotator CurrentActorRotation = GetActorRotation();
	CurrentActorRotation.Yaw = fmod(CurrentActorRotation.Yaw - RotationSpeed, 360);

	FVector CurrentActorLocation = GetActorLocation();
	CurrentActorLocation.X += Speed * cos(CurrentActorRotation.Yaw * UKismetMathLibrary::GetPI() / 180.0);
	CurrentActorLocation.Y += Speed * sin(CurrentActorRotation.Yaw * UKismetMathLibrary::GetPI() / 180.0);

	SetActorLocationAndRotation(CurrentActorLocation, CurrentActorRotation, true, ActorHit);

	if (ActorHit->bBlockingHit)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void ADiscreteActionManagerIsar::TurnLeftGoForwardCallback()
{
	TurnLeftGoForward();
}

const int8_t ADiscreteActionManagerIsar::TurnRightGoBackward()
{
	FRotator CurrentActorRotation = GetActorRotation();
	CurrentActorRotation.Yaw = fmod(CurrentActorRotation.Yaw + RotationSpeed, 360);

	FVector CurrentActorLocation = GetActorLocation();
	CurrentActorLocation.X -= Speed * cos(CurrentActorRotation.Yaw * UKismetMathLibrary::GetPI() / 180.0);
	CurrentActorLocation.Y -= Speed * sin(CurrentActorRotation.Yaw * UKismetMathLibrary::GetPI() / 180.0);

	SetActorLocationAndRotation(CurrentActorLocation, CurrentActorRotation, true, ActorHit);

	if (ActorHit->bBlockingHit)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void ADiscreteActionManagerIsar::TurnRightGoBackwardCallback()
{
	TurnRightGoBackward();
}

const int8_t ADiscreteActionManagerIsar::TurnLeftGoBackward()
{
	FRotator CurrentActorRotation = GetActorRotation();
	CurrentActorRotation.Yaw = fmod(CurrentActorRotation.Yaw - RotationSpeed, 360);

	FVector CurrentActorLocation = GetActorLocation();
	CurrentActorLocation.X -= Speed * cos(CurrentActorRotation.Yaw * UKismetMathLibrary::GetPI() / 180.0);
	CurrentActorLocation.Y -= Speed * sin(CurrentActorRotation.Yaw * UKismetMathLibrary::GetPI() / 180.0);

	SetActorLocationAndRotation(CurrentActorLocation, CurrentActorRotation, true, ActorHit);

	if (ActorHit->bBlockingHit)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void ADiscreteActionManagerIsar::TurnLeftGoBackwardCallback()
{
	TurnLeftGoBackward();
}



void ADiscreteActionManagerIsar::Possess()
{
	GetWorld()->GetFirstPlayerController()->Possess(this);
}

void ADiscreteActionManagerIsar::UnPossess()
{
	GetWorld()->GetFirstPlayerController()->UnPossess();
}