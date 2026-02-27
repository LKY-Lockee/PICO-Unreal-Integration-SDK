// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GestureConfig.h"
#include "PXR_Plugin_Types.h"
#include "Components/ActorComponent.h"
#include "Gesture.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGestureDelegate);

DECLARE_MULTICAST_DELEGATE(FGestureEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGestureTickDelegate, float, Time);

DECLARE_MULTICAST_DELEGATE_OneParam(FGestureTickEvent, float);

UENUM()
enum class ETrackType
{
	Any,
	Left,
	Right
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PICOGESTURE_API UGesture : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Gesture")
	ETrackType TrackType;

	UPROPERTY(EditDefaultsOnly, Category = "Gesture")
	UGestureConfig* Config;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gesture")
	bool bIsEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gesture")
	bool bIsDebug;

	UPROPERTY(BlueprintAssignable)
	FGestureDelegate GestureStartDelegate;

	UPROPERTY(BlueprintAssignable)
	FGestureTickDelegate GestureTickDelegate;

	UPROPERTY(BlueprintAssignable)
	FGestureDelegate GestureEndDelegate;

	UGesture();

private:
	UPROPERTY()
	TArray<FVector> LeftJointPos;

	UPROPERTY()
	TArray<FVector> RightJointPos;

	PxrHandJointsLocations LeftHandJointLocations;
	PxrHandJointsLocations RightHandJointLocations;

	// ShapesRecognizer
	float ShapesHoldDuration = 0.09f;

	bool bLeftShapesHold = false;
	bool bLeftShapesActive = false;
	float LeftShapesHoldTime = 0.0f;

	bool bRightShapesActive = false;
	bool bRightShapesHold = false;
	float RightShapesHoldTime = 0.0f;

	bool bAngleCheckValid = false;
	bool bAbducCheckOpen = false;

	FVector LeftWristPos;
	FVector RightWristPos;
	FQuat LeftWristRot;
	FQuat RightWristRot;

	FVector Thumb0, Thumb1, Thumb2, Thumb3;
	FVector Index0, Index1, Index2, Index3;
	FVector Middle0, Middle1, Middle2, Middle3;
	FVector Ring0, Ring1, Ring2, Ring3;
	FVector Pinky0, Pinky1, Pinky2, Pinky3;

	bool bThumbFlex, bIndexFlex, bMiddleFlex, bRingFlex, bPinkyFlex;
	bool bThumbCurl, bIndexCurl, bMiddleCurl, bRingCurl, bPinkyCurl;
	bool bThumbAbduc, bIndexAbduc, bMiddleAbduc, bRingAbduc, bPinkyAbduc;

	// BonesRecognizer
	TArray<FBonesGroup> Bones;
	bool bLeftBonesHold = false;
	bool bLeftBonesActive = false;
	float LeftBonesHoldTime = 0.0f;

	bool bRightBonesHold = false;
	bool bRightBonesActive = false;
	float RightBonesHoldTime = 0.0f;

	float BonesHoldDuration = 0.0f;

	bool bPoseStateHold = false;
	bool bPoseStateActive = false;
	float PoseStateHoldTime = 0.0f;

	// TransRecognizer
	bool bLeftTransHold = false;
	bool bLeftTransActive = false;
	float LeftTransHoldTime = 0.0f;

	bool bRightTransHold = false;
	bool bRightTransActive = false;
	float RightTransHoldTime = 0.0f;

	ETrackAxis TransTrackAxis;
	ESpaceType TransSpaceType;
	ETrackTarget TransTrackTarget;

	float TransAngleThreshold = 0.0f;
	float TransThresholdWidth = 0.0f;
	float TransHoldDuration = 0.0f;

	FVector HMDPose;
	FVector PalmPos;
	FVector PalmAxis;
	FVector TargetPos;

	uint64 DebugKey = 1;

	void GestureEventCheck(float DeltaTime);
	bool HoldCheck(bool bHoldState, float HoldDuration, bool bResultState, float& HoldTime) const;
	float GetAngle(FVector VectorA, FVector VectorB) const;

	// ShapesRecognizer
	bool ShapesRecognizerCheck(TArray<FVector>& JointPos, FVector WristRight, FVector WristForward, int WristDirect = 1);
	bool FlexionCheck(const FFinger& Finger, const FVector& WristRight, const FVector& WristForward);
	bool CurlCheck(const FFinger& Finger);
	bool AbductionCheck(const FFinger& Finger);
	bool AngleCheck(float Angle, float Min, float Max, float Width, float RangeMin, float RangeMax);
	bool AbducCheck(float Angle, float Mid, float Width);

	// BonesRecognizer
	bool BonesCheck(PxrHandType HandType);
	FVector GetHandJoint(PxrHandType Hand, EPICOXRHandJoint Bone) const;

	// TransRecognizer
	bool TransCheck(ETrackType Type, const FVector& WristPos, const FQuat& WristRot, const FVector& HeadPose, bool bHoldState);
	FVector GetTrackAxis(ETrackType Type, const FQuat& WristRot);
	FVector GetProjectedTarget(const FVector& HeadPose, const FQuat& WristRot, const FVector& WristPos);

	void PrintDebug(const FColor& Color, const FString& Message);

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Init();
};
