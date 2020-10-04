// Copyright 2018 David Colson. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"
#include "EditorReimportHandler.h"
#include "AssetTypeActions_Base.h"
#include "StoryAsset.h"
#include "StoryAssetFactory.generated.h"

class FAssetTypeActions_StoryAsset : public FAssetTypeActions_Base
{
public:
	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_StoryAsset", "Story Asset"); }
	virtual FColor GetTypeColor() const override { return FColor(175, 0, 128); }
	virtual UClass* GetSupportedClass() const override { return UStoryAsset::StaticClass(); }
	virtual bool IsImportedAsset() const override { return true; }
	virtual uint32 GetCategories() override { return EAssetTypeCategories::Misc; }
	virtual void GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const override;
};

UCLASS(hidecategories = Object)
class UStoryAssetFactory : public UFactory, public FReimportHandler
{
	GENERATED_BODY()

	UStoryAssetFactory();

	//~ UFactory interface
	virtual bool FactoryCanImport(const FString& Filename);
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
	//~ UFactory interface

	//~ Begin FReimportHandler Interface
	virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	virtual void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;
	virtual EReimportResult::Type Reimport(UObject* Obj) override;
	virtual int32 GetPriority() const override;
	//~ End FReimportHandler Interface
};