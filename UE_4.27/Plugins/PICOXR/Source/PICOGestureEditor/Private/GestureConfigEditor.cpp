// Fill out your copyright notice in the Description page of Project Settings.

#include "GestureConfigEditor.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "GestureConfig.h"
#include "IDetailGroup.h"
#include "Widgets/Input/SSpinBox.h"

#define LOCTEXT_NAMESPACE "PXRHandPoseEditor"

TSharedRef<IDetailCustomization> GestureConfigEditor::MakeInstance()
{
	return MakeShareable(new GestureConfigEditor);
}

void GestureConfigEditor::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	CustomizeShapesRecognizer(DetailBuilder);
	CustomizeBonesRecognizer(DetailBuilder);
	CustomizeTransRecognizer(DetailBuilder);
}

void GestureConfigEditor::CustomizeShapesRecognizer(IDetailLayoutBuilder& DetailBuilder) const
{
	IDetailCategoryBuilder& Category =
		DetailBuilder.EditCategory("Shapes Recognizer", LOCTEXT("ShapesCategory", "Shapes Recognizer"), ECategoryPriority::Important);

	TSharedPtr<IPropertyHandle> ShapesHandle =
		DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UGestureConfig, ShapesRecognizer));

	if (!ShapesHandle.IsValid())
	{
		return;
	}

	// Hide the entire ShapesRecognizer property to prevent default display
	DetailBuilder.HideProperty(ShapesHandle);

	// Thumb
	TSharedPtr<IPropertyHandle> ThumbHandle = ShapesHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FShapesRecognizer, Thumb));
	IDetailGroup& ThumbGroup = Category.AddGroup("Thumb", LOCTEXT("ThumbGroup", "Thumb"), false, true);
	CreateFingerGroup(ThumbGroup, ThumbHandle, EPICOXRHandFinger::None);

	// Index
	TSharedPtr<IPropertyHandle> IndexHandle = ShapesHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FShapesRecognizer, Index));
	IDetailGroup& IndexGroup = Category.AddGroup("Index", LOCTEXT("IndexGroup", "Index"), false, true);
	CreateFingerGroup(IndexGroup, IndexHandle, EPICOXRHandFinger::Index);

	// Middle
	TSharedPtr<IPropertyHandle> MiddleHandle = ShapesHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FShapesRecognizer, Middle));
	IDetailGroup& MiddleGroup = Category.AddGroup("Middle", LOCTEXT("MiddleGroup", "Middle"), false, true);
	CreateFingerGroup(MiddleGroup, MiddleHandle, EPICOXRHandFinger::Middle);

	// Ring
	TSharedPtr<IPropertyHandle> RingHandle = ShapesHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FShapesRecognizer, Ring));
	IDetailGroup& RingGroup = Category.AddGroup("Ring", LOCTEXT("RingGroup", "Ring"), false, true);
	CreateFingerGroup(RingGroup, RingHandle, EPICOXRHandFinger::Ring);

	// Pinky
	TSharedPtr<IPropertyHandle> PinkyHandle = ShapesHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FShapesRecognizer, Pinky));
	IDetailGroup& PinkyGroup = Category.AddGroup("Pinky", LOCTEXT("PinkyGroup", "Pinky"), false, true);
	CreateFingerGroup(PinkyGroup, PinkyHandle, EPICOXRHandFinger::Pinky);

	// Hold Duration
	TSharedPtr<IPropertyHandle> HoldDurationHandle = ShapesHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FShapesRecognizer, HoldDuration));
	Category.AddProperty(HoldDurationHandle);
}

void GestureConfigEditor::CreateFingerGroup(IDetailGroup& Group, const TSharedPtr<IPropertyHandle>& FingerProperty, EPICOXRHandFinger FingerType) const
{
	if (!FingerProperty.IsValid())
	{
		return;
	}

	AddFlexionProperty(Group, FingerProperty, FingerType);
	AddCurlProperty(Group, FingerProperty, FingerType);

	if (FingerType != EPICOXRHandFinger::Pinky)
	{
		AddAbductionProperty(Group, FingerProperty);
	}
}

void GestureConfigEditor::AddFlexionProperty(IDetailGroup& Group, const TSharedPtr<IPropertyHandle>& FingerProperty, EPICOXRHandFinger FingerType) const
{
	TSharedPtr<IPropertyHandle> FlexionHandle = FingerProperty->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FFinger, Flexion));

	// Add custom callback for Flexion enum changes
	FlexionHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateLambda([FingerProperty, FingerType]()
	{
		TSharedPtr<IPropertyHandle> FlexionHandle = FingerProperty->GetChildHandle(
			GET_MEMBER_NAME_CHECKED(FFinger, Flexion));
		TSharedPtr<IPropertyHandle> ConfigsHandle = FingerProperty->GetChildHandle(
			GET_MEMBER_NAME_CHECKED(FFinger, FingerConfigs));

		if (FlexionHandle.IsValid() && ConfigsHandle.IsValid())
		{
			TSharedPtr<IPropertyHandle> FlexionConfigsHandle = ConfigsHandle->GetChildHandle(
				GET_MEMBER_NAME_CHECKED(FFingerConfigs, FlexionConfigs));

			if (FlexionConfigsHandle.IsValid())
			{
				TSharedPtr<IPropertyHandle> MinHandle = FlexionConfigsHandle->GetChildHandle(
					GET_MEMBER_NAME_CHECKED(FRangeConfigs, Min));
				TSharedPtr<IPropertyHandle> MaxHandle = FlexionConfigsHandle->GetChildHandle(
					GET_MEMBER_NAME_CHECKED(FRangeConfigs, Max));

				uint8 FlexionValue = 0;
				FlexionHandle->GetValue(FlexionValue);

				// EFlexion: 0=Any, 1=Open, 2=Close
				if (FlexionValue == 1) // Open
				{
					if (FingerType == EPICOXRHandFinger::None) // Thumb
					{
						MinHandle->SetValue(FlexionThumbOpenMin);
						MaxHandle->SetValue(FlexionThumbOpenMax);
					}
					else
					{
						MinHandle->SetValue(FlexionOpenMin);
						MaxHandle->SetValue(FlexionOpenMax);
					}
					MinHandle->NotifyFinishedChangingProperties();
					MaxHandle->NotifyFinishedChangingProperties();
				}
				else if (FlexionValue == 2) // Close
				{
					if (FingerType == EPICOXRHandFinger::None) // Thumb
					{
						MinHandle->SetValue(FlexionThumbCloseMin);
						MaxHandle->SetValue(FlexionThumbCloseMax);
					}
					else
					{
						MinHandle->SetValue(FlexionCloseMin);
						MaxHandle->SetValue(FlexionCloseMax);
					}
					MinHandle->NotifyFinishedChangingProperties();
					MaxHandle->NotifyFinishedChangingProperties();
				}
			}
		}
	}));

	Group.AddPropertyRow(FlexionHandle.ToSharedRef());

	TSharedPtr<IPropertyHandle> ConfigsHandle = FingerProperty->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FFinger, FingerConfigs));

	if (ConfigsHandle.IsValid())
	{
		TSharedPtr<IPropertyHandle> FlexionConfigsHandle = ConfigsHandle->GetChildHandle(
			GET_MEMBER_NAME_CHECKED(FFingerConfigs, FlexionConfigs));

		if (FlexionConfigsHandle.IsValid())
		{
			TSharedPtr<IPropertyHandle> MinHandle = FlexionConfigsHandle->GetChildHandle(
				GET_MEMBER_NAME_CHECKED(FRangeConfigs, Min));
			TSharedPtr<IPropertyHandle> MaxHandle = FlexionConfigsHandle->GetChildHandle(
				GET_MEMBER_NAME_CHECKED(FRangeConfigs, Max));
			TSharedPtr<IPropertyHandle> WidthHandle = FlexionConfigsHandle->GetChildHandle(
				GET_MEMBER_NAME_CHECKED(FRangeConfigs, Width));

			Group.AddWidgetRow()
			     .NameContent()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("FlexionMargin", "Margin"))
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
				.ValueContent()
				.MinDesiredWidth(250.0f)
				[
					SNew(SSpinBox<float>)
					.MinValue(0.0f)
					.MaxValue(FlexionMax - FlexionMin)
					.MinSliderValue(0.0f)
					.MaxSliderValue(FlexionMax - FlexionMin)
					.MinFractionalDigits(1)
					.MaxFractionalDigits(1)
					.Delta(0.1f)
					.Value_Lambda([WidthHandle]()
					{
						float Value = 0.0f;
						WidthHandle->GetValue(Value);
						return Value;
					})
					.OnValueChanged_Lambda([WidthHandle](float NewValue)
					{
						WidthHandle->SetValue(NewValue, EPropertyValueSetFlags::InteractiveChange);
					})
					.OnValueCommitted_Lambda([WidthHandle](float NewValue, ETextCommit::Type)
					{
						WidthHandle->SetValue(NewValue);
					})
				]
				.Visibility(TAttribute<EVisibility>::Create([FlexionHandle]()
				{
					uint8 FlexionValue = 0;
					FlexionHandle->GetValue(FlexionValue);
					return (FlexionValue == 0) ? EVisibility::Collapsed : EVisibility::Visible; // 0 = Any
				}));

			Group.AddWidgetRow()
			     .NameContent()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("FlexionRange", "Flexion Range"))
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
				.ValueContent()
				.MinDesiredWidth(250.0f)
				[
					SNew(STextBlock)
					.Text_Lambda([MinHandle, MaxHandle, WidthHandle]()
					{
						float Min = 0.0f, Max = 0.0f, Width = 0.0f;
						MinHandle->GetValue(Min);
						MaxHandle->GetValue(Max);
						WidthHandle->GetValue(Width);
						return FText::FromString(FString::Printf(TEXT("[%.1f - %.1f, %.1f + %.1f]"), Min, Width, Max, Width));
					})
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
				.Visibility(TAttribute<EVisibility>::Create([FlexionHandle]()
				{
					uint8 FlexionValue = 0;
					FlexionHandle->GetValue(FlexionValue);
					return (FlexionValue == 0) ? EVisibility::Collapsed : EVisibility::Visible; // 0 = Any
				}));
		}
	}
}

void GestureConfigEditor::AddCurlProperty(IDetailGroup& Group, const TSharedPtr<IPropertyHandle>& FingerProperty, EPICOXRHandFinger FingerType) const
{
	TSharedPtr<IPropertyHandle> CurlHandle = FingerProperty->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FFinger, Curl));

	// Add custom callback for Curl enum changes
	CurlHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateLambda([FingerProperty, FingerType]()
	{
		TSharedPtr<IPropertyHandle> CurlHandle = FingerProperty->GetChildHandle(
			GET_MEMBER_NAME_CHECKED(FFinger, Curl));
		TSharedPtr<IPropertyHandle> ConfigsHandle = FingerProperty->GetChildHandle(
			GET_MEMBER_NAME_CHECKED(FFinger, FingerConfigs));

		if (CurlHandle.IsValid() && ConfigsHandle.IsValid())
		{
			TSharedPtr<IPropertyHandle> CurlConfigsHandle = ConfigsHandle->GetChildHandle(
				GET_MEMBER_NAME_CHECKED(FFingerConfigs, CurlConfigs));

			if (CurlConfigsHandle.IsValid())
			{
				TSharedPtr<IPropertyHandle> MinHandle = CurlConfigsHandle->GetChildHandle(
					GET_MEMBER_NAME_CHECKED(FRangeConfigs, Min));
				TSharedPtr<IPropertyHandle> MaxHandle = CurlConfigsHandle->GetChildHandle(
					GET_MEMBER_NAME_CHECKED(FRangeConfigs, Max));

				uint8 CurlValue = 0;
				CurlHandle->GetValue(CurlValue);

				// ECurl: 0=Any, 1=Open, 2=Close
				if (CurlValue == 1) // Open
				{
					if (FingerType == EPICOXRHandFinger::None) // Thumb
					{
						MinHandle->SetValue(CurlThumbOpenMin);
						MaxHandle->SetValue(CurlThumbOpenMax);
					}
					else
					{
						MinHandle->SetValue(CurlOpenMin);
						MaxHandle->SetValue(CurlOpenMax);
					}
					MinHandle->NotifyFinishedChangingProperties();
					MaxHandle->NotifyFinishedChangingProperties();
				}
				else if (CurlValue == 2) // Close
				{
					if (FingerType == EPICOXRHandFinger::None) // Thumb
					{
						MinHandle->SetValue(CurlThumbCloseMin);
						MaxHandle->SetValue(CurlThumbCloseMax);
					}
					else
					{
						MinHandle->SetValue(CurlCloseMin);
						MaxHandle->SetValue(CurlCloseMax);
					}
					MinHandle->NotifyFinishedChangingProperties();
					MaxHandle->NotifyFinishedChangingProperties();
				}
			}
		}
	}));

	Group.AddPropertyRow(CurlHandle.ToSharedRef());

	TSharedPtr<IPropertyHandle> ConfigsHandle = FingerProperty->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FFinger, FingerConfigs));

	if (ConfigsHandle.IsValid())
	{
		TSharedPtr<IPropertyHandle> CurlConfigsHandle = ConfigsHandle->GetChildHandle(
			GET_MEMBER_NAME_CHECKED(FFingerConfigs, CurlConfigs));

		if (CurlConfigsHandle.IsValid())
		{
			TSharedPtr<IPropertyHandle> MinHandle = CurlConfigsHandle->GetChildHandle(
				GET_MEMBER_NAME_CHECKED(FRangeConfigs, Min));
			TSharedPtr<IPropertyHandle> MaxHandle = CurlConfigsHandle->GetChildHandle(
				GET_MEMBER_NAME_CHECKED(FRangeConfigs, Max));
			TSharedPtr<IPropertyHandle> WidthHandle = CurlConfigsHandle->GetChildHandle(
				GET_MEMBER_NAME_CHECKED(FRangeConfigs, Width));

			Group.AddWidgetRow()
			     .NameContent()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("CurlMargin", "Margin"))
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
				.ValueContent()
				.MinDesiredWidth(250.0f)
				[
					SNew(SSpinBox<float>)
					.MinValue(0.0f)
					.MaxValue(CurlMax - CurlMin)
					.MinSliderValue(0.0f)
					.MaxSliderValue(CurlMax - CurlMin)
					.MinFractionalDigits(0)
					.MaxFractionalDigits(0)
					.Delta(1)
					.Value_Lambda([WidthHandle]()
					{
						float Value = 0.0f;
						WidthHandle->GetValue(Value);
						return Value;
					})
					.OnValueChanged_Lambda([WidthHandle](float NewValue)
					{
						WidthHandle->SetValue(NewValue, EPropertyValueSetFlags::InteractiveChange);
					})
					.OnValueCommitted_Lambda([WidthHandle](float NewValue, ETextCommit::Type)
					{
						WidthHandle->SetValue(NewValue);
					})
				]
				.Visibility(TAttribute<EVisibility>::Create([CurlHandle]()
				{
					uint8 CurlValue = 0;
					CurlHandle->GetValue(CurlValue);
					return (CurlValue == 0) ? EVisibility::Collapsed : EVisibility::Visible; // 0 = Any
				}));

			Group.AddWidgetRow()
			     .NameContent()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("CurlRange", "Curl Range"))
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
				.ValueContent()
				.MinDesiredWidth(250.0f)
				[
					SNew(STextBlock)
					.Text_Lambda([MinHandle, MaxHandle, WidthHandle]()
					{
						float Min = 0.0f, Max = 0.0f, Width = 0.0f;
						MinHandle->GetValue(Min);
						MaxHandle->GetValue(Max);
						WidthHandle->GetValue(Width);
						return FText::FromString(FString::Printf(TEXT("[%.1f - %.1f, %.1f + %.1f]"), Min, Width, Max, Width));
					})
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
				.Visibility(TAttribute<EVisibility>::Create([CurlHandle]()
				{
					uint8 CurlValue = 0;
					CurlHandle->GetValue(CurlValue);
					return (CurlValue == 0) ? EVisibility::Collapsed : EVisibility::Visible; // 0 = Any
				}));
		}
	}
}

void GestureConfigEditor::AddAbductionProperty(IDetailGroup& Group, const TSharedPtr<IPropertyHandle>& FingerProperty) const
{
	TSharedPtr<IPropertyHandle> AbductionHandle = FingerProperty->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FFinger, Abduction));
	Group.AddPropertyRow(AbductionHandle.ToSharedRef());

	TSharedPtr<IPropertyHandle> ConfigsHandle = FingerProperty->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FFinger, FingerConfigs));

	if (ConfigsHandle.IsValid())
	{
		TSharedPtr<IPropertyHandle> AbductionConfigsHandle = ConfigsHandle->GetChildHandle(
			GET_MEMBER_NAME_CHECKED(FFingerConfigs, AbductionConfigs));

		if (AbductionConfigsHandle.IsValid())
		{
			TSharedPtr<IPropertyHandle> MidHandle = AbductionConfigsHandle->GetChildHandle(
				GET_MEMBER_NAME_CHECKED(FRangeConfigsAbduction, Mid));
			TSharedPtr<IPropertyHandle> WidthHandle = AbductionConfigsHandle->GetChildHandle(
				GET_MEMBER_NAME_CHECKED(FRangeConfigsAbduction, Width));

			Group.AddWidgetRow()
			     .NameContent()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("AbductionMargin", "Margin"))
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
				.ValueContent()
				.MinDesiredWidth(250.0f)
				[
					SNew(SSpinBox<float>)
					.MinValue(0.0f)
					.MaxValue(AbductionMax - AbductionMin)
					.MinSliderValue(0.0f)
					.MaxSliderValue(AbductionMax - AbductionMin)
					.MinFractionalDigits(1)
					.MaxFractionalDigits(1)
					.Delta(0.1f)
					.Value_Lambda([WidthHandle]()
					{
						float Value = 0.0f;
						WidthHandle->GetValue(Value);
						return Value;
					})
					.OnValueChanged_Lambda([WidthHandle](float NewValue)
					{
						WidthHandle->SetValue(NewValue, EPropertyValueSetFlags::InteractiveChange);
					})
					.OnValueCommitted_Lambda([WidthHandle](float NewValue, ETextCommit::Type)
					{
						WidthHandle->SetValue(NewValue);
					})
				]
				.Visibility(TAttribute<EVisibility>::Create([AbductionHandle]()
				{
					uint8 AbductionValue = 0;
					AbductionHandle->GetValue(AbductionValue);
					return (AbductionValue == 0) ? EVisibility::Collapsed : EVisibility::Visible; // 0 = Any
				}));

			Group.AddWidgetRow()
			     .NameContent()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("AbductionRange", "Abduction Range"))
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
				.ValueContent()
				.MinDesiredWidth(250.0f)
				[
					SNew(STextBlock)
					.Text_Lambda([MidHandle, WidthHandle]()
					{
						float Mid = 0.0f, Width = 0.0f;
						MidHandle->GetValue(Mid);
						WidthHandle->GetValue(Width);
						return FText::FromString(FString::Printf(TEXT("[%.1f ± %.1f/2]"), Mid, Width));
					})
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
				.Visibility(TAttribute<EVisibility>::Create([AbductionHandle]()
				{
					uint8 AbductionValue = 0;
					AbductionHandle->GetValue(AbductionValue);
					return (AbductionValue == 0) ? EVisibility::Collapsed : EVisibility::Visible; // 0 = Any
				}));
		}
	}
}

void GestureConfigEditor::CustomizeBonesRecognizer(IDetailLayoutBuilder& DetailBuilder) const
{
	IDetailCategoryBuilder& Category =
		DetailBuilder.EditCategory("Bones Recognizer", LOCTEXT("BonesCategory", "Bones Recognizer"), ECategoryPriority::Important);

	TSharedPtr<IPropertyHandle> BonesHandle =
		DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UGestureConfig, BonesRecognizer));

	if (!BonesHandle.IsValid())
	{
		return;
	}

	DetailBuilder.HideProperty(BonesHandle);

	TSharedPtr<IPropertyHandle> BonesArrayHandle = BonesHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FBonesRecognizer, Bones));
	Category.AddProperty(BonesArrayHandle);

	TSharedPtr<IPropertyHandle> HoldDurationHandle = BonesHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FBonesRecognizer, HoldDuration));
	Category.AddProperty(HoldDurationHandle);
}

void GestureConfigEditor::CustomizeTransRecognizer(IDetailLayoutBuilder& DetailBuilder) const
{
	IDetailCategoryBuilder& Category =
		DetailBuilder.EditCategory("Transform Recognizer", LOCTEXT("TransCategory", "Transform Recognizer"), ECategoryPriority::Important);

	TSharedPtr<IPropertyHandle> TransHandle =
		DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UGestureConfig, TransRecognizer));

	if (!TransHandle.IsValid())
	{
		return;
	}

	DetailBuilder.HideProperty(TransHandle);

	Category.AddProperty(TransHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FTransRecognizer, TrackAxis)));
	Category.AddProperty(TransHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FTransRecognizer, TrackTarget)));
	Category.AddProperty(TransHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FTransRecognizer, AngleThreshold)));
	Category.AddProperty(TransHandle->GetChildHandle(
		        GET_MEMBER_NAME_CHECKED(FTransRecognizer, ThresholdWidth)))
	        .DisplayName(LOCTEXT("TransMargin", "Margin"));
	Category.AddProperty(TransHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FTransRecognizer, HoldDuration)));
}

#undef LOCTEXT_NAMESPACE
