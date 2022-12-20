#include "Actor/Include/ActionManagers/DroneActionManagerIsar.h"
#include <Misc/OutputDeviceNull.h>

ADroneActionManagerIsar::ADroneActionManagerIsar() 
{
	PrimaryActorTick.bCanEverTick = false; //with respect to a sensor, an action manager can move only if "python sends" a command

	//qui ha senso definire una shape di base
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	static ConstructorHelpers::FObjectFinder<UBlueprint> BPDrone(TEXT("Blueprint'/IsarPlugin/PhysQuad/Blueprints/BP_Drone'"));
	if (BPDrone.Object) {
		PawnToSpawn = BPDrone.Object->GeneratedClass;
	}
	//create hit
	ActorHit = new FHitResult();  //init the info object
}
void ADroneActionManagerIsar::BeginPlay()
{
	Super::BeginPlay();

	//spawn blueprint
	if (PawnToSpawn) {
		Drone = GetWorld()->SpawnActor<APawn>(PawnToSpawn, GetActorLocation(), GetActorRotation());
	}
}

//void ADroneActionManagerIsar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	if (PlayerInputComponent) {
//		Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//		FBoolProperty *Manual = FindFProperty<FBoolProperty>(Drone->GetClass(), TEXT("Manual"));
//		Manual->SetPropertyValue_InContainer(Drone, true);
//		FBoolProperty* Enabled = FindFProperty<FBoolProperty>(Drone->GetClass(), TEXT("Enabled"));
//		Enabled->SetPropertyValue_InContainer(Drone, true);
//
//		FBoolProperty* Crashed = FindFProperty<FBoolProperty>(Drone->GetClass(), TEXT("Crashed"));
//		if (Crashed->GetPropertyValue(Drone)) {
//			Crashed->SetPropertyValue_InContainer(Drone, false);
//
//		}
//		//PlayerInputComponent->BindAxis(FName("Throttle"), this, &ADroneActionManagerIsar::ThrottleCallback);
//		//PlayerInputComponent->BindAxis(FName("Pitch"), this, &ADroneActionManagerIsar::PitchCallback);
//		//PlayerInputComponent->BindAxis(FName("Yaw"), this, &ADroneActionManagerIsar::YawCallback);
//		//PlayerInputComponent->BindAxis(FName("Roll"), this, &ADroneActionManagerIsar::RollCallback);
//	}
//}

void ADroneActionManagerIsar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


const FString ADroneActionManagerIsar::GetActionManagerName() const
{
	return "DroneActionManager("+GetActorLabel() + ")";
}

const int ADroneActionManagerIsar::ActionToID(const FString& Action) const
{
	if (Action == "THROTTLE") return THROTTLE;
	else if (Action == "PITCH") return PITCH;
	if (Action == "YAW") return YAW;
	else if (Action == "ROLL") return ROLL;
	else return UNKNOWN;
}

const bool ADroneActionManagerIsar::InitSettings(const TArray<FString>& Settings)
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

const int8_t ADroneActionManagerIsar::PerformAction(TArray<FString>& Action)
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

void ADroneActionManagerIsar::Possess()
{
	FBoolProperty* Manual = FindFProperty<FBoolProperty>(Drone->GetClass(), TEXT("Manual"));
	Manual->SetPropertyValue_InContainer(Drone, true);
	FBoolProperty* Enabled = FindFProperty<FBoolProperty>(Drone->GetClass(), TEXT("Enabled"));
	Enabled->SetPropertyValue_InContainer(Drone, true);

	FBoolProperty* Crashed = FindFProperty<FBoolProperty>(Drone->GetClass(), TEXT("Crashed"));
	if (Crashed->GetPropertyValue(Drone)) {
		Crashed->SetPropertyValue_InContainer(Drone, false);

	}
	GetWorld()->GetFirstPlayerController()->Possess(Drone);
}

void ADroneActionManagerIsar::UnPossess()
{
	GetWorld()->GetFirstPlayerController()->UnPossess();

}

const int8_t ADroneActionManagerIsar::Throttle(TArray<FString>& ActionSettings) const
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

//void ADroneActionManagerIsar::ThrottleCallback(float Power) {
//	if (Drone) {
//		//FBoolProperty *Enabled = FindFProperty<FBoolProperty>(Drone->GetClass(), TEXT("Enabled"));
//		//Enabled->SetPropertyValue_InContainer(Drone, true);
//		FDoubleProperty* ThrottlePower = FindFProperty<FDoubleProperty>(Drone->GetClass(), TEXT("ThrottlePower"));
//		ThrottlePower->SetPropertyValue_InContainer(Drone, Power);
//		UE_LOG(LogTemp, Error, TEXT("Power %f"), ThrottlePower->GetPropertyValue_InContainer(Drone))
//		//Enabled->SetPropertyValue_InContainer(Drone, false);
//	}
//}

const int8_t ADroneActionManagerIsar::Pitch(TArray<FString>& ActionSettings) const
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

//void ADroneActionManagerIsar::PitchCallback(float PitchValue)
//{
//	FDoubleProperty* Power = FindFProperty<FDoubleProperty>(Drone->GetClass(), TEXT("PitchPower"));
//	Power->SetPropertyValue_InContainer(Drone, PitchValue);
//	UE_LOG(LogTemp, Error, TEXT("Pitch %f"), Power->GetPropertyValue_InContainer(Drone))
//}

const int8_t ADroneActionManagerIsar::Yaw(TArray<FString>& ActionSettings) const
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

//void ADroneActionManagerIsar::YawCallback(float YawValue)
//{
//	FDoubleProperty* Power = FindFProperty<FDoubleProperty>(Drone->GetClass(), TEXT("YawPower"));
//	Power->SetPropertyValue_InContainer(Drone, YawValue);
//	UE_LOG(LogTemp, Error, TEXT("Yaw %f"), Power->GetPropertyValue_InContainer(Drone))
//}

const int8_t ADroneActionManagerIsar::Roll(TArray<FString>& ActionSettings) const
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

//void ADroneActionManagerIsar::RollCallback(float RollValue)
//{
//
//	FDoubleProperty* Power = FindFProperty<FDoubleProperty>(Drone->GetClass(), TEXT("RollPower"));
//	Power->SetPropertyValue_InContainer(Drone, RollValue);
//	UE_LOG(LogTemp, Error, TEXT("Roll %f"), Power->GetPropertyValue_InContainer(Drone))
//}

