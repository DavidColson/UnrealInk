// Copyright 2018 David Colson. All Rights Reserved.

#pragma once

#include "Delegates/DelegateCombinations.h"
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

DECLARE_DYNAMIC_DELEGATE_TwoParams(FVariableObserver, FString, variableName, FInkVar, newValue);

extern "C" __declspec(dllexport) void ObserverCallbackInt(int instanceId, const char* variableName, int newValue);
extern "C" __declspec(dllexport) void ObserverCallbackFloat(int instanceId, const char* variableName, float newValue);
extern "C" __declspec(dllexport) void ObserverCallbackString(int instanceId, const char* variableName, const char* newValue);

UCLASS(BlueprintType)
class INK_API UStory : public UMonoBaseClass
{
	GENERATED_BODY()

public:
	UStory();
	~UStory();

	UFUNCTION(BlueprintPure, Category = Ink)
	static int GetInkVersionCurrent() { return 19; }

	UFUNCTION(BlueprintCallable, Category = Ink)
	static UStory* NewStory(UStoryAsset* StoryAsset);

	UFUNCTION(BlueprintCallable, Category = Ink)
	FString Continue();

	UFUNCTION(BlueprintPure, Category = Ink)
	bool CanContinue();

	UFUNCTION(BlueprintCallable, Category = Ink)
	TArray<UChoice*> CurrentChoices();

	UFUNCTION(BlueprintCallable, Category = Ink)
	void ChooseChoiceIndex(int index);

	UFUNCTION(BlueprintPure, Category = Ink)
	FString CurrentText();

	UFUNCTION(BlueprintPure, Category = Ink)
	UStoryState* State();

	UFUNCTION(BlueprintPure, Category = Ink)
	bool HasError();

	UFUNCTION(BlueprintPure, Category = Ink)
	bool HasWarning();

	UFUNCTION(BlueprintPure, Category = Ink)
	TArray<FString> CurrentTags();

	UFUNCTION(BlueprintPure, Category = Ink)
	TArray<FString> GlobalTags();

	UFUNCTION(BlueprintPure, Category = Ink)
	TArray<FString> CurrentErrors();

	UFUNCTION(BlueprintPure, Category = Ink)
	TArray<FString> CurrentWarnings();

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

	UFUNCTION(BlueprintCallable, Category = Ink)
	void ObserveVariable(FString variableName, const FVariableObserver& observer);

	UFUNCTION(BlueprintCallable, Category = Ink)
	void ObserveVariables(TArray<FString> variableNames, const FVariableObserver& observer);

	UFUNCTION(BlueprintCallable, Category = Ink)
	void RemoveVariableObserver(const FVariableObserver& observer, FString specificVariableName = "");

	UFUNCTION(BlueprintCallable, Category = Ink)
	FString BuildStringOfHeirarchy();

	UFUNCTION(BlueprintCallable, Category = Ink)
	UStoryState* CopyStateForBackgroundThreadSave();

	UFUNCTION(BlueprintCallable, Category = Ink)
	void BackgroundSaveComplete();

private:
	typedef TPair<int, FString> FDelegateMapKey;
	static TMap<FDelegateMapKey, TArray<FVariableObserver>> delegateMap;
	static int instanceCounter;
	int instanceId{ -1 };

	friend __declspec(dllexport) void ObserverCallbackInt(int instanceId, const char* variableName, int newValue);
	friend __declspec(dllexport) void ObserverCallbackFloat(int instanceId, const char* variableName, float newValue);
	friend __declspec(dllexport) void ObserverCallbackString(int instanceId, const char* variableName, const char* newValue);
};