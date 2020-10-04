// Copyright 2018 David Colson. All Rights Reserved.

#pragma once

#include "UObject/Object.h"

#include "Ink.h"

#include "StoryAsset.generated.h"

UCLASS(Blueprintable)
class INK_API UStoryAsset : public UObject
{
	GENERATED_BODY()

public:
	UStoryAsset();

	UPROPERTY()
	FString CompiledStory;

#if WITH_EDITORONLY_DATA
	// Import data for this 
	UPROPERTY(VisibleAnywhere, Instanced, Category = ImportSettings)
	class UAssetImportData* AssetImportData;

	// UObject interface
	virtual void PostInitProperties() override;
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
	virtual void Serialize(FArchive& Ar) override;
	// End of UObject interface
#endif
};
