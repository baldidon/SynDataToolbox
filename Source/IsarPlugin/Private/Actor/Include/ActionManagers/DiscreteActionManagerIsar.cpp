// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Include/ActionManagers/DiscreteActionManagerSDT.h"

ADiscreteActionManagerSDT::ADiscreteActionManagerSDT()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; //with respect to a sensor, an action manager can move only if "python sends" a command

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	SetRootComponent(Mesh);
	ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshAsset(TEXT("/SynDataToolbox/SensorMesh"));  
	Mesh->SetStaticMesh(StaticMeshAsset.Object);
	Mesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	CameraSpringArm->SetupAttachment(Mesh);
	CameraComponent->SetupAttachment(CameraSpringArm);
	CameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-45.0f, 0.0f, 0.0f));
	CameraSpringArm->TargetArmLength = 300.f;

	// Create hit
	ActorHit = new FHitResult();	// Initialize the hit info object
}

void ADiscreteActionManagerSDT::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PrimaryActorTick.bCanEverTick = false;
	if (PlayerInputComponent) {
		PlayerInputComponent->BindAction("TURNL",IE_Pressed, this, &ADiscreteActionManagerSDT::TurnLeftCallback);
		PlayerInputComponent->BindAction("TURNR",IE_Pressed, this, &ADiscreteActionManagerSDT::TurnRightCallback);
		PlayerInputComponent->BindAction("FORWARD", IE_Pressed, this, &ADiscreteActionManagerSDT::GoForwardCallback);
		PlayerInputComponent->BindAction("TURNLFORWARD", IE_Pressed, this, &ADiscreteActionManagerSDT::TurnLeftGoForwardCallback);
		PlayerInputComponent->BindAction("TURNRFORWARD", IE_Pressed, this, &ADiscreteActionManagerSDT::TurnRightGoForwardCallback);
		PlayerInputComponent->BindAction("TURNLBACKWARD", IE_Pressed, this, &ADiscreteActionManagerSDT::TurnLeftGoBackwardCallback);
		PlayerInputComponent->BindAction("TURNRBACKWARD", IE_Pressed, this, &ADiscreteActionManagerSDT::TurnRightGoBackwardCallback);
		PlayerInputComponent->BindAction("BACKWARD", IE_Pressed, this, &ADiscreteActionManagerSDT::GoBackwardCallback);
	}
}

void ADiscreteActionManagerSDT::BeginPlay()
{
	Super::BeginPlay();
}

void ADiscreteActionManagerSDT::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


const FString ADiscreteActionManagerSDT::GetActionManagerName() const
{
	return "DiscreteActionManager("+GetActorLabel() + ")";
}

const FString ADiscreteActionManagerSDT::GetActionManagerSetup() const
{
	return "TURNL,TURNR,FORWARD,TURNRFORWARD,TURNLFORWARD,TURNRBACKWARD,TURNLBACKWARD,BACKWARD,IDLE@{Speed:"+FString::SanitizeFloat(Speed)+",RotationSpeed:"+ FString::SanitizeFloat(Speed)+"}";
}

const int ADiscreteActionManagerSDT::ActionToID(const FString& Action) const
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

const bool ADiscreteActionManagerSDT::InitSettings(const TArray<FString>& Settings)
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

const int8_t ADiscreteActionManagerSDT::PerformAction(TArray<FString>& Action)
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



const int8_t ADiscreteActionManagerSDT::GoForward()
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

void ADiscreteActionManagerSDT::GoForwardCallback()
{
	FVector CurrentActorLocation = GetActorLocation();
	FRotator CurrentActorRotation = GetActorRotation();
	CurrentActorLocation.X += Speed * cos(CurrentActorRotation.Yaw * UKismetMathLibrary::GetPI() / 180.0);
	CurrentActorLocation.Y += Speed * sin(CurrentActorRotation.Yaw * UKismetMathLibrary::GetPI() / 180.0);
	SetActorLocation(CurrentActorLocation, true, ActorHit);
}

const int8_t ADiscreteActionManagerSDT::GoBackward()
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

void ADiscreteActionManagerSDT::GoBackwardCallback()
{
	GoBackward();
}


const int8_t ADiscreteActionManagerSDT::TurnRight()
{
	FRotator CurrentActorRotation = GetActorRotation();
	CurrentActorRotation.Yaw = fmod(CurrentActorRotation.Yaw + RotationSpeed, 360);
	SetActorRotation(CurrentActorRotation);

	return 0;
}

void ADiscreteActionManagerSDT::TurnRightCallback()
{
	TurnRight();
}

const int8_t ADiscreteActionManagerSDT::TurnLeft()
{
	FRotator CurrentActorRotation = GetActorRotation();
	CurrentActorRotation.Yaw = fmod(CurrentActorRotation.Yaw - RotationSpeed, 360);
	SetActorRotation(CurrentActorRotation);

	return 0;
}

void ADiscreteActionManagerSDT::TurnLeftCallback()
{
	FRotator CurrentActorRotation = GetActorRotation();
	CurrentActorRotation.Yaw = fmod(CurrentActorRotation.Yaw - RotationSpeed, 360);
	SetActorRotation(CurrentActorRotation);
}

const int8_t ADiscreteActionManagerSDT::TurnRightGoForward()
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

void ADiscreteActionManagerSDT::TurnRightGoForwardCallback()
{
	TurnRightGoForward();
}

const int8_t ADiscreteActionManagerSDT::TurnLeftGoForward()
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

void ADiscreteActionManagerSDT::TurnLeftGoForwardCallback()
{
	TurnLeftGoForward();
}

const int8_t ADiscreteActionManagerSDT::TurnRightGoBackward()
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

void ADiscreteActionManagerSDT::TurnRightGoBackwardCallback()
{
	TurnRightGoBackward();
}

const int8_t ADiscreteActionManagerSDT::TurnLeftGoBackward()
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

void ADiscreteActionManagerSDT::TurnLeftGoBackwardCallback()
{
	TurnLeftGoBackward();
}



void ADiscreteActionManagerSDT::Possess()
{
	UE_LOG(LogTemp,Warning, TEXT("Controlling: %s"),*GetActionManagerName())
	GetWorld()->GetFirstPlayerController()->Possess(this);
}

void ADiscreteActionManagerSDT::UnPossess()
{
	GetWorld()->GetFirstPlayerController()->UnPossess();
}