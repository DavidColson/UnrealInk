// Copyright 2018 David Colson. All Rights Reserved.

#pragma once

#include "MonoBaseClass.h"

#include "Story.generated.h"

class UStoryAsset;
class UChoice;
class UStoryState;

// A wrapper for passing around ink vars to and fro ink itself
// Not templated so it can be used in blueprints
UENUM(BlueprintType)
enum class EInkVarType : uint8
{
	Float,
	Int,
	String,
	None
};

USTRUCT(BlueprintType)
struct FInkVar
{
	GENERATED_BODY()

	FInkVar() { type = EInkVarType::None; }

	FInkVar(float val) { type = EInkVarType::Float; floatVar = val; }
	FInkVar(int val) { type = EInkVarType::Int; intVar = val; }
	FInkVar(FString val) { type = EInkVarType::String; stringVar = val; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ink")
	EInkVarType type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ink")
	float floatVar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ink")
	int intVar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ink")
	FString stringVar;
};


UCLASS(BlueprintType)
class INK_API UStory : public UMonoBaseClass
{
	GENERATED_BODY()

public:
	UStory();

	UFUNCTION(BlueprintCallable, Category = Ink)
	static UStory* NewStory(UStoryAsset* StoryAsset);

	UFUNCTION(BlueprintCallable, Category = Ink)
	FString Continue();

	UFUNCTION(BlueprintCallable, Category = Ink)
	bool CanContinue();

	UFUNCTION(BlueprintCallable, Category = Ink)
	TArray<UChoice*> CurrentChoices();

	UFUNCTION(BlueprintCallable, Category = Ink)
	void ChooseChoiceIndex(int index);

	UFUNCTION(BlueprintCallable, Category = Ink)
	FString CurrentText();

	UFUNCTION(BlueprintCallable, Category = Ink)
	UStoryState* State();

	UFUNCTION(BlueprintCallable, Category = Ink)
	bool HasError();

	UFUNCTION(BlueprintCallable, Category = Ink)
	TArray<FString> CurrentTags();

	UFUNCTION(BlueprintCallable, Category = Ink)
	TArray<FString> CurrentErrors();

	UFUNCTION(BlueprintCallable, Category = Ink)
	void ResetState();

	UFUNCTION(BlueprintCallable, Category = Ink)
	void ResetErrors();

	UFUNCTION(BlueprintCallable, Category = Ink)
	void ResetCallstack();

	UFUNCTION(BlueprintCallable, Category = Ink)
	void ContinueAsync(float millisecondLimitAsync);

	UFUNCTION(BlueprintCallable, Category = Ink)
	FString ContinueMaximally();

	UFUNCTION(BlueprintCallable, Category = Ink)
	bool AsyncContinueComplete();

	UFUNCTION(BlueprintCallable, Category = Ink)
	TArray<FString> TagsForContentAtPath(FString Path);

	UFUNCTION(BlueprintCallable, Category = Ink)
	void ChoosePathString(FString Path, bool ResetCallstack, TArray<FInkVar> vars);
};