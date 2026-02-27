// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PICOGestureEditor : ModuleRules
{
	public PICOGestureEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
				new string[]
				{
					// ... add public include paths required here ...
				}
			);


		PrivateIncludePaths.AddRange(
				new string[]
				{
					// ... add other private include paths required here ...
				}
			);


		PublicDependencyModuleNames.AddRange(
				new[]
				{
					"Core", "PICOGesture", "PXRPlugin", "PICOXRInput", "PICOXRHMD"
					// ... add other public dependencies that you statically link with here ...
				}
			);


		PrivateDependencyModuleNames.AddRange(
				new[]
				{
					"CoreUObject", "Engine", "Slate", "SlateCore", "EditorStyle", "InputCore", "AdvancedPreviewScene", "UnrealEd"
					// ... add private dependencies that you statically link with here ...
				}
			);


		DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
					// ... add any modules that your module loads dynamically here ...
				}
			);
	}
}
