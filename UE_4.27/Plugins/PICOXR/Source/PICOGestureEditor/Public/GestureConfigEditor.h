// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "IDetailCustomization.h"

enum class EPICOXRHandFinger : uint8;
class IDetailGroup;

class GestureConfigEditor : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	void CustomizeShapesRecognizer(IDetailLayoutBuilder& DetailBuilder) const;
	void CustomizeBonesRecognizer(IDetailLayoutBuilder& DetailBuilder) const;
	void CustomizeTransRecognizer(IDetailLayoutBuilder& DetailBuilder) const;

	void CreateFingerGroup(IDetailGroup& Group, const TSharedPtr<IPropertyHandle>& FingerProperty, EPICOXRHandFinger FingerType) const;
	void AddFlexionProperty(IDetailGroup& Group, const TSharedPtr<IPropertyHandle>& FingerProperty, EPICOXRHandFinger FingerType) const;
	void AddCurlProperty(IDetailGroup& Group, const TSharedPtr<IPropertyHandle>& FingerProperty, EPICOXRHandFinger FingerType) const;
	void AddAbductionProperty(IDetailGroup& Group, const TSharedPtr<IPropertyHandle>& FingerProperty) const;
};
