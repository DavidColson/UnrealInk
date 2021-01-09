// Copyright 2018 David Colson. All Rights Reserved.

#include "StoryAssetFactory.h"

#include "HAL/PlatformFilemanager.h"
#include "Misc/Paths.h"
#include "Interfaces/IPluginManager.h"
#include "InkEditor.h"
#include "InkCompiler.h"
#include "Misc/FileHelper.h"
#include "Misc/FeedbackContext.h"
#include "Logging/MessageLog.h"
#include "EditorFramework/AssetImportData.h"

void FAssetTypeActions_StoryAsset::GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const
{
	for (auto& Asset : TypeAssets)
	{
		const auto StoryAsset = CastChecked<UStoryAsset>(Asset);
		if (StoryAsset->AssetImportData)
		{
			StoryAsset->AssetImportData->ExtractFilenames(OutSourceFilePaths);
		}
	}
}


////////////////////////////////////////////////////////
UStoryAssetFactory::UStoryAssetFactory()
{
	Formats.Add(FString(TEXT("ink;")) + NSLOCTEXT("UTextAssetFactory", "FormatTxt", "Ink Story File").ToString());
	Formats.Add(FString(TEXT("json;")) + NSLOCTEXT("UTextAssetFactory", "FormatTxt", "Compiled Ink Story File").ToString());
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
	FString FileExtension = FPaths::GetExtension(Filename);

	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, InClass, InParent, InName, *FileExtension);

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
			NewStory->AssetImportData->Update(Filename);
		}

		bOutOperationCanceled = false;

		GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, NewStory);
		return NewStory;
	}
	else if (Filename.EndsWith("ink"))
	{

		if (FFileHelper::LoadFileToString(FileContents, *Filename))
		{
			// Make new inkcompiler object
			UInkCompiler* compiler = UInkCompiler::NewInkCompiler(FileContents, Filename);

			FMessageLog InkCompilerLog("InkCompiler");

			TArray<FString> warnings = compiler->GetWarnings();
			if (warnings.Num() != 0)
			{
				for (int i = 0; i < warnings.Num(); i++)
					InkCompilerLog.Warning(FText::FromString(warnings[i]));
			}

			TArray<FString> authorMessages = compiler->GetAuthorMessages();
			if (authorMessages.Num() != 0)
			{
				for (int i = 0; i < authorMessages.Num(); i++)
					InkCompilerLog.Info(FText::FromString(authorMessages[i]));
			}
			
			TArray<FString> errors = compiler->GetErrors();
			if (errors.Num() != 0)
			{
				for (int i = 0; i < errors.Num(); i++)
					InkCompilerLog.Error(FText::FromString(errors[i]));

				InkCompilerLog.Open(EMessageSeverity::Error);
				bOutOperationCanceled = true;
				GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, nullptr);
				return nullptr;
			}


			FString compiledStory = compiler->CompileToJson();

			// Run the function
			NewStory = NewObject<UStoryAsset>(InParent, InClass, InName, Flags);
			NewStory->CompiledStory = compiledStory;
			NewStory->AssetImportData->Update(Filename);

			bOutOperationCanceled = false;

			// profit
			GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, NewStory);
			return NewStory;
		}
	}

	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, nullptr);
	return nullptr;
}

bool UStoryAssetFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	UStoryAsset* StoryAsset = Cast<UStoryAsset>(Obj);
	if (StoryAsset && StoryAsset->AssetImportData)
	{
		StoryAsset->AssetImportData->ExtractFilenames(OutFilenames);
		return true;
	}
	return false;
}

void UStoryAssetFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	UStoryAsset* StoryAsset = Cast<UStoryAsset>(Obj);
	if (StoryAsset && ensure(NewReimportPaths.Num() == 1))
	{
		StoryAsset->AssetImportData->UpdateFilenameOnly(NewReimportPaths[0]);
	}
}

EReimportResult::Type UStoryAssetFactory::Reimport(UObject* Obj)
{
	UStoryAsset* StoryAsset = Cast<UStoryAsset>(Obj);
	if (StoryAsset != nullptr)
	{
		if (StoryAsset->AssetImportData)
		{
			bool bOperationCancelled = false;
			if (ImportObject(StoryAsset->GetClass(), StoryAsset->GetOuter(), *StoryAsset->GetName(), RF_Public | RF_Standalone, StoryAsset->AssetImportData->GetFirstFilename(), nullptr, bOperationCancelled) != nullptr)
			{
				return EReimportResult::Succeeded;
			}
		}
	}

	return EReimportResult::Failed;
}

int32 UStoryAssetFactory::GetPriority() const
{
	return ImportPriority;
}
