// Fill out your copyright notice in the Description page of Project Settings.

#include "PICOGestureEditor.h"

#include "GestureConfig.h"
#include "GestureConfigEditor.h"
#include "AssetTypeActions_GestureConfig.h"
#include "AssetToolsModule.h"

#define LOCTEXT_NAMESPACE "FPICOGestureEditorModule"

void FPICOGestureEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(UGestureConfig::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&GestureConfigEditor::MakeInstance));
	PropertyModule.NotifyCustomizationModuleChanged();

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	GestureConfigAssetTypeActions = MakeShared<FAssetTypeActions_GestureConfig>();
	AssetTools.RegisterAssetTypeActions(GestureConfigAssetTypeActions.ToSharedRef());
}

void FPICOGestureEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		if (GestureConfigAssetTypeActions.IsValid())
		{
			AssetTools.UnregisterAssetTypeActions(GestureConfigAssetTypeActions.ToSharedRef());
		}
	}

	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomClassLayout(UGestureConfig::StaticClass()->GetFName());
		PropertyModule.NotifyCustomizationModuleChanged();
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPICOGestureEditorModule, PICOGesture)
