// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BonesRecognizer.h"
#include "ShapesRecognizer.h"
#include "TransRecognizer.h"
#include "GameFramework/Actor.h"
#include "GestureConfig.generated.h"

UCLASS()
class PICOGESTURE_API UGestureConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FShapesRecognizer ShapesRecognizer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FBonesRecognizer BonesRecognizer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FTransRecognizer TransRecognizer;

	UGestureConfig() = default;
};
