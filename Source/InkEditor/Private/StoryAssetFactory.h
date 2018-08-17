// Copyright 2018 David Colson. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"

#include "StoryAssetFactory.generated.h"

UCLASS(hidecategories = Object)
class UStoryAssetFactory : public UFactory
{
	GENERATED_BODY()

	UStoryAssetFactory();

	virtual bool FactoryCanImport(const FString& Filename);

	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
};