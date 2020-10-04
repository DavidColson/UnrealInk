// Copyright 2018 David Colson. All Rights Reserved.

#pragma once

#include "MonoBaseClass.h"

#include "Choice.generated.h"

UCLASS(BlueprintType)
class INK_API UChoice : public UMonoBaseClass
{
	GENERATED_BODY()
public:
	UChoice();

	UFUNCTION(BlueprintPure, Category = Ink)
	FString Text();

	UFUNCTION(BlueprintPure, Category = Ink)
	FString PathStringOnChoice();

	UFUNCTION(BlueprintPure, Category = Ink)
	FString SourcePath();

	UFUNCTION(BlueprintPure, Category = Ink)
	int Index();

private:
	// DO NOT USE! This is called internally with the ink glue instance of this choice.
	// Since the constructor is internal to InkGlue
	friend class UStory;
	static UChoice* NewChoice(MonoObject* MonoChoice);
};