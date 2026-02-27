// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TransRecognizer.generated.h"

UENUM()
enum class ESpaceType
{
	WorldSpace,
	LocalXY,
	LocalYZ,
	LocalXZ
};

UENUM()
enum class ETrackAxis
{
	Fingers,
	Palm,
	Thumb
};

UENUM()
enum class ETrackTarget
{
	TowardsFace,
	AwayFromFace,
	WorldUp,
	WorldDown,
};

USTRUCT(BlueprintType)
struct FTransRecognizer
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ETrackAxis TrackAxis;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ESpaceType SpaceType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ETrackTarget TrackTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AngleThreshold = 35;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ThresholdWidth = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HoldDuration = 0.022f;
};
