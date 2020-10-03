// Copyright 2018 David Colson. All Rights Reserved.

#pragma once

#include "MonoBaseClass.h"

#include "InkCompiler.generated.h"

UCLASS(BlueprintType)
class INK_API UInkCompiler: public UMonoBaseClass
{
	GENERATED_BODY()
public:
	UInkCompiler();

	static UInkCompiler* NewInkCompiler(FString inkFileContents, FString inkFileName);

	UFUNCTION()
	FString CompileToJson();
};
