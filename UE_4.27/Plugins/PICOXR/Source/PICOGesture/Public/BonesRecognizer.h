// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PXR_InputFunctionLibrary.h"
#include "UObject/Object.h"
#include "BonesRecognizer.generated.h"

USTRUCT(BlueprintType)
struct FBonesGroup
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EPICOXRHandJoint Bone1 = EPICOXRHandJoint::Wrist;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EPICOXRHandJoint Bone2 = EPICOXRHandJoint::Wrist;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Distance = 0.025f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ThresholdWidth = 0.003f;

	bool ActiveState;
};

USTRUCT(BlueprintType)
struct FBonesRecognizer
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FBonesGroup> Bones;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HoldDuration = 0.022f;
};
