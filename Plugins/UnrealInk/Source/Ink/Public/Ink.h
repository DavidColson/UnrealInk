// Copyright 2020 David Colson. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"

typedef struct _MonoImage MonoImage;
typedef struct _MonoDomain MonoDomain;
typedef struct _MonoObject MonoObject;

DECLARE_LOG_CATEGORY_EXTERN(LogInk, Log, All)

class FInkModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	// TODO: Make this a singleton private class where we do InkRuntime::Get()->Image
	// Raw access to Ink assembly. Not recommended to use this, mostly internal use only
	MonoImage* GetInkAssemblyImage();

	// TODO: Make this a singleton private class where we do InkRuntime::Get()->Image
	MonoImage* GetMsCorLibImage();

	// TODO: Make this a singleton private class where we do MonoDomain::Get()
	// Raw access to mono domain in which the Ink Runtime is being used
	MonoDomain* GetMonoDomain();

	void MaybeThrowMonoException(MonoObject* exception);

private:
	/** Handle to the mono dll */
	void*	MonoLibraryHandle;

	MonoImage* Mscorlib;
	MonoImage* InkAssembly;
	MonoDomain* MainDomain;
};