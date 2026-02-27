// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GestureGroupConfig.generated.h"

class UGestureConfig;

UCLASS()
class PICOGESTURE_API UGestureGroupConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UGestureConfig* LeftGesture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UGestureConfig* RightGesture;

	UGestureGroupConfig() = default;
};
