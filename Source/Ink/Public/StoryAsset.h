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
};
