// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class Ink : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }

    private string BinariesPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../Binaries/")); }
    }

    private string LibraryPath
    {
        get { return Path.GetFullPath(Path.Combine(ThirdPartyPath, "Mono", "lib")); }
    }

    public Ink(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;

        PublicIncludePaths.AddRange(
			new string[] {
				"Ink/Public"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"Ink/Private",
                 Path.Combine(ThirdPartyPath, "Mono", "include")
            }
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Projects",
                "CoreUObject",
                "Engine",
				// ... add other public dependencies that you statically link with here ...
			}
			);

        string pluginDirectory = ModuleDirectory + "/../..";

        string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "Win64" : "Win32";

        PublicLibraryPaths.Add(pluginDirectory +  "/ThirdParty/Mono/lib/" + PlatformString);

        // Add the import library
        PublicAdditionalLibraries.Add("mono-2.0-sgen.lib");
        // Delay-load the DLL, so we can load it from the right place first
        PublicDelayLoadDLLs.Add("mono-2.0-sgen.dll");

	}
}