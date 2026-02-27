// Fill out your copyright notice in the Description page of Project Settings.

#include "GestureConfigAssetEditor.h"
#include "GestureConfig.h"
#include "ShapesRecognizer.h"
#include "Widgets/Docking/SDockTab.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "Editor.h"
#include "EditorViewportClient.h"
#include "PreviewScene.h"
#include "AdvancedPreviewScene.h"
#include "EditorModeManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Engine/SkeletalMesh.h"

#define LOCTEXT_NAMESPACE "GestureConfigAssetEditor"

FGestureConfigViewportClient::FGestureConfigViewportClient(FAdvancedPreviewScene& InPreviewScene, const TSharedRef<SEditorViewport>& InEditorViewportWidget) : FEditorViewportClient(nullptr, &InPreviewScene, StaticCastSharedRef<SEditorViewport>(InEditorViewportWidget))
{
	SetViewLocation(FVector(-150.0f, 0.0f, 100.0f));
	SetViewRotation(FRotator(-10.0f, 0.0f, 0.0f));

	DrawHelper.bDrawGrid = false;
	DrawHelper.bDrawWorldBox = false;
	DrawHelper.bDrawPivot = false;

	EngineShowFlags.SetSeparateTranslucency(true);
	EngineShowFlags.SetSnap(false);
	EngineShowFlags.SetCompositeEditorPrimitives(true);

	OverrideNearClipPlane(1.0f);
	bUsingOrbitCamera = true;
}

void FGestureConfigViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);

	if (PreviewScene)
	{
		PreviewScene->GetWorld()->Tick(LEVELTICK_All, DeltaSeconds);
	}
}

SGestureConfigViewport::FArguments::FArguments()
{
}

void SGestureConfigViewport::Construct(const FArguments&, const TSharedPtr<FAdvancedPreviewScene>& InPreviewScene)
{
	PreviewScene = InPreviewScene;
	SEditorViewport::Construct(SEditorViewport::FArguments());
}

TSharedRef<FEditorViewportClient> SGestureConfigViewport::MakeEditorViewportClient()
{
	ViewportClient = MakeShareable(new FGestureConfigViewportClient(*PreviewScene.Get(), SharedThis(this)));
	return ViewportClient.ToSharedRef();
}

const FName FGestureConfigAssetEditor::ViewportTabId(TEXT("GestureConfigEditor_Viewport"));
const FName FGestureConfigAssetEditor::DetailsTabId(TEXT("GestureConfigEditor_Details"));

FGestureConfigAssetEditor::FGestureConfigAssetEditor() : GestureConfig(nullptr), HandMeshComponent(nullptr)
{
}

FGestureConfigAssetEditor::~FGestureConfigAssetEditor()
{
	if (HandMeshComponent)
	{
		HandMeshComponent->DestroyComponent();
		HandMeshComponent = nullptr;
	}

	DetailsView.Reset();
	ViewportWidget.Reset();
	PreviewScene.Reset();
}

void FGestureConfigAssetEditor::InitGestureConfigAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UGestureConfig* InGestureConfig)
{
	GestureConfig = InGestureConfig;

	CreatePreviewScene();
	LoadHandMesh();

	// Create details view
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bHideSelectionTip = true;

	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObject(GestureConfig);

	// Listen for property changes
	DetailsView->OnFinishedChangingProperties().AddSP(this, &FGestureConfigAssetEditor::OnPropertyChanged);

	// Create viewport widget
	ViewportWidget = SNew(SGestureConfigViewport, PreviewScene);
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_GestureConfigEditor_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(1.0f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.6f)
					->AddTab(ViewportTabId, ETabState::OpenedTab)
					->SetHideTabWell(true)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.4f)
					->AddTab(DetailsTabId, ETabState::OpenedTab)
					->SetHideTabWell(true)
				)
			)
		);

	constexpr bool bCreateDefaultStandaloneMenu = true;
	constexpr bool bCreateDefaultToolbar = true;

	FAssetEditorToolkit::InitAssetEditor(
		Mode,
		InitToolkitHost,
		TEXT("GestureConfigEditorApp"),
		StandaloneDefaultLayout,
		bCreateDefaultStandaloneMenu,
		bCreateDefaultToolbar,
		GestureConfig
	);
}

void FGestureConfigAssetEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_GestureConfigEditor", "Gesture Config Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(ViewportTabId, FOnSpawnTab::CreateSP(this, &FGestureConfigAssetEditor::SpawnTab_Viewport))
	            .SetDisplayName(LOCTEXT("ViewportTab", "Viewport"))
	            .SetGroup(WorkspaceMenuCategoryRef)
	            .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));

	InTabManager->RegisterTabSpawner(DetailsTabId, FOnSpawnTab::CreateSP(this, &FGestureConfigAssetEditor::SpawnTab_Details))
	            .SetDisplayName(LOCTEXT("DetailsTab", "Details"))
	            .SetGroup(WorkspaceMenuCategoryRef)
	            .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FGestureConfigAssetEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(ViewportTabId);
	InTabManager->UnregisterTabSpawner(DetailsTabId);
}

FName FGestureConfigAssetEditor::GetToolkitFName() const
{
	return FName("GestureConfigEditor");
}

FText FGestureConfigAssetEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Gesture Config Editor");
}

FText FGestureConfigAssetEditor::GetToolkitName() const
{
	const bool bDirtyState = GestureConfig->GetOutermost()->IsDirty();

	FFormatNamedArguments Args;
	Args.Add(TEXT("GestureConfigName"), FText::FromString(GestureConfig->GetName()));
	Args.Add(TEXT("DirtyState"), bDirtyState ? FText::FromString(TEXT("*")) : FText::GetEmpty());
	return FText::Format(LOCTEXT("GestureConfigEditorToolkitName", "{GestureConfigName}{DirtyState}"), Args);
}

FText FGestureConfigAssetEditor::GetToolkitToolTipText() const
{
	return GetToolTipTextForObject(GestureConfig);
}

FLinearColor FGestureConfigAssetEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.5f, 0.77f, 0.45f, 0.5f);
}

FString FGestureConfigAssetEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "GestureConfig ").ToString();
}

void FGestureConfigAssetEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	if (GestureConfig)
	{
		Collector.AddReferencedObject(GestureConfig);
	}
}

FString FGestureConfigAssetEditor::GetReferencerName() const
{
	return TEXT("FGestureConfigAssetEditor");
}

void FGestureConfigAssetEditor::PostUndo(bool bSuccess)
{
	if (DetailsView.IsValid())
	{
		DetailsView->SetObject(GestureConfig, true);
	}
}

void FGestureConfigAssetEditor::PostRedo(bool bSuccess)
{
	if (DetailsView.IsValid())
	{
		DetailsView->SetObject(GestureConfig, true);
	}
}

TSharedRef<SDockTab> FGestureConfigAssetEditor::SpawnTab_Viewport(const FSpawnTabArgs& Args) const
{
	check(Args.GetTabId() == ViewportTabId);

	return SNew(SDockTab)
		.Label(LOCTEXT("ViewportTab_Title", "Viewport"))
		[
			ViewportWidget.ToSharedRef()
		];
}

TSharedRef<SDockTab> FGestureConfigAssetEditor::SpawnTab_Details(const FSpawnTabArgs& Args) const
{
	check(Args.GetTabId() == DetailsTabId);

	return SNew(SDockTab)
		.Label(LOCTEXT("DetailsTab_Title", "Details"))
		[
			DetailsView.ToSharedRef()
		];
}

void FGestureConfigAssetEditor::CreatePreviewScene()
{
	PreviewScene = MakeShareable(new FAdvancedPreviewScene(FPreviewScene::ConstructionValues()));
	PreviewScene->SetFloorVisibility(false);
}

void FGestureConfigAssetEditor::LoadHandMesh()
{
	if (!PreviewScene.IsValid())
	{
		return;
	}

	USkeletalMesh* HandMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("SkeletalMesh'/PICOXR/Meshes/Hand/RightHand/SM_Hand_R.SM_Hand_R'"));
	if (!HandMesh)
	{
		return;
	}

	HandMeshComponent = NewObject<UPoseableMeshComponent>(GetTransientPackage(), NAME_None, RF_Transient);
	if (HandMeshComponent)
	{
		HandMeshComponent->SetSkeletalMesh(HandMesh);

		FTransform HandTransform;
		HandTransform.SetLocation(FVector(0.0f, 0.0f, 0.0f));
		HandTransform.SetScale3D(FVector(10, 10, 10));

		PreviewScene->AddComponent(HandMeshComponent, HandTransform);
		HandMeshComponent->ResetBoneTransformByName(FName("OpenXR_RIGHT"));

		UpdateHandPose();
	}
}

void FGestureConfigAssetEditor::UpdateHandPose() const
{
	if (!HandMeshComponent || !HandMeshComponent->SkeletalMesh || !GestureConfig)
	{
		return;
	}

	const FShapesRecognizer& Shapes = GestureConfig->ShapesRecognizer;
	const FReferenceSkeleton& RefSkeleton = HandMeshComponent->SkeletalMesh->GetRefSkeleton();

	const int32 NumBones = HandMeshComponent->GetNumBones();
	for (int32 i = 0; i < NumBones; ++i)
	{
		HandMeshComponent->ResetBoneTransformByName(HandMeshComponent->GetBoneName(i));
	}

	auto SetBoneLocalRotation = [&](const FName& BoneName, const FRotator& Rotation)
	{
		const int32 BoneIndex = RefSkeleton.FindBoneIndex(BoneName);
		if (BoneIndex != INDEX_NONE)
		{
			FTransform LocalTransform = RefSkeleton.GetRefBonePose()[BoneIndex];
			LocalTransform.SetRotation(Rotation.Quaternion());

			const int32 ParentBoneIndex = RefSkeleton.GetParentIndex(BoneIndex);
			if (ParentBoneIndex != INDEX_NONE)
			{
				const FName ParentBoneName = RefSkeleton.GetBoneName(ParentBoneIndex);
				const FTransform ParentTransform = HandMeshComponent->GetBoneTransformByName(ParentBoneName, EBoneSpaces::ComponentSpace);
				FTransform ComponentTransform = LocalTransform * ParentTransform;

				HandMeshComponent->SetBoneTransformByName(BoneName, ComponentTransform, EBoneSpaces::ComponentSpace);
			}
			else
			{
				HandMeshComponent->SetBoneTransformByName(BoneName, LocalTransform, EBoneSpaces::ComponentSpace);
			}
		}
	};

	// Thumb
	if (Shapes.Thumb.Flexion == EFlexion::Close)
	{
		SetBoneLocalRotation(FName("right_thumb_proximal"), FRotator(-52.0f, 37.0f, 8.0f));
	}
	else if (Shapes.Thumb.Abduction == EAbduction::Close)
	{
		SetBoneLocalRotation(FName("right_thumb_proximal"), FRotator(-58.0f, -16.0f, -1.0f));
	}

	if (Shapes.Thumb.Curl == ECurl::Close)
	{
		SetBoneLocalRotation(FName("right_thumb_distal"), FRotator(-36.0f, 0.0f, 0.0f));
	}

	// Index
	if (Shapes.Index.Flexion == EFlexion::Close)
	{
		SetBoneLocalRotation(FName("right_index_proximal"), FRotator(-68.0f, 0.0f, 0.0f));
	}
	else if (Shapes.Index.Abduction == EAbduction::Close)
	{
		SetBoneLocalRotation(FName("right_index_proximal"), FRotator(0.0f, -18.0f, 0.0f));
	}

	if (Shapes.Index.Curl == ECurl::Close)
	{
		SetBoneLocalRotation(FName("right_index_intermediate"), FRotator(-60.0f, 0.0f, 0.0f));
		SetBoneLocalRotation(FName("right_index_distal"), FRotator(-65.0f, 0.0f, 0.0f));
	}

	// Middle
	if (Shapes.Middle.Flexion == EFlexion::Close)
	{
		SetBoneLocalRotation(FName("right_middle_proximal"), FRotator(-68.0f, 0.0f, 0.0f));
	}

	if (Shapes.Middle.Curl == ECurl::Close)
	{
		SetBoneLocalRotation(FName("right_middle_intermediate"), FRotator(-60.0f, 0.0f, 0.0f));
		SetBoneLocalRotation(FName("right_middle_distal"), FRotator(-65.0f, 0.0f, 0.0f));
	}

	// Ring
	if (Shapes.Ring.Flexion == EFlexion::Close)
	{
		SetBoneLocalRotation(FName("right_ring_proximal"), FRotator(-68.0f, 0.0f, 0.0f));
	}
	else if (Shapes.Ring.Abduction == EAbduction::Close)
	{
		SetBoneLocalRotation(FName("right_ring_proximal"), FRotator(0.0f, 18.0f, 0.0f));
	}

	if (Shapes.Ring.Curl == ECurl::Close)
	{
		SetBoneLocalRotation(FName("right_ring_intermediate"), FRotator(-60.0f, 0.0f, 0.0f));
		SetBoneLocalRotation(FName("right_ring_distal"), FRotator(-65.0f, 0.0f, 0.0f));
	}

	// Pinky
	if (Shapes.Pinky.Flexion == EFlexion::Close)
	{
		SetBoneLocalRotation(FName("right_little_proximal"), FRotator(-68.0f, 0.0f, 0.0f));
	}
	else if (Shapes.Pinky.Abduction == EAbduction::Close)
	{
		SetBoneLocalRotation(FName("right_little_proximal"), FRotator(0.0f, 18.0f, 0.0f));
	}

	if (Shapes.Pinky.Curl == ECurl::Close)
	{
		SetBoneLocalRotation(FName("right_little_intermediate"), FRotator(-60.0f, 0.0f, 0.0f));
		SetBoneLocalRotation(FName("right_little_distal"), FRotator(-65.0f, 0.0f, 0.0f));
	}

	// Update Hand Orientation based on Track Axis and Track Target
	const FTransRecognizer& Trans = GestureConfig->TransRecognizer;
	FVector UnityRot = FVector::ZeroVector;

	switch (Trans.TrackTarget)
	{
	case ETrackTarget::TowardsFace:
	case ETrackTarget::AwayFromFace:
		if (Trans.TrackAxis == ETrackAxis::Fingers)
		{
			UnityRot = FVector(0.f, 180.f, 0.f);
		}
		else if (Trans.TrackAxis == ETrackAxis::Palm)
		{
			UnityRot = FVector(-90.f, 180.f, 0.f);
		}
		else // Thumb
		{
			UnityRot = FVector(-90.f, 0.f, -90.f);
		}
		break;

	case ETrackTarget::WorldUp:
		if (Trans.TrackAxis == ETrackAxis::Fingers)
		{
			UnityRot = FVector(-90.f, 0.f, 0.f);
		}
		else if (Trans.TrackAxis == ETrackAxis::Palm)
		{
			UnityRot = FVector(0.f, 0.f, 180.f);
		}
		else // Thumb
		{
			UnityRot = FVector(0.f, 0.f, -90.f);
		}
		break;

	case ETrackTarget::WorldDown:
		if (Trans.TrackAxis == ETrackAxis::Fingers)
		{
			UnityRot = FVector(90.f, 0.f, 0.f);
		}
		else if (Trans.TrackAxis == ETrackAxis::Palm)
		{
			UnityRot = FVector(0.f, 0.f, 0.f);
		}
		else // Thumb
		{
			UnityRot = FVector(0.f, 0.f, 90.f);
		}
		break;
	}

	FRotator TargetRot = FRotator(-UnityRot.X, -UnityRot.Y, -UnityRot.Z);

	if (Trans.TrackTarget == ETrackTarget::AwayFromFace)
	{
		TargetRot.Yaw += 180.0f;
	}

	HandMeshComponent->SetRelativeRotation(TargetRot);
}

void FGestureConfigAssetEditor::OnPropertyChanged(const FPropertyChangedEvent&) const
{
	UpdateHandPose();
}

#undef LOCTEXT_NAMESPACE
