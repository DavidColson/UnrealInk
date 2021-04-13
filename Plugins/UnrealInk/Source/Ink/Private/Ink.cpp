// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Ink.h"
#include "Modules/ModuleInterface.h"
#include "Interfaces/IPluginManager.h"
#include "HAL/PlatformFilemanager.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "Misc/App.h"

#include "Story.h"
#include "StoryState.h"
#include "Choice.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

#define LOCTEXT_NAMESPACE "FInkModule"

DEFINE_LOG_CATEGORY(LogInk)

static TArray<FString> MonoPreloadSearchPaths;

////////////////////////////////////////////////////////
static MonoAssembly* assembly_preload_hook(MonoAssemblyName *aname, char **assemblies_path, void* user_data)
{
	IFileManager& FileManager = IFileManager::Get();

	const char *name = mono_assembly_name_get_name(aname);
	const char *culture = mono_assembly_name_get_culture(aname);
	auto AsmName = FString(ANSI_TO_TCHAR(name));
	auto AsmCulture = FString(ANSI_TO_TCHAR(culture));

	if (!AsmName.EndsWith(TEXT(".dll"), ESearchCase::IgnoreCase))
	{
		AsmName = AsmName + TEXT(".dll");
	}

	FString AttemptedSearchPaths;

	for (FString searchPath : MonoPreloadSearchPaths)
	{
		// Determine if file exists in the search path
		auto AsmPath = FPaths::Combine(*searchPath, *AsmName);
		if (!FPaths::FileExists(AsmPath))
		{
			AsmPath = FPaths::Combine(*searchPath, *AsmCulture, *AsmName);
			if (!FPaths::FileExists(AsmPath))
			{
				FString AbsoluteAssemblyPath = FileManager.ConvertToAbsolutePathForExternalAppForRead(*AsmPath);
				AttemptedSearchPaths.Appendf(TEXT("%s\n"), *AbsoluteAssemblyPath);
				continue;
			}
		}

		FString AbsoluteAssemblyPath = FileManager.ConvertToAbsolutePathForExternalAppForRead(*AsmPath);

		// Run mono_assmebly_open
		// Return success
		MonoImageOpenStatus status;
		MonoAssembly *loaded_asm = mono_assembly_open(TCHAR_TO_UTF8(*AbsoluteAssemblyPath), &status);
		if (loaded_asm)
		{
			UE_LOG(LogInk, Log, TEXT("Loaded assembly from path '%s'."), *AbsoluteAssemblyPath);
			return loaded_asm;
		}
	}

	UE_LOG(LogInk, Fatal, TEXT("Failed to load assembly '%s' searched in the following folders: \n %s"), *AsmName, *AttemptedSearchPaths);
	
	return nullptr;
}

////////////////////////////////////////////////////////
void FInkModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("UnrealInk")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("ThirdParty/Mono/lib/Win64/mono-2.0-sgen.dll"));
#elif PLATFORM_MAC
    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/mono/mono-2.0-sgen.dylib"));
#endif // PLATFORM_WINDOWS

	MonoLibraryHandle = FPlatformProcess::GetDllHandle(*LibraryPath);


	// Load and init Mono and Ink
	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	// TODO: For packaged builds, this directory should be the engine binaries directory, and the binaries need to be copied in

	FString MonoSearchPath = FPaths::EnginePluginsDir() + "UnrealInk/ThirdParty/Mono/Assemblies/";
	MonoPreloadSearchPaths.Add(MonoSearchPath);
	MonoSearchPath = FPaths::ProjectPluginsDir() + "UnrealInk/ThirdParty/Mono/Assemblies/";
	MonoPreloadSearchPaths.Add(MonoSearchPath);
	MonoSearchPath = FString(FPlatformProcess::BaseDir()) + "/Mono/Assemblies/";
	MonoPreloadSearchPaths.Add(MonoSearchPath);

	FString InkSearchPath = FPaths::EnginePluginsDir() + "UnrealInk/ThirdParty/Ink/";
	MonoPreloadSearchPaths.Add(InkSearchPath);
	InkSearchPath = FPaths::ProjectPluginsDir() + "UnrealInk/ThirdParty/Ink/";
	MonoPreloadSearchPaths.Add(InkSearchPath);
	InkSearchPath = FString(FPlatformProcess::BaseDir()) + "Ink/";
	MonoPreloadSearchPaths.Add(InkSearchPath);

	MonoSearchPath = FString(FPlatformProcess::BaseDir()) + "/";
	MonoPreloadSearchPaths.Add(MonoSearchPath);
	
	mono_install_assembly_preload_hook(assembly_preload_hook, NULL);

	//Init mono into the main domain
	MainDomain = mono_jit_init(TCHAR_TO_UTF8(FApp::GetProjectName()));
	if (!MainDomain)
	{
		UE_LOG(LogInk, Fatal, TEXT("Initializing Mono for Ink plugin failed"));
	}

	// Load Ink Runtime assembly
	MonoAssembly *assembly;
	const char* assemblyPath = TCHAR_TO_UTF8(*FString(BaseDir + "/ThirdParty/Ink/InkGlue.dll"));

	if(!FPaths::FileExists(FString(assemblyPath))) 
		assemblyPath = TCHAR_TO_UTF8(*FString(FString(FPlatformProcess::BaseDir()) + "/Ink/InkGlue.dll"));

	if (!FPaths::FileExists(FString(assemblyPath)))
		assemblyPath = TCHAR_TO_UTF8(*FString(FString(FPlatformProcess::BaseDir()) + "/InkGlue.dll"));

	assembly = mono_domain_assembly_open(MainDomain, assemblyPath);
	if (!assembly)
	{
		UE_LOG(LogInk, Fatal, TEXT("Failed To Load Ink Runtime Assembly"));
	}
	InkAssembly = mono_assembly_get_image(assembly);
}

////////////////////////////////////////////////////////
void FInkModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(MonoLibraryHandle);
	MonoLibraryHandle = nullptr;
}

////////////////////////////////////////////////////////
MonoImage * FInkModule::GetInkAssemblyImage()
{
	return InkAssembly;
}

MonoImage* FInkModule::GetMsCorLibImage()
{
	return Mscorlib;
}

////////////////////////////////////////////////////////
MonoDomain * FInkModule::GetMonoDomain()
{
	return MainDomain;
}

////////////////////////////////////////////////////////
void FInkModule::MaybeThrowMonoException(MonoObject* exception)
{
	if (exception)
	{
		const char* error = mono_string_to_utf8(mono_object_to_string(exception, nullptr));
		UE_LOG(LogInk, Error, TEXT("Ink Runtime Exception : %s"), *FString(error));
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FInkModule, Ink)