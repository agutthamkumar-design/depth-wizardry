// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class satillite_imagery : ModuleRules
{
	public satillite_imagery(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
			"ApplicationCore",
			"Slate",
			"SlateCore",
            "HTTP",
            "ImageWrapper",
            "RenderCore",
            "EnhancedInput",
            "AIModule",
            "UMG",
            "StateTreeModule",
            "Json",       
            "JsonUtilities",
            "GameplayStateTreeModule"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"satillite_imagery",
			"satillite_imagery/Variant_Horror",
			"satillite_imagery/Variant_Horror/UI",
			"satillite_imagery/Variant_Shooter",
			"satillite_imagery/Variant_Shooter/AI",
			"satillite_imagery/Variant_Shooter/UI",
			"satillite_imagery/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
