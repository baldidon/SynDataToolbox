#include "Actor/Include/LevelManager/ActionManagerController.h"
#include <Actor/Include/ActionManagers/DroneActionManagerIsar.h>

void AActionManagerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	ActionManagers = TArray<IActionManagerIsar*>();
	InputComponent->BindAction("Switch_Up", IE_Pressed, this, &AActionManagerController::SwitchUp);
	InputComponent->BindAction("Switch_Down", IE_Pressed, this, &AActionManagerController::SwitchDown);

}

void AActionManagerController::SwitchUp()
{
	if (!ActionManagers.IsEmpty()) {
		int PreviousIndexActionManager = IndexCurrentActionManager;
		if (IndexCurrentActionManager < ActionManagers.Num() - 1) {
			IndexCurrentActionManager += 1;
		}
		else {
			IndexCurrentActionManager = 0;
		}
		if (PreviousIndexActionManager != -1) {
			ActionManagers[PreviousIndexActionManager]->UnPossess();
		}
		ActionManagers[IndexCurrentActionManager]->Possess();
	}
}	

void AActionManagerController::SwitchDown()
{
	if (!ActionManagers.IsEmpty()) {
		int PreviousIndexActionManager = IndexCurrentActionManager;
		if (IndexCurrentActionManager > 0) {
			IndexCurrentActionManager -= 1;
		}
		else {
			IndexCurrentActionManager = ActionManagers.Num() - 1;
		}
		if (PreviousIndexActionManager != -1) {
			ActionManagers[PreviousIndexActionManager]->UnPossess();
		}
		ActionManagers[IndexCurrentActionManager]->Possess();
	}
}

void AActionManagerController::SetupActorListToPossess(TArray<IActionManagerIsar*> ActionManagerList)
{
	for (auto AManager : ActionManagerList) {
		ActionManagers.Add(AManager);
	}
}