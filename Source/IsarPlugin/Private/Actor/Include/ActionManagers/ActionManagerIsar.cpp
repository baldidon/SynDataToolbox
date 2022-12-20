// Fill out your copyright notice in the Description page of Project Settings.
#include "Actor/Include/ActionManagers/ActionManagerIsar.h"


const FString IActionManagerIsar::GetActionManagerName() const
{
	return FString();
}

const bool IActionManagerIsar::IsActionManagerName(const FString& Name) const
{
	if (Name == GetActionManagerName())
	{
		return true;
	}
	else
	{
		return false;
	}
}

const bool IActionManagerIsar::InitSettings(const TArray<FString>& Settings)
{
	return false;
}

const int8_t IActionManagerIsar::PerformAction(TArray<FString>& Action)
{
	return 0;
}

const int IActionManagerIsar::ActionToID(const FString& Action) const
{
	return 0;
}

void IActionManagerIsar::Possess()
{
}

void IActionManagerIsar::UnPossess()
{
}
