// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System;

public class Ink : ModuleRules
{
	// Module path is into the Ink source module, that is UnrealInk/Source/Ink
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
				Path.Combine(ModulePath, "Public")
				// ... add public include paths required here ...
			}
			);

        PrivateIncludePaths.AddRange(
			new string[] {
                Path.Combine(ModulePath, "Private"),
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

        AddMonoRuntime(Target, pluginDirectory);


        // Copy dlls and assemblies to output directory
        // --------------------------------------------

        string PlatformString = "";
        string SgenAssemblyFileName = "mono-2.0-sgen.dll"; // default
        if(Target.Platform == UnrealTargetPlatform.Win64)
        {
            PlatformString = "Win64";
        }
        else if(Target.Platform == UnrealTargetPlatform.Win32)
        {
            PlatformString = "Win32";
        }
        else if(Target.Platform == UnrealTargetPlatform.Mac)
        {
            PlatformString = "Mac";
            SgenAssemblyFileName = "libmonosgen-2.0.dylib";
        }
        else if(Target.Platform == UnrealTargetPlatform.Linux)
        {
            // todo: 64bit vs 32bit, differences on Linux?
            PlatformString = "Linux";
            SgenAssemblyFileName = "libmonosgen-2.0.so";
        }
        RuntimeDependencies.Add(("$(TargetOutputDir)/" + SgenAssemblyFileName), Path.Combine(pluginDirectory, "ThirdParty/Mono/lib", PlatformString, SgenAssemblyFileName));

        // Mono Assemblies
        RuntimeDependencies.Add("$(TargetOutputDir)/...", Path.Combine(pluginDirectory, "ThirdParty/Mono/Assemblies/..."));

        // Ink Assemblies
        RuntimeDependencies.Add("$(TargetOutputDir)/ink_compiler.dll", Path.Combine(pluginDirectory, "ThirdParty/Ink/ink_compiler.dll"));
        RuntimeDependencies.Add("$(TargetOutputDir)/ink-engine-runtime.dll", Path.Combine(pluginDirectory, "ThirdParty/Ink/ink-engine-runtime.dll"));
        RuntimeDependencies.Add("$(TargetOutputDir)/InkGlue.dll", Path.Combine(pluginDirectory, "ThirdParty/Ink/InkGlue.dll"));

        PrecompileForTargets = PrecompileTargetsType.Any;
    }

    void AddMonoRuntime(ReadOnlyTargetRules Target, string MonoUEPluginDirectory)
    {
        string MonoLibPath = MonoUEPluginDirectory + "/ThirdParty/Mono/lib/" + Target.Platform;
        string LibraryName;
        if(Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Win32)
        {
            LibraryName = "mono-2.0-sgen";
            PublicAdditionalLibraries.Add(Path.Combine(MonoLibPath, LibraryName + ".lib"));
            PublicDelayLoadDLLs.Add(LibraryName + ".dll");
        }
        else if(Target.Platform == UnrealTargetPlatform.Mac)
        {
            LibraryName = "libmonosgen-2.0";
            PublicAdditionalLibraries.Add("iconv");
            PublicDelayLoadDLLs.Add(LibraryName + ".dylib");
        }
        else if(Target.Platform == UnrealTargetPlatform.Linux)
        {
            LibraryName = "libmonosgen-2.0";
            PublicAdditionalLibraries.Add("iconv");
            PublicDelayLoadDLLs.Add(LibraryName + ".so");
        }
        else
        {
            throw new BuildException("Mono not supported on target platform '{0}'", Target.Platform);
        }

        PublicDefinitions.Add("MONO_IS_DYNAMIC_LIB=1");
    }
}
