// Fill out your copyright notice in the Description page of Project Settings.
#include "Actor/Include/ActionManagers/ActionManagerSDT.h"


const FString IActionManagerSDT::GetActionManagerName() const
{
	return FString();
}

const FString IActionManagerSDT::GetActionManagerSetup() const
{
	//in this method are stored commands and other setup info
	return FString("@{}");
}

const bool IActionManagerSDT::IsActionManagerName(const FString& Name) const
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

const bool IActionManagerSDT::InitSettings(const TArray<FString>& Settings)
{
	return false;
}

const int8_t IActionManagerSDT::PerformAction(TArray<FString>& Action)
{
	return 0;
}

const int IActionManagerSDT::ActionToID(const FString& Action) const
{
	return 0;
}

void IActionManagerSDT::Possess()
{
}

void IActionManagerSDT::UnPossess()
{
}
