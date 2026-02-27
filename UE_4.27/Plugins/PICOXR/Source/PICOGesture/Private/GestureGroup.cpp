// Fill out your copyright notice in the Description page of Project Settings.

#include "GestureGroup.h"

#include "Gesture.h"
#include "GestureGroupConfig.h"

UGestureGroup::UGestureGroup()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGestureGroup::BeginPlay()
{
	Super::BeginPlay();

	if (!Config)
	{
		return;
	}

	LeftGestureComponent = NewObject<UGesture>(this->GetOuter());
	if (LeftGestureComponent)
	{
		LeftGestureComponent->Config = Config->LeftGesture;
		LeftGestureComponent->RegisterComponent();
		LeftGestureComponent->GestureStartDelegate.AddDynamic(this, &UGestureGroup::LeftActivate);
		LeftGestureComponent->GestureEndDelegate.AddDynamic(this, &UGestureGroup::LeftDeactivate);
		if (TrackType == EGroupTrackType::Strict)
		{
			LeftGestureComponent->TrackType = ETrackType::Left;
		}
		LeftGestureComponent->Init();
	}
	RightGestureComponent = NewObject<UGesture>(this->GetOuter());
	if (RightGestureComponent)
	{
		RightGestureComponent->Config = Config->RightGesture;
		RightGestureComponent->RegisterComponent();
		RightGestureComponent->GestureStartDelegate.AddDynamic(this, &UGestureGroup::RightActivate);
		RightGestureComponent->GestureEndDelegate.AddDynamic(this, &UGestureGroup::RightDeactivate);
		if (TrackType == EGroupTrackType::Strict)
		{
			RightGestureComponent->TrackType = ETrackType::Right;
		}
		RightGestureComponent->Init();
	}
}

void UGestureGroup::LeftActivate()
{
	bLeftGestureActive = true;
}

void UGestureGroup::LeftDeactivate()
{
	bLeftGestureActive = false;
}

void UGestureGroup::RightActivate()
{
	bRightGestureActive = true;
}

void UGestureGroup::RightDeactivate()
{
	bRightGestureActive = false;
}

void UGestureGroup::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	GestureGroupEventCheck(DeltaTime);
}

void UGestureGroup::SetIsEnabled(bool bIsEnabled) const
{
	LeftGestureComponent->bIsEnabled = bIsEnabled;
	RightGestureComponent->bIsEnabled = bIsEnabled;
}

void UGestureGroup::GestureGroupEventCheck(float DeltaTime)
{
	bool bBothGesturesActive = bLeftGestureActive && bRightGestureActive;

	if (bBothGesturesActive != bBothGesturesHold)
	{
		bBothGesturesHold = bBothGesturesActive;
		if (bBothGesturesHold)
		{
			GestureStartDelegate.Broadcast();
		}
		else
		{
			GestureEndDelegate.Broadcast();
		}
		BothGesturesHoldTime = 0.0f;
	}
	else if (bBothGesturesHold)
	{
		BothGesturesHoldTime += DeltaTime;
		GestureTickDelegate.Broadcast(BothGesturesHoldTime);
	}
}
