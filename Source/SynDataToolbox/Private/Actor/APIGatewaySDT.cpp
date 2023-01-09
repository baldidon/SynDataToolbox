// Fill out your copyright notice in the Description page of Project Settings.
#include "Actor/APIGatewaySDT.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AAPIGatewaySDT::AAPIGatewaySDT()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	HeroSocket = MakeShared<UnrealSocketSDT>();
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

// Called when the game starts or when spawned
void AAPIGatewaySDT::BeginPlay()
{
	Super::BeginPlay();
	//because ActorHero now can manipulate only "object" spawned into level.
	FetchSDTObjects();
	gameViewport = GEngine->GameViewport;
	gameViewport->bDisableWorldRendering = 1-showRender;
	FTimerHandle FPSTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(FPSTimerHandle, this, &AAPIGatewaySDT::ShowFrameRateOnScreen, 1.0f, true);
	CreateConnection();
}

const int AAPIGatewaySDT::CommandToID(const FString& command) const
{
	if (command == "CHANGE") return CHANGE;
	else if (command == "OBS") return OBS;
	else if (command == "CLOSE") return CLOSE;
	else if (command == "SENSORS") return SENSORS;
	else if (command == "RENDER") return RENDER;
	else if (command == "ACTIONS") return ACTIONS;
	else if (command == "SETACTIONMAN") return SETACTIONMAN;
	else if (command == "ACTION") return ACTION;
	else if (command == "RESETS") return RESETS;
	else if (command == "SETRESETMAN") return SETRESETMAN;
	else if (command == "RESET") return RESET;
	else if (command == "FPS") return FPS;
	else return UNKNOWN;
}

void AAPIGatewaySDT::ShowFrameRateOnScreen()
{
	if (ShowFPS)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Yellow, "FPS_" + GetName() + ": " + FString::FromInt(FPSCounter));
	}
	FPSCounter = 0;
}

// Called every frame
void AAPIGatewaySDT::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called every frame
void AAPIGatewaySDT::RepeatingFunction()
{
	const FString PythonCommand = HeroSocket->TCPSocketListener();
	if (PythonCommand != "")
	{
		FPSCounter++;
		ParseCommand(PythonCommand);
	}
}

void AAPIGatewaySDT::CreateConnection()
{
	if (HeroSocket->StartTCPReceiver(port))
	{
		//Start the Listener //thread this eventually
		GetWorldTimerManager().SetTimer(TimerTCPConnectionListener, this, &AAPIGatewaySDT::TCPConnectionListener, 0.01, true);
	}
}

void AAPIGatewaySDT::TCPConnectionListener()
{
	if (HeroSocket->TCPConnectionListener())
	{

		//can thread this too
		GetWorldTimerManager().SetTimer(TimerRepeatingFunction, this, &AAPIGatewaySDT::RepeatingFunction, 0.01, true);
	}
}

const bool AAPIGatewaySDT::CloseConnection()
{
	HeroSocket->CloseConnectionSocket();
	UE_LOG(LogTemp, Warning, TEXT("Connection closed on port: %i"), port);
	return true;
}

const bool AAPIGatewaySDT::SendSensorsList()
{
	//sends a list of just istanciated sensors
	FString SensorNameList = "";
	for(auto Sensor : SensorsList)
	{
		const FString SensorName = Sensor->GetSensorName();
		SensorNameList += SensorName;
		SensorNameList += "#"+FString::FromInt(Sensor->GetObservationSize())+"@"+Sensor->GetSensorSetup() + " ";
	}
	const int32 StringLength = SensorNameList.Len();
	HeroSocket->SendBytes(TCHAR_TO_ANSI(*SensorNameList), StringLength);

	return true;
}

const bool AAPIGatewaySDT::SendActionManagersList()
{
	FString ActionManagersNameList = "";

	for(IActionManagerSDT* ActionManager: ActionManagersList)
	{
		const FString ActionManagerName = ActionManager->GetActionManagerName();
		ActionManagersNameList += ActionManagerName;
		ActionManagersNameList += "#" + ActionManager->GetActionManagerSetup()+" ";
	}
	const int32 StringLength = ActionManagersNameList.Len();
	HeroSocket->SendBytes(TCHAR_TO_ANSI(*ActionManagersNameList), StringLength);

	return true;
}

const bool AAPIGatewaySDT::SendResetManagersList()
{
	FString ResetManagersNameList = "";

	for(IResetManagerSDT* ResetManager : ResetManagersList)
	{
		const FString ResetManagerName = ResetManager->GetResetManagerName();
		ResetManagersNameList += ResetManagerName;
		ResetManagersNameList += " ";
	}
	const int32 StringLength = ResetManagersNameList.Len();

	HeroSocket->SendBytes(TCHAR_TO_ANSI(*ResetManagersNameList), StringLength);

	return true;
}


const bool AAPIGatewaySDT::SetCurrentResetManager(TArray<FString>& FieldArray)
{
	bool bSetCurrentResetManagerSuccessful = false;

	const FString ResetManagerName = FieldArray[0];
	FieldArray.RemoveAt(0);
	for(auto ResetManager:ResetManagersList)
	{
		if (ResetManager->IsResetManagerName(ResetManagerName))
		{
			CurrentResetManager = ResetManager;
			bSetCurrentResetManagerSuccessful = CurrentResetManager->ChangeResetSettings(FieldArray);
			break;
		}
	}

	return bSetCurrentResetManagerSuccessful;
}

const bool AAPIGatewaySDT::ChangeSensorsSettings(TArray<FString>& FieldArray) const
{
	bool bChangeSensorsSettingsSuccessful = false;

	const FString SensorName = FieldArray[0];
	FString new_buf = "";
	FieldArray.RemoveAt(0);
	for(auto Sensor: SensorsList)
	{
		if (Sensor->IsSensorName(SensorName))
		{
			bChangeSensorsSettingsSuccessful = Sensor->ChangeSensorSettings(FieldArray);
			if (bChangeSensorsSettingsSuccessful) {
				new_buf = "";
				new_buf = FString::FromInt(Sensor->GetObservationSize());
				int32 StringLength = new_buf.Len();
				HeroSocket->SendBytes(TCHAR_TO_ANSI(*new_buf), StringLength);
			}
			break;
		}
	}
	return bChangeSensorsSettingsSuccessful;
}

const bool AAPIGatewaySDT::GetSensorsObs(TArray<FString>& FieldArray)
{
	bool bGetSensorsObsSuccessful = false;

	const FString SensorName = FieldArray[0];
	FieldArray.RemoveAt(0);
	for(auto Sensor: SensorsList)
	{
		if (Sensor->IsSensorName(SensorName))
		{
			const uint32 size = Sensor->GetObservationSize();
			uint8* const Buffer = new uint8[size];
			bGetSensorsObsSuccessful = Sensor->GetLastObs(Buffer);
			if (bGetSensorsObsSuccessful)
			{
				HeroSocket->SendObsBytes(Buffer, size);
			}
			delete[] Buffer;
			break;
		}
	}

	return bGetSensorsObsSuccessful;
}

const bool AAPIGatewaySDT::PerformAction(TArray<FString>& FieldArray)
{
	bool bPerformActionSuccessful = true;
	bool bHit = false;

	const FString ActionManagerName = FieldArray[0];
	FieldArray.RemoveAt(0);
	FString Command = FieldArray[0];
	for (auto ActionManager : ActionManagersList) {
		if (ActionManager->IsActionManagerName(ActionManagerName))
		{
			TArray<FString> Action;
			Command.ParseIntoArray(Action, TEXT(";"), true);
			const int8_t PerformActionCode = ActionManager->PerformAction(Action);
			if (PerformActionCode == -1)
			{
				bPerformActionSuccessful = false;
			}
			if (bPerformActionSuccessful)
			{
				if (PerformActionCode != 0)
				{
					bHit = true;
				}
			}
			HeroSocket->SendBytes(&bHit, 1);
			break;
		}
		break;
	}
	return bPerformActionSuccessful;
}

const bool AAPIGatewaySDT::PerformReset(TArray<FString>& FieldArray)
{
	return CurrentResetManager->PerformReset(FieldArray);
}

void AAPIGatewaySDT::ParseCommand(const FString PythonCommand)
{
	TArray<FString> CommandArray;
	PythonCommand.ParseIntoArray(CommandArray, TEXT(" "), true);

	for (auto Command = CommandArray.CreateConstIterator(); Command; ++Command)
	{
		TArray<FString> FieldArray;
		Command->ParseIntoArray(FieldArray, TEXT("_"), true);

		const FString CommandName = FieldArray[0];
		FieldArray.RemoveAt(0);
		const int FieldID = CommandToID(CommandName);

		bool bValidCommand = false;

		switch (FieldID)
		{
		case SENSORS:
		{
			bValidCommand = SendSensorsList();
			if (!bValidCommand) UE_LOG(LogTemp, Error, TEXT("SENSORS"));
		} break;
		case ACTIONS:
		{
			bValidCommand = SendActionManagersList();
			if (!bValidCommand) UE_LOG(LogTemp, Error, TEXT("ACTIONS"));
		} break;
		case RESETS:
		{
			bValidCommand = SendResetManagersList();
			if (!bValidCommand) UE_LOG(LogTemp, Error, TEXT("RESETS"));
		} break;
		//case SETACTIONMAN:
		//{
		//	bValidCommand = SetCurrentActionManager(FieldArray);
		//	if (!bValidCommand) UE_LOG(LogTemp, Error, TEXT("SETACTIONMAN"));
		//} break;
		case SETRESETMAN:
		{
			bValidCommand = SetCurrentResetManager(FieldArray);
			if (!bValidCommand) UE_LOG(LogTemp, Error, TEXT("SETRESETMAN"));
		} break;
		case CHANGE:
		{
			bValidCommand = ChangeSensorsSettings(FieldArray);
			if (!bValidCommand) UE_LOG(LogTemp, Error, TEXT("CHANGE"));
		} break;
		case OBS:
		{
			bValidCommand = GetSensorsObs(FieldArray);
			if (!bValidCommand) UE_LOG(LogTemp, Error, TEXT("OBS"));
		} break;
		case ACTION:
		{
			bValidCommand = PerformAction(FieldArray);
			if (!bValidCommand) UE_LOG(LogTemp, Error, TEXT("ACTION"));
		} break;
		case RESET:
		{
			bValidCommand = PerformReset(FieldArray);
			if (!bValidCommand) UE_LOG(LogTemp, Error, TEXT("RESET"));
		} break;
		case RENDER:
		{
			gameViewport->bDisableWorldRendering = 0;
			bValidCommand = true;
		} break;
		case FPS:
		{
			ShowFPS = true;
			bValidCommand = true;
		} break;
		case CLOSE:
		{
			bValidCommand = CloseConnection();
			if (!bValidCommand) UE_LOG(LogTemp, Error, TEXT("CLOSE"));
		} break;
		default:
		{
			UE_LOG(LogTemp, Error, TEXT("Unknown command: %s"), *CommandName);
		} break;
		}
		if (!bValidCommand)
		{
			UE_LOG(LogTemp, Error, TEXT("CLOSED"));
			CloseConnection();
			return;
		}
	}
}

void AAPIGatewaySDT::FetchSDTObjects()
{
	//search into level istanciated object like sensors; actionManagers; ResetManagers
	if (GetWorld() != nullptr) 
	{
		for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			//reducing "castable" elements
			if (ActorItr->GetName().Contains("SDT")) {
				ISensorSDT* S = Cast<ISensorSDT>(*ActorItr);
				if (S) {
					SensorsList.Add(S);
					UE_LOG(LogTemp, Warning, TEXT("Found new Sensor!   :%s"), *S->GetSensorName())
				}
				else {
					IActionManagerSDT* A = Cast<IActionManagerSDT>(*ActorItr);
					if (A) {
						ActionManagersList.Add(A);
						if (ActionManagersList.Num() == 1 && DebugActionManager) {
							UE_LOG(LogTemp, Warning, TEXT("DebugActionManager: Possess %s"), *A->GetActionManagerName())

						}
						UE_LOG(LogTemp, Warning, TEXT("Found new ActionManager!   :%s"), *A->GetActionManagerName())
					}
					else {
						IResetManagerSDT* R = Cast<IResetManagerSDT>(*ActorItr);
						ResetManagersList.Add(R);					
						UE_LOG(LogTemp, Warning, TEXT("Found new ResetManager!   :%s"), *R->GetResetManagerName())
					}
				}
			}
		}
	}
	if (DebugActionManager && ActionManagersList.Num()>=1){
		AActionManagerController* PlayerManager = Cast<AActionManagerController>(GetWorld()->GetFirstPlayerController());
		if (!PlayerManager) {
			UE_LOG(LogTemp, Fatal, TEXT("Debug ActionManager is on but GameMode's PlayerController is not set"))
		}
		else {
			PlayerManager->SetupActorListToPossess(ActionManagersList);
		}
	}

}

