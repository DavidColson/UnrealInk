#pragma once

#include "MonoBaseClass.h"

#include "VariablesState.generated.h"

struct FInkVar;

UCLASS(BlueprintType)
class INK_API UVariablesState : public UMonoBaseClass
{
	GENERATED_BODY()
public:
	UVariablesState();

	UFUNCTION(BlueprintPure, Category = Ink)
	TArray<FString> GetVariables();

	UFUNCTION(BlueprintPure, Category = Ink)
	FInkVar GetVariable(const FString& variableName);

	UFUNCTION(BlueprintCallable, Category = Ink)
	void SetVariableFloat(const FString& variableName, float value);

	UFUNCTION(BlueprintCallable, Category = Ink)
	void SetVariableInt(const FString& variableName, int value);

	UFUNCTION(BlueprintCallable, Category = Ink)
	void SetVariableString(const FString& variableName, const FString& value);

	UFUNCTION(BlueprintCallable, Category = Ink)
	void SetVariable(const FString& variableName, const FInkVar& value);

private:
	friend class UStory;
	static UVariablesState* NewVariablesState(MonoObject* MonoVariableState);
};