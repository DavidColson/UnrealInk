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

	UFUNCTION(BlueprintCallable, Category = Ink)
	FString Text();

	UFUNCTION(BlueprintCallable, Category = Ink)
	FString PathStringOnChoice();

	UFUNCTION(BlueprintCallable, Category = Ink)
	FString SourcePath();

	UFUNCTION(BlueprintCallable, Category = Ink)
	int Index();

private:
	// DO NOT USE! This is called internally with the ink glue instance of this choice.
	// Since the constructor is internal to InkGlue
	friend class UStory;
	static UChoice* NewChoice(MonoObject* MonoChoice);
};