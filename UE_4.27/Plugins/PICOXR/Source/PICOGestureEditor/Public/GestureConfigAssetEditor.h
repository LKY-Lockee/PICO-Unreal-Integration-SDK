// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "EditorUndoClient.h"
#include "SEditorViewport.h"

class UPoseableMeshComponent;
class UGestureConfig;
class IDetailsView;
class FAdvancedPreviewScene;

class FGestureConfigViewportClient : public FEditorViewportClient
{
public:
	FGestureConfigViewportClient(FAdvancedPreviewScene& InPreviewScene, const TSharedRef<SEditorViewport>& InEditorViewportWidget);

	virtual void Tick(float DeltaSeconds) override;
};

class SGestureConfigViewport : public SEditorViewport
{
public:
	SLATE_BEGIN_ARGS(SGestureConfigViewport);

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedPtr<FAdvancedPreviewScene>& InPreviewScene);

protected:
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;

private:
	TSharedPtr<FAdvancedPreviewScene> PreviewScene;
	TSharedPtr<FGestureConfigViewportClient> ViewportClient;
};

class FGestureConfigAssetEditor : public FAssetEditorToolkit, public FEditorUndoClient, public FGCObject
{
	UGestureConfig* GestureConfig;
	TSharedPtr<IDetailsView> DetailsView;
	TSharedPtr<SEditorViewport> ViewportWidget;
	TSharedPtr<FAdvancedPreviewScene> PreviewScene;
	UPoseableMeshComponent* HandMeshComponent;

	static const FName ViewportTabId;
	static const FName DetailsTabId;

public:
	FGestureConfigAssetEditor();
	virtual ~FGestureConfigAssetEditor() override;

	void InitGestureConfigAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UGestureConfig* InGestureConfig);

	// IToolkit interface
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	// End of IToolkit interface

	// FAssetEditorToolkit interface
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	// End of FAssetEditorToolkit interface

	// FGCObject interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;
	// End of FGCObject interface

	// FEditorUndoClient interface
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;
	// End of FEditorUndoClient interface

private:
	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args) const;
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args) const;

	void CreatePreviewScene();
	void LoadHandMesh();
	void UpdateHandPose() const;
	void OnPropertyChanged(const FPropertyChangedEvent& PropertyChangedEvent) const;
};
