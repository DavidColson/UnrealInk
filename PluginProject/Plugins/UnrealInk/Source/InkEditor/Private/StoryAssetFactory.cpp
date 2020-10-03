// Copyright 2018 David Colson. All Rights Reserved.

#include "StoryAssetFactory.h"

#include "HAL/PlatformFilemanager.h"
#include "Misc/Paths.h"
#include "Interfaces/IPluginManager.h"
#include "InkEditor.h"
#include "StoryAsset.h"
#include "InkCompiler.h"
#include "Misc/FileHelper.h"

////////////////////////////////////////////////////////
UStoryAssetFactory::UStoryAssetFactory()
{
	Formats.Add(FString(TEXT("ink;")) + NSLOCTEXT("UStoryAssetFactory", "FormatTxt", "Ink Story File").ToString());
	Formats.Add(FString(TEXT("json;")) + NSLOCTEXT("UStoryAssetFactory", "FormatTxt", "Compiled Ink Story File").ToString());
	SupportedClass = UStoryAsset::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
}

////////////////////////////////////////////////////////
bool UStoryAssetFactory::FactoryCanImport(const FString & Filename)
{
	if (Filename.EndsWith("ink.json") || Filename.EndsWith("ink"))
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////
UObject * UStoryAssetFactory::FactoryCreateFile(UClass * InClass, UObject * InParent, FName InName, EObjectFlags Flags, const FString & Filename, const TCHAR * Parms, FFeedbackContext * Warn, bool & bOutOperationCanceled)
{
	UStoryAsset* NewStory = nullptr;
	FString FileContents;

	if (Filename.EndsWith("ink.json"))
	{
		// This is precompiled, just save the contents
		if (FFileHelper::LoadFileToString(FileContents, *Filename))
		{
			FString Assetname = InName.ToString();
			NewStory = NewObject<UStoryAsset>(InParent, InClass, FName(*(Assetname.LeftChop(4))), Flags);
			NewStory->CompiledStory = FileContents;
		}

		bOutOperationCanceled = false;

		return NewStory;
	}

	if (Filename.EndsWith("ink"))
	{

		if (FFileHelper::LoadFileToString(FileContents, *Filename))
		{
			// Make new inkcompiler object
			UInkCompiler* compiler = UInkCompiler::NewInkCompiler(FileContents, Filename);
			FString compiledStory = compiler->CompileToJson();


			// Run the function
			NewStory = NewObject<UStoryAsset>(InParent, InClass, InName, Flags);
			NewStory->CompiledStory = compiledStory;

			bOutOperationCanceled = false;

			// profit
			return NewStory;
		}
	}

	return nullptr;
}

UObject* UStoryAssetFactory::FactoryCreateBinary(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd, FFeedbackContext* Warn)
{
	return nullptr;
}
