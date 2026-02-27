// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PXR_InputFunctionLibrary.h"
#include "ShapesRecognizer.generated.h"

constexpr float DefaultFlexionWidth = 10;

constexpr float FlexionThumbOpenMin = 155;
constexpr float FlexionThumbOpenMax = 180;
constexpr float FlexionThumbCloseMin = 90;
constexpr float FlexionThumbCloseMax = 120;

constexpr float FlexionOpenMin = 144;
constexpr float FlexionOpenMax = 180;
constexpr float FlexionCloseMin = 90;
constexpr float FlexionCloseMax = 126;
constexpr float FlexionMin = 90;
constexpr float FlexionMax = 180;

constexpr float DefaultCurlWidth = 20;

constexpr float CurlThumbOpenMin = 90;
constexpr float CurlThumbOpenMax = 180;
constexpr float CurlThumbCloseMin = 45;
constexpr float CurlThumbCloseMax = 90;
constexpr float CurlThumbMin = 45;
constexpr float CurlThumbMax = 180;

constexpr float CurlOpenMin = 107;
constexpr float CurlOpenMax = 180;
constexpr float CurlCloseMin = 0;
constexpr float CurlCloseMax = 73;
constexpr float CurlMin = 0;
constexpr float CurlMax = 180;

constexpr float AbductionThumbMid = 13;
constexpr float AbductionThumbWidth = 6;

constexpr float AbductionMid = 10;
constexpr float AbductionWidth = 6;
constexpr float AbductionMin = 0;
constexpr float AbductionMax = 90;

UENUM()
enum class EShapeType
{
	Flexion,
	Curl,
	Abduction
};

UENUM()
enum class EFlexion
{
	Any,
	Open,
	Close,
};

UENUM()
enum class ECurl
{
	Any,
	Open,
	Close,
};

UENUM()
enum class EAbduction
{
	Any,
	Open,
	Close,
};

USTRUCT(BlueprintType)
struct FRangeConfigs
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Min;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Max;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Width;

	FRangeConfigs() = default;

	FRangeConfigs(float Min, float Max, float Width) : Min(Min), Max(Max), Width(Width)
	{
	}
};

USTRUCT(BlueprintType)
struct FRangeConfigsAbduction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Mid;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Width;

	FRangeConfigsAbduction() = default;

	FRangeConfigsAbduction(float Mid, float Width) : Mid(Mid), Width(Width)
	{
	}
};

USTRUCT(BlueprintType)
struct FFingerConfigs
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FRangeConfigs FlexionConfigs;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FRangeConfigs CurlConfigs;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FRangeConfigsAbduction AbductionConfigs;

	FFingerConfigs() = default;

	FFingerConfigs(const EPICOXRHandFinger& Finger)
	{
		FlexionConfigs = FRangeConfigs(FlexionMin, FlexionMax, DefaultFlexionWidth);
		if (Finger == EPICOXRHandFinger::None)
		{
			CurlConfigs = FRangeConfigs(CurlThumbMin, CurlThumbMax, DefaultCurlWidth);
			AbductionConfigs = FRangeConfigsAbduction(AbductionThumbMid, AbductionThumbWidth);
		}
		else
		{
			CurlConfigs = FRangeConfigs(CurlMin, CurlMax, DefaultCurlWidth);
			AbductionConfigs = FRangeConfigsAbduction(AbductionMid, AbductionWidth);
		}
	}
};

USTRUCT(BlueprintType)
struct FFinger
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EPICOXRHandFinger HandFinger;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EFlexion Flexion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ECurl Curl;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EAbduction Abduction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FFingerConfigs FingerConfigs;

	FFinger() = default;

	FFinger(const EPICOXRHandFinger& Finger) : HandFinger(Finger), Flexion(EFlexion::Any), Curl(ECurl::Any), Abduction(EAbduction::Any), FingerConfigs(FFingerConfigs(Finger))
	{
	}
};

USTRUCT(BlueprintType)
struct FShapesRecognizer
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FFinger Thumb;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FFinger Index;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FFinger Middle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FFinger Ring;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FFinger Pinky;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HoldDuration;

	FShapesRecognizer()
		: Thumb(FFinger(EPICOXRHandFinger::None)),
		  Index(FFinger(EPICOXRHandFinger::Index)),
		  Middle(FFinger(EPICOXRHandFinger::Middle)),
		  Ring(FFinger(EPICOXRHandFinger::Ring)),
		  Pinky(FFinger(EPICOXRHandFinger::Pinky)),
		  HoldDuration(0.09f)
	{
	}
};
