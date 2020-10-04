// Copyright 2018 David Colson. All Rights Reserved.

#pragma once

#include "MonoBaseClass.h"

#include "StoryState.generated.h"

UCLASS(BlueprintType)
class INK_API UStoryState : public UMonoBaseClass
{
	GENERATED_BODY()
public:
	UStoryState();

	UFUNCTION(BlueprintCallable, Category = Ink)
	int InkSaveStateVersion();

	UFUNCTION(BlueprintCallable, Category = Ink)
	FString ToJson();

	UFUNCTION(BlueprintCallable, Category = Ink)
	void LoadJson(FString Json);

	UFUNCTION(BlueprintCallable, Category = Ink)
	int VisitCountAtPathString(FString PathString);

	UFUNCTION(BlueprintPure, Category = Ink)
	FString CurrentPathString();

	UFUNCTION(BlueprintCallable, Category = Ink)
	void ForceEnd();

private:
	// DO NOT USE! This is called internally with the ink glue instance of this choice.
	// Since the constructor is internal to InkGlue
	friend class UStory;
	static UStoryState* NewStoryState(MonoObject* MonoStoryState);
};