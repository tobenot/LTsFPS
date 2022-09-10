// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LTsFPS : ModuleRules
{
	public LTsFPS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
