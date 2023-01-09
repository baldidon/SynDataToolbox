#include "Actor/Include/ActionManagers/DroneActionManagerSDT.h"
#include <Misc/OutputDeviceNull.h>

ADroneActionManagerSDT::ADroneActionManagerSDT()
{
	PrimaryActorTick.bCanEverTick = false; //with respect to a sensor, an action manager can move only if "python sends" a command

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	static ConstructorHelpers::FObjectFinder<UBlueprint> BPDrone(TEXT("Blueprint'/SynDataToolbox/PhysQuad/Blueprints/BP_Drone'"));
	if (BPDrone.Object) {
		PawnToSpawn = BPDrone.Object->GeneratedClass;
	}
	//create hit
	ActorHit = new FHitResult();  //init the info object
}
void ADroneActionManagerSDT::BeginPlay()
{
	Super::BeginPlay();

	//spawn blueprint
	if (PawnToSpawn) {
		Drone = GetWorld()->SpawnActor<APawn>(PawnToSpawn, GetActorLocation(), GetActorRotation());
	}
}


void ADroneActionManagerSDT::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


const FString ADroneActionManagerSDT::GetActionManagerName() const
{
	return "DroneActionManager("+GetActorLabel() + ")";
}

const FString ADroneActionManagerSDT::GetActionManagerSetup() const
{
	return FString("THROTTLE,PITCH,YAW,ROLL@{}");
}

const int ADroneActionManagerSDT::ActionToID(const FString& Action) const
{
	if (Action == "THROTTLE") return THROTTLE;
	else if (Action == "PITCH") return PITCH;
	if (Action == "YAW") return YAW;
	else if (Action == "ROLL") return ROLL;
	else return UNKNOWN;
}

const bool ADroneActionManagerSDT::InitSettings(const TArray<FString>& Settings)
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

const int8_t ADroneActionManagerSDT::PerformAction(TArray<FString>& Action)
{
	int8_t PerformActionCode;

	FString ActionName = Action[0];
	Action.RemoveAt(0);
	const int ActionID = ActionToID(ActionName);

	switch (ActionID)
	{
	case THROTTLE:
	{
		PerformActionCode = Throttle(Action);
	} break;
	case PITCH:
	{
		PerformActionCode = Pitch(Action);
	} break;
	case YAW:
	{
		PerformActionCode = Yaw(Action);
	} break;
	case ROLL:
	{
		PerformActionCode = Roll(Action);
	} break;
	default:
	{
		PerformActionCode = -1;
		UE_LOG(LogTemp, Error, TEXT("Unknown action: %s"), *ActionName);
	} break;
	}

	//Drone->Tick(0.0f);
	//Enabled->SetPropertyValue_InContainer(Drone, false);

	return PerformActionCode;
}

void ADroneActionManagerSDT::Possess()
{
	FBoolProperty* Manual = FindFProperty<FBoolProperty>(Drone->GetClass(), TEXT("Manual"));
	Manual->SetPropertyValue_InContainer(Drone, true);
	FBoolProperty* Enabled = FindFProperty<FBoolProperty>(Drone->GetClass(), TEXT("Enabled"));
	Enabled->SetPropertyValue_InContainer(Drone, true);

	FBoolProperty* Crashed = FindFProperty<FBoolProperty>(Drone->GetClass(), TEXT("Crashed"));
	if (Crashed->GetPropertyValue(Drone)) {
		Crashed->SetPropertyValue_InContainer(Drone, false);

	}
	UE_LOG(LogTemp, Warning, TEXT("Controlling: %s"), *GetActionManagerName())
	GetWorld()->GetFirstPlayerController()->Possess(Drone);
}

void ADroneActionManagerSDT::UnPossess()
{
	GetWorld()->GetFirstPlayerController()->UnPossess();

}

const int8_t ADroneActionManagerSDT::Throttle(TArray<FString>& ActionSettings) const
{
	const float Power = FCString::Atof(*ActionSettings[0]);
	ActionSettings.RemoveAt(0);

	FDoubleProperty* ThrottlePower = FindFProperty<FDoubleProperty>(Drone->GetClass(), TEXT("ThrottlePower"));
	ThrottlePower->SetPropertyValue_InContainer(Drone, Power);

	FBoolProperty* CrashedProperty = FindFProperty<FBoolProperty>(Drone->GetClass(), TEXT("Crashed"));
	const bool Crashed = CrashedProperty->GetPropertyValue_InContainer(Drone);

	if (Crashed)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


const int8_t ADroneActionManagerSDT::Pitch(TArray<FString>& ActionSettings) const
{
	const float Power = FCString::Atof(*ActionSettings[0]);
	ActionSettings.RemoveAt(0);

	FFloatProperty* PitchPower = FindFProperty<FFloatProperty>(Drone->GetClass(), TEXT("PitchPower"));
	PitchPower->SetPropertyValue_InContainer(Drone, Power);

	FBoolProperty* CrashedProperty = FindFProperty<FBoolProperty>(Drone->GetClass(), TEXT("Crashed"));
	const bool Crashed = CrashedProperty->GetPropertyValue_InContainer(Drone);

	FString Command;
	FOutputDeviceNull ar;

	if (Crashed)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


const int8_t ADroneActionManagerSDT::Yaw(TArray<FString>& ActionSettings) const
{
	const float Power = FCString::Atof(*ActionSettings[0]);
	ActionSettings.RemoveAt(0);

	FFloatProperty* YawPower = FindFProperty<FFloatProperty>(Drone->GetClass(), TEXT("YawPower"));
	YawPower->SetPropertyValue_InContainer(Drone, Power);


	FBoolProperty* CrashedProperty = FindFProperty<FBoolProperty>(Drone->GetClass(), TEXT("Crashed"));
	const bool Crashed = CrashedProperty->GetPropertyValue_InContainer(Drone);

	FString Command;
	FOutputDeviceNull ar;

	if (Crashed)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


const int8_t ADroneActionManagerSDT::Roll(TArray<FString>& ActionSettings) const
{
	const float Power = FCString::Atof(*ActionSettings[0]);
	ActionSettings.RemoveAt(0);

	FFloatProperty* RollPower = FindFProperty<FFloatProperty>(PawnToSpawn, TEXT("RollPower"));
	RollPower->SetPropertyValue_InContainer(Drone, Power);

	FBoolProperty* CrashedProperty = FindFProperty<FBoolProperty>(Drone->GetClass(), TEXT("Crashed"));
	const bool Crashed = CrashedProperty->GetPropertyValue_InContainer(Drone);

	FString Command;
	FOutputDeviceNull ar;

	if (Crashed)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

