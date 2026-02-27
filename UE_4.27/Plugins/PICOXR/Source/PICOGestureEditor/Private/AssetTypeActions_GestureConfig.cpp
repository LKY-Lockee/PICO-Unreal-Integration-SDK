// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetTypeActions_GestureConfig.h"
#include "GestureConfigAssetEditor.h"
#include "GestureConfig.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_GestureConfig"

FAssetTypeActions_GestureConfig::FAssetTypeActions_GestureConfig()
{
}

FText FAssetTypeActions_GestureConfig::GetName() const
{
	return LOCTEXT("FAssetTypeActions_GestureConfig", "Gesture Config");
}

FColor FAssetTypeActions_GestureConfig::GetTypeColor() const
{
	return FColor(129, 196, 115);
}

UClass* FAssetTypeActions_GestureConfig::GetSupportedClass() const
{
	return UGestureConfig::StaticClass();
}

void FAssetTypeActions_GestureConfig::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UGestureConfig* GestureConfig = Cast<UGestureConfig>(*ObjIt))
		{
			TSharedRef<FGestureConfigAssetEditor> NewEditor(new FGestureConfigAssetEditor());
			NewEditor->InitGestureConfigAssetEditor(Mode, EditWithinLevelEditor, GestureConfig);
		}
	}
}

uint32 FAssetTypeActions_GestureConfig::GetCategories()
{
	return EAssetTypeCategories::Misc;
}

#undef LOCTEXT_NAMESPACE
