// Fill out your copyright notice in the Description page of Project Settings.

#include "Gesture.h"

#include "PXR_HMDModule.h"
#include "Kismet/GameplayStatics.h"

UGesture::UGesture()
{
	PrimaryComponentTick.bCanEverTick = true;

	LeftJointPos.SetNum(26);
	RightJointPos.SetNum(26);
}

void UGesture::GestureEventCheck(float DeltaTime)
{
	switch (TrackType)
	{
	case ETrackType::Any:
		bPoseStateActive = (bLeftShapesActive && bLeftBonesActive && bLeftTransActive) || (bRightShapesActive && bRightBonesActive && bRightTransActive);
		break;
	case ETrackType::Left:
		bPoseStateActive = bLeftShapesActive && bLeftBonesActive && bLeftTransActive;
		break;
	case ETrackType::Right:
		bPoseStateActive = bRightShapesActive && bRightBonesActive && bRightTransActive;
		break;
	default:
		break;
	}
	if (bPoseStateHold != bPoseStateActive)
	{
		bPoseStateHold = bPoseStateActive;
		if (bPoseStateHold)
		{
			bPoseStateActive = true;
			GestureStartDelegate.Broadcast();
		}
		else
		{
			bPoseStateActive = false;
			GestureEndDelegate.Broadcast();
		}
		PoseStateHoldTime = 0;
	}
	else
	{
		if (bPoseStateHold)
		{
			PoseStateHoldTime += DeltaTime;
			GestureTickDelegate.Broadcast(PoseStateHoldTime);
		}
	}
}

bool UGesture::HoldCheck(bool bHoldState, float HoldDuration, bool bResultState, float& HoldTime) const
{
	if (bResultState != bHoldState)
	{
		HoldTime += UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
		if (HoldTime >= HoldDuration)
		{
			bResultState = bHoldState;
		}
	}
	else
	{
		HoldTime = 0;
	}

	return bResultState;
}

float UGesture::GetAngle(FVector VectorA, FVector VectorB) const
{
	VectorA.Normalize();
	VectorB.Normalize();

	float DotProduct = FVector::DotProduct(VectorA, VectorB);
	DotProduct = FMath::Clamp(DotProduct, -1.0f, 1.0f);

	return FMath::RadiansToDegrees(FMath::Acos(DotProduct));
}

bool UGesture::ShapesRecognizerCheck(TArray<FVector>& JointPos, FVector WristRight, FVector WristForward, int WristDirect)
{
	Thumb0 = JointPos[static_cast<int>(EPICOXRHandJoint::ThumbTip)];
	Thumb1 = JointPos[static_cast<int>(EPICOXRHandJoint::ThumbDistal)];
	Thumb2 = JointPos[static_cast<int>(EPICOXRHandJoint::ThumbProximal)];
	Thumb3 = JointPos[static_cast<int>(EPICOXRHandJoint::ThumbMetacarpal)];

	Index0 = JointPos[static_cast<int>(EPICOXRHandJoint::IndexTip)];
	Index1 = JointPos[static_cast<int>(EPICOXRHandJoint::IndexDistal)];
	Index2 = JointPos[static_cast<int>(EPICOXRHandJoint::IndexIntermediate)];
	Index3 = JointPos[static_cast<int>(EPICOXRHandJoint::IndexProximal)];

	Middle0 = JointPos[static_cast<int>(EPICOXRHandJoint::MiddleTip)];
	Middle1 = JointPos[static_cast<int>(EPICOXRHandJoint::MiddleDistal)];
	Middle2 = JointPos[static_cast<int>(EPICOXRHandJoint::MiddleIntermediate)];
	Middle3 = JointPos[static_cast<int>(EPICOXRHandJoint::MiddleProximal)];

	Ring0 = JointPos[static_cast<int>(EPICOXRHandJoint::RingTip)];
	Ring1 = JointPos[static_cast<int>(EPICOXRHandJoint::RingDistal)];
	Ring2 = JointPos[static_cast<int>(EPICOXRHandJoint::RingIntermediate)];
	Ring3 = JointPos[static_cast<int>(EPICOXRHandJoint::RingProximal)];

	Pinky0 = JointPos[static_cast<int>(EPICOXRHandJoint::LittleTip)];
	Pinky1 = JointPos[static_cast<int>(EPICOXRHandJoint::LittleDistal)];
	Pinky2 = JointPos[static_cast<int>(EPICOXRHandJoint::LittleIntermediate)];
	Pinky3 = JointPos[static_cast<int>(EPICOXRHandJoint::LittleProximal)];

	FShapesRecognizer& ShapesRecognizer = Config->ShapesRecognizer;

	PrintDebug(FColor::Cyan, TEXT("Flexion"));
	bThumbFlex = FlexionCheck(ShapesRecognizer.Thumb, WristRight * WristDirect, WristForward);
	bIndexFlex = FlexionCheck(ShapesRecognizer.Index, WristRight, WristForward);
	bMiddleFlex = FlexionCheck(ShapesRecognizer.Middle, WristRight, WristForward);
	bRingFlex = FlexionCheck(ShapesRecognizer.Ring, WristRight, WristForward);
	bPinkyFlex = FlexionCheck(ShapesRecognizer.Pinky, WristRight, WristForward);

	PrintDebug(FColor::Cyan, TEXT("Curl"));
	bThumbCurl = CurlCheck(ShapesRecognizer.Thumb);
	bIndexCurl = CurlCheck(ShapesRecognizer.Index);
	bMiddleCurl = CurlCheck(ShapesRecognizer.Middle);
	bRingCurl = CurlCheck(ShapesRecognizer.Ring);
	bPinkyCurl = CurlCheck(ShapesRecognizer.Pinky);

	PrintDebug(FColor::Cyan, TEXT("Abduction"));
	bThumbAbduc = AbductionCheck(ShapesRecognizer.Thumb);
	bIndexAbduc = AbductionCheck(ShapesRecognizer.Index);
	bMiddleAbduc = AbductionCheck(ShapesRecognizer.Middle);
	bRingAbduc = AbductionCheck(ShapesRecognizer.Ring);
	bPinkyAbduc = AbductionCheck(ShapesRecognizer.Pinky);

	return bThumbFlex && bIndexFlex && bMiddleFlex && bRingFlex && bPinkyFlex &&
		bThumbCurl && bIndexCurl && bMiddleCurl && bRingCurl && bPinkyCurl &&
		bThumbAbduc && bIndexAbduc && bMiddleAbduc && bRingAbduc && bPinkyAbduc;
}

bool UGesture::FlexionCheck(const FFinger& Finger, const FVector& WristRight, const FVector& WristForward)
{
	if (Finger.Flexion == EFlexion::Any) { return true; }

	float flexAngle = 0;
	switch (Finger.HandFinger)
	{
	case EPICOXRHandFinger::None:
		{
			FVector thumb23 = (Thumb2 - Thumb3);
			FVector thumb23_project = FVector::VectorPlaneProject(thumb23, WristRight);
			flexAngle = GetAngle(thumb23_project, WristForward);
			break;
		}
	case EPICOXRHandFinger::Index:
		{
			FVector index23 = (Index2 - Index3);
			FVector index_project = FVector::VectorPlaneProject(index23, WristForward);
			flexAngle = GetAngle(index_project, WristRight);
			break;
		}
	case EPICOXRHandFinger::Middle:
		{
			FVector middle23 = (Middle2 - Middle3);
			FVector middle_project = FVector::VectorPlaneProject(middle23, WristForward);
			flexAngle = GetAngle(middle_project, WristRight);
			break;
		}
	case EPICOXRHandFinger::Ring:
		{
			FVector ring23 = (Ring2 - Ring3);
			FVector ring_project = FVector::VectorPlaneProject(ring23, WristForward);
			flexAngle = GetAngle(ring_project, WristRight);
			break;
		}
	case EPICOXRHandFinger::Pinky:
		{
			FVector pinky23 = (Pinky2 - Pinky3);
			FVector pinky_project = FVector::VectorPlaneProject(pinky23, WristForward);
			flexAngle = GetAngle(pinky_project, WristRight);
			break;
		}
	default:
		break;
	}

	bool bResult = AngleCheck(flexAngle, Finger.FingerConfigs.FlexionConfigs.Min, Finger.FingerConfigs.FlexionConfigs.Max, Finger.FingerConfigs.FlexionConfigs.Width, FlexionMin, FlexionMax);

	switch (Finger.HandFinger)
	{
	case EPICOXRHandFinger::None:
		{
			PrintDebug(bResult ? FColor::Green : FColor::Red, FString::Printf(TEXT("Thumb: %f"), flexAngle));
			break;
		}
	case EPICOXRHandFinger::Index:
		{
			PrintDebug(bResult ? FColor::Green : FColor::Red, FString::Printf(TEXT("Index: %f"), flexAngle));
			break;
		}
	case EPICOXRHandFinger::Middle:
		{
			PrintDebug(bResult ? FColor::Green : FColor::Red, FString::Printf(TEXT("Middle: %f"), flexAngle));
			break;
		}
	case EPICOXRHandFinger::Ring:
		{
			PrintDebug(bResult ? FColor::Green : FColor::Red, FString::Printf(TEXT("Ring: %f"), flexAngle));
			break;
		}
	case EPICOXRHandFinger::Pinky:
		{
			PrintDebug(bResult ? FColor::Green : FColor::Red, FString::Printf(TEXT("Pinky: %f"), flexAngle));
			break;
		}
	}

	return bResult;
}

bool UGesture::CurlCheck(const FFinger& Finger)
{
	if (Finger.Curl == ECurl::Any) { return true; }

	float curlAngle = 0;
	switch (Finger.HandFinger)
	{
	case EPICOXRHandFinger::None:
		{
			FVector thumb01 = (Thumb0 - Thumb1);
			FVector thumb32 = (Thumb3 - Thumb2);
			curlAngle = GetAngle(thumb01, thumb32);
			break;
		}
	case EPICOXRHandFinger::Index:
		{
			FVector index01 = (Index0 - Index1);
			FVector index32 = (Index3 - Index2);
			curlAngle = GetAngle(index32, index01);
			break;
		}
	case EPICOXRHandFinger::Middle:
		{
			FVector middle01 = (Middle0 - Middle1);
			FVector middle32 = (Middle3 - Middle2);
			curlAngle = GetAngle(middle32, middle01);
			break;
		}
	case EPICOXRHandFinger::Ring:
		{
			FVector ring01 = (Ring0 - Ring1);
			FVector ring32 = (Ring3 - Ring2);
			curlAngle = GetAngle(ring32, ring01);
			break;
		}
	case EPICOXRHandFinger::Pinky:
		{
			FVector pinky01 = (Pinky0 - Pinky1);
			FVector pinky32 = (Pinky3 - Pinky2);
			curlAngle = GetAngle(pinky32, pinky01);
			break;
		}
	default:
		break;
	}

	bool bResult = AngleCheck(curlAngle, Finger.FingerConfigs.CurlConfigs.Min, Finger.FingerConfigs.CurlConfigs.Max, Finger.FingerConfigs.CurlConfigs.Width, CurlMin, CurlMax);

	switch (Finger.HandFinger)
	{
	case EPICOXRHandFinger::None:
		{
			PrintDebug(bResult ? FColor::Green : FColor::Red, FString::Printf(TEXT("Thumb: %f"), curlAngle));
			break;
		}
	case EPICOXRHandFinger::Index:
		{
			PrintDebug(bResult ? FColor::Green : FColor::Red, FString::Printf(TEXT("Index: %f"), curlAngle));
			break;
		}
	case EPICOXRHandFinger::Middle:
		{
			PrintDebug(bResult ? FColor::Green : FColor::Red, FString::Printf(TEXT("Middle: %f"), curlAngle));
			break;
		}
	case EPICOXRHandFinger::Ring:
		{
			PrintDebug(bResult ? FColor::Green : FColor::Red, FString::Printf(TEXT("Ring: %f"), curlAngle));
			break;
		}
	case EPICOXRHandFinger::Pinky:
		{
			PrintDebug(bResult ? FColor::Green : FColor::Red, FString::Printf(TEXT("Pinky: %f"), curlAngle));
			break;
		}
	}

	return bResult;
}

bool UGesture::AbductionCheck(const FFinger& Finger)
{
	if (Finger.Abduction == EAbduction::Any) { return true; }

	float AbducAngle = 0;
	FVector thumb12 = (Thumb1 - Thumb2);
	FVector index23 = (Index2 - Index3);
	FVector middle23 = (Middle2 - Middle3);
	FVector ring23 = (Ring2 - Ring3);
	FVector pinky23 = (Pinky2 - Pinky3);

	switch (Finger.HandFinger)
	{
	case EPICOXRHandFinger::None:
		AbducAngle = GetAngle(thumb12, index23);
		break;
	case EPICOXRHandFinger::Index:
		AbducAngle = GetAngle(index23, middle23);
		break;
	case EPICOXRHandFinger::Middle:
		AbducAngle = GetAngle(middle23, ring23);
		break;
	case EPICOXRHandFinger::Ring:
		AbducAngle = GetAngle(ring23, pinky23);
		break;
	case EPICOXRHandFinger::Pinky:
		AbducAngle = GetAngle(pinky23, ring23);
		break;
	default:
		break;
	}

	bool bResult = false;
	if (Finger.Abduction == EAbduction::Open)
	{
		bResult = AbducCheck(AbducAngle, Finger.FingerConfigs.AbductionConfigs.Mid, Finger.FingerConfigs.AbductionConfigs.Width);
	}
	else if (Finger.Abduction == EAbduction::Close)
	{
		bResult = !AbducCheck(AbducAngle, Finger.FingerConfigs.AbductionConfigs.Mid, Finger.FingerConfigs.AbductionConfigs.Width);
	}

	switch (Finger.HandFinger)
	{
	case EPICOXRHandFinger::None:
		PrintDebug(bResult ? FColor::Green : FColor::Red, FString::Printf(TEXT("Thumb: %f"), AbducAngle));
		break;
	case EPICOXRHandFinger::Index:
		PrintDebug(bResult ? FColor::Green : FColor::Red, FString::Printf(TEXT("Index: %f"), AbducAngle));
		break;
	case EPICOXRHandFinger::Middle:
		PrintDebug(bResult ? FColor::Green : FColor::Red, FString::Printf(TEXT("Middle: %f"), AbducAngle));
		break;
	case EPICOXRHandFinger::Ring:
		PrintDebug(bResult ? FColor::Green : FColor::Red, FString::Printf(TEXT("Ring: %f"), AbducAngle));
		break;
	case EPICOXRHandFinger::Pinky:
		PrintDebug(bResult ? FColor::Green : FColor::Red, FString::Printf(TEXT("Pinky: %f"), AbducAngle));
		break;
	default:
		break;
	}

	return bResult;
}

bool UGesture::AngleCheck(float Angle, float Min, float Max, float Width, float RangeMin, float RangeMax)
{
	if (Angle > Min && Angle < Max)
	{
		bAngleCheckValid = true;
	}
	if (Min - RangeMin <= 1)
	{
		bAngleCheckValid = Angle < Max;
	}
	else if (Angle < (Min - Width))
	{
		bAngleCheckValid = false;
	}

	if (RangeMax - Max <= 1)
	{
		bAngleCheckValid = Angle > Min;
	}
	else if ((Angle > (Max + Width)))
	{
		bAngleCheckValid = false;
	}

	return bAngleCheckValid;
}

bool UGesture::AbducCheck(float Angle, float Mid, float Width)
{
	if (Angle > Mid + Width / 2)
	{
		bAbducCheckOpen = true;
	}
	if (Angle < Mid - Width / 2)
	{
		bAbducCheckOpen = false;
	}

	return bAbducCheckOpen;
}

bool UGesture::BonesCheck(PxrHandType HandType)
{
	for (int i = 0; i < Bones.Num(); i++)
	{
		float distance = FVector::Distance(GetHandJoint(HandType, Bones[i].Bone1), GetHandJoint(HandType, Bones[i].Bone2));
		if (distance < Bones[i].Distance - Bones[i].ThresholdWidth / 2)
		{
			Bones[i].ActiveState = true;
		}
		else if (distance > Bones[i].Distance + Bones[i].ThresholdWidth / 2)
		{
			Bones[i].ActiveState = false;
		}

		if (!Bones[i].ActiveState)
		{
			return false;
		}
	}
	return true;
}

FVector UGesture::GetHandJoint(PxrHandType Hand, EPICOXRHandJoint Bone) const
{
	PxrVector3f position;
	if (Hand == PxrHandLeft)
	{
		position = LeftHandJointLocations.jointLocations[static_cast<int>(Bone)].pose.position;
	}
	else
	{
		position = RightHandJointLocations.jointLocations[static_cast<int>(Bone)].pose.position;
	}

	return FVector(-position.z, position.x, position.y) * 100.0f;
}

bool UGesture::TransCheck(ETrackType Type, const FVector& WristPos, const FQuat& WristRot, const FVector& HeadPose, bool bHoldState)
{
	GetTrackAxis(Type, WristRot);
	GetProjectedTarget(HeadPose, WristRot, WristPos);

	float errorAngle = GetAngle(PalmAxis, TargetPos);

	if (errorAngle < TransAngleThreshold - TransThresholdWidth / 2)
	{
		PrintDebug(FColor::Green, FString::Printf(TEXT("Trans: %f"), errorAngle));
		bHoldState = true;
	}

	if (errorAngle > TransAngleThreshold + TransThresholdWidth / 2)
	{
		PrintDebug(FColor::Red, FString::Printf(TEXT("Trans: %f"), errorAngle));
		bHoldState = false;
	}

	return bHoldState;
}

FVector UGesture::GetTrackAxis(ETrackType Type, const FQuat& WristRot)
{
	switch (TransTrackAxis)
	{
	case ETrackAxis::Fingers:
		PalmAxis = WristRot * FVector::ForwardVector;
		break;
	case ETrackAxis::Palm:
		PalmAxis = WristRot * FVector::DownVector;
		break;
	case ETrackAxis::Thumb:
		PalmAxis = Type == ETrackType::Right ? WristRot * FVector::LeftVector : WristRot * FVector::RightVector;
		break;
	}

	return PalmAxis;
}

FVector UGesture::GetProjectedTarget(const FVector& HeadPose, const FQuat& WristRot, const FVector& WristPos)
{
	PalmPos = WristRot * (TrackType == ETrackType::Right ? FVector(0.0f, 8.0f, 0.0f) : FVector(0.0f, -8.0f, 0.0f)) + WristPos;

	switch (TransTrackTarget)
	{
	case ETrackTarget::TowardsFace:
		TargetPos = HeadPose;
		break;
	case ETrackTarget::AwayFromFace:
		TargetPos = PalmPos * 2 - HeadPose;
		break;
	case ETrackTarget::WorldUp:
		TargetPos = PalmPos + FVector::UpVector;
		break;
	case ETrackTarget::WorldDown:
		TargetPos = PalmPos + FVector::DownVector;
		break;
	}

	TargetPos -= PalmPos;

	switch (TransSpaceType)
	{
	case ESpaceType::WorldSpace:
		break;
	case ESpaceType::LocalXY:
		TargetPos = FVector::VectorPlaneProject(TargetPos, WristRot * FVector::ForwardVector);
		break;
	case ESpaceType::LocalXZ:
		TargetPos = FVector::VectorPlaneProject(TargetPos, WristRot * FVector::UpVector);
		break;
	case ESpaceType::LocalYZ:
		TargetPos = FVector::VectorPlaneProject(TargetPos, WristRot * FVector::RightVector);
		break;
	}

	return TargetPos;
}

void UGesture::PrintDebug(const FColor& Color, const FString& Message)
{
	if (bIsDebug)
	{
		GEngine->AddOnScreenDebugMessage(DebugKey++, 1, Color, Message);
	}
}

void UGesture::BeginPlay()
{
	Super::BeginPlay();

	Init();
}

void UGesture::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsEnabled)
	{
		return;
	}

	if (!Config)
	{
		return;
	}

	HMDPose = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
	PrintDebug(FColor::Cyan, HMDPose.ToString());

	PrintDebug(FColor::Cyan, TEXT("Right"));
	if (TrackType == ETrackType::Right || TrackType == ETrackType::Any)
	{
		FPICOXRHMDModule::GetPluginWrapper().GetHandTrackerJointLocations(PxrHandRight, &RightHandJointLocations);

		for (int i = 0; i < RightJointPos.Num(); ++i)
		{
#if WITH_EDITOR
			auto Location = UPICOXRInputFunctionLibrary::GetBoneLocation(EPICOXRHandType::HandRight, static_cast<EPICOXRHandJoint>(i));
			auto Rotation = UPICOXRInputFunctionLibrary::GetBoneRotation(EPICOXRHandType::HandRight, static_cast<EPICOXRHandJoint>(i));
			RightJointPos[i] = Location;

			if (i == static_cast<int>(EPICOXRHandJoint::Wrist))
			{
				RightWristPos = FVector(-Location.Z, Location.X, Location.Y) * 100.0f;
				RightWristRot = FQuat(-Rotation.Z, Rotation.X, Rotation.Y, -Rotation.W);
			}
#else
			if (RightHandJointLocations.jointLocations == nullptr)
			{
				break;
			}

			auto& pose = RightHandJointLocations.jointLocations[i].pose;
			RightJointPos[i] = FVector(-pose.position.z, pose.position.x, pose.position.y) * 100.0f;

			if (i == static_cast<int>(EPICOXRHandJoint::Wrist))
			{
				RightWristPos = FVector(-pose.position.z, pose.position.x, pose.position.y) * 100.0f;
				RightWristRot = FQuat(-pose.orientation.z, pose.orientation.x, pose.orientation.y, -pose.orientation.w);
			}
#endif
		}
		bRightShapesHold = ShapesRecognizerCheck(RightJointPos, RightWristRot * FVector::LeftVector, RightWristRot * FVector::BackwardVector);
		bRightShapesActive = HoldCheck(bRightShapesHold, ShapesHoldDuration, bRightShapesActive, RightShapesHoldTime);

		bRightBonesHold = BonesCheck(PxrHandRight);
		bRightBonesActive = HoldCheck(bRightBonesHold, BonesHoldDuration, bRightBonesActive, RightBonesHoldTime);

		bRightTransHold = TransCheck(ETrackType::Right, RightWristPos, RightWristRot, HMDPose, bRightTransHold);
		bRightTransActive = HoldCheck(bRightTransHold, TransHoldDuration, bRightTransActive, RightTransHoldTime);
	}

	PrintDebug(FColor::Cyan, TEXT("Left"));
	if (TrackType == ETrackType::Left || TrackType == ETrackType::Any)
	{
		FPICOXRHMDModule::GetPluginWrapper().GetHandTrackerJointLocations(PxrHandLeft, &LeftHandJointLocations);

		for (int i = 0; i < LeftJointPos.Num(); ++i)
		{
#if WITH_EDITOR
			auto Location = UPICOXRInputFunctionLibrary::GetBoneLocation(EPICOXRHandType::HandLeft, static_cast<EPICOXRHandJoint>(i));
			auto Rotation = UPICOXRInputFunctionLibrary::GetBoneRotation(EPICOXRHandType::HandLeft, static_cast<EPICOXRHandJoint>(i));
			LeftJointPos[i] = Location;

			if (i == static_cast<int>(EPICOXRHandJoint::Wrist))
			{
				LeftWristPos = FVector(-Location.Z, Location.X, Location.Y) * 100.0f;
				LeftWristRot = FQuat(-Rotation.Z, Rotation.X, Rotation.Y, -Rotation.W);
			}
#else
			if (LeftHandJointLocations.jointLocations == nullptr)
			{
				break;
			}

			auto& pose = LeftHandJointLocations.jointLocations[i].pose;
			LeftJointPos[i] = FVector(-pose.position.z, pose.position.x, pose.position.y) * 100.0f;

			if (i == static_cast<int>(EPICOXRHandJoint::Wrist))
			{
				LeftWristPos = FVector(-pose.position.z, pose.position.x, pose.position.y) * 100.0f;
				LeftWristRot = FQuat(-pose.orientation.z, pose.orientation.x, pose.orientation.y, -pose.orientation.w);
			}
#endif
		}
		bLeftShapesHold = ShapesRecognizerCheck(LeftJointPos, LeftWristRot * FVector::RightVector, LeftWristRot * FVector::ForwardVector, -1);
		bLeftShapesActive = HoldCheck(bLeftShapesHold, ShapesHoldDuration, bLeftShapesActive, LeftShapesHoldTime);

		bLeftBonesHold = BonesCheck(PxrHandLeft);
		bLeftBonesActive = HoldCheck(bLeftBonesHold, BonesHoldDuration, bLeftBonesActive, LeftBonesHoldTime);

		bLeftTransHold = TransCheck(ETrackType::Left, LeftWristPos, LeftWristRot, HMDPose, bLeftTransHold);
		bLeftTransActive = HoldCheck(bLeftTransHold, TransHoldDuration, bLeftTransActive, LeftTransHoldTime);
	}

	GestureEventCheck(DeltaTime);

	DebugKey = 1;
}

void UGesture::Init()
{
	if (!Config)
	{
		return;
	}

	ShapesHoldDuration = Config->ShapesRecognizer.HoldDuration;

	Bones = Config->BonesRecognizer.Bones;
	for (auto& Bone : Bones)
	{
		Bone.Distance *= 100.0f;
		Bone.ThresholdWidth *= 100.0f;
	}
	BonesHoldDuration = Config->BonesRecognizer.HoldDuration;

	TransTrackAxis = Config->TransRecognizer.TrackAxis;
	TransSpaceType = Config->TransRecognizer.SpaceType;
	TransTrackTarget = Config->TransRecognizer.TrackTarget;
	TransHoldDuration = Config->TransRecognizer.HoldDuration;
	TransAngleThreshold = Config->TransRecognizer.AngleThreshold;
	TransThresholdWidth = Config->TransRecognizer.ThresholdWidth;
}
