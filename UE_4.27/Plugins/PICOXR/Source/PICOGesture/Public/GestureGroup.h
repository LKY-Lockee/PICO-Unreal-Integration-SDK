// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GestureGroup.generated.h"

class UGesture;
class UGestureGroupConfig;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGestureGroupDelegate);

DECLARE_MULTICAST_DELEGATE(FGestureGroupEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGestureGroupTickDelegate, float, Time);

DECLARE_MULTICAST_DELEGATE_OneParam(FGestureGroupTickEvent, float);

UENUM()
enum class EGroupTrackType
{
	Strict,
	Loose
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PICOGESTURE_API UGestureGroup : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Gesture")
	EGroupTrackType TrackType;

	UPROPERTY(EditDefaultsOnly, Category = "Gesture")
	UGestureGroupConfig* Config;

	UPROPERTY(BlueprintAssignable)
	FGestureGroupDelegate GestureStartDelegate;

	UPROPERTY(BlueprintAssignable)
	FGestureGroupTickDelegate GestureTickDelegate;

	UPROPERTY(BlueprintAssignable)
	FGestureGroupDelegate GestureEndDelegate;

private:
	UPROPERTY()
	UGesture* LeftGestureComponent;

	UPROPERTY()
	UGesture* RightGestureComponent;

	bool bLeftGestureActive = false;
	bool bRightGestureActive = false;
	bool bBothGesturesHold = false;
	float BothGesturesHoldTime = 0.0f;

	void GestureGroupEventCheck(float DeltaTime);

public:
	UGestureGroup();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void LeftActivate();
	UFUNCTION()
	void LeftDeactivate();
	UFUNCTION()
	void RightActivate();
	UFUNCTION()
	void RightDeactivate();

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetIsEnabled(bool bIsEnabled) const;
};
