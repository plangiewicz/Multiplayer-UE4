// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class FuryFunTaleProd : ModuleRules
{
    public FuryFunTaleProd(TargetInfo Target)
    {
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "Slate", "SlateCore" });

        PrivateDependencyModuleNames.AddRange(new string[] { });
        
    }
}
