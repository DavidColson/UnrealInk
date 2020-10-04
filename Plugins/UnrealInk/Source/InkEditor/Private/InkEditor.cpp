// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "InkEditor.h"

#include "AssetToolsModule.h"
#include "MessageLogModule.h"

#include "StoryAssetFactory.h"

#define LOCTEXT_NAMESPACE "FInkEditorModule"

DEFINE_LOG_CATEGORY(LogInkEditor)

////////////////////////////////////////////////////////
void FInkEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	UE_LOG(LogInkEditor, Log, TEXT("Ink Editor Code Loaded"));

	// Register asset types
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	{
		TSharedRef<IAssetTypeActions> Action = MakeShareable(new FAssetTypeActions_StoryAsset);
		AssetTools.RegisterAssetTypeActions(Action);
	}

	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	{
		FMessageLogInitializationOptions InitOptions;
		InitOptions.bAllowClear = true;
		InitOptions.bShowFilters = true;
		MessageLogModule.RegisterLogListing("InkCompiler", LOCTEXT("InkCompiler", "Ink Compiler"), InitOptions);
	}
}

////////////////////////////////////////////////////////
void FInkEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FInkEditorModule, InkEditor)