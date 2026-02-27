// Fill out your copyright notice in the Description page of Project Settings.

#include "GestureConfigFactory.h"
#include "GestureConfig.h"

UGestureConfigFactory::UGestureConfigFactory()
{
	SupportedClass = UGestureConfig::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UGestureConfigFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UGestureConfig>(InParent, InClass, InName, Flags);
}

bool UGestureConfigFactory::ShouldShowInNewMenu() const
{
	return true;
}
