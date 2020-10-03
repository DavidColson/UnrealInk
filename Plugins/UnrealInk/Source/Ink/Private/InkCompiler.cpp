// Copyright 2018 David Colson. All Rights Reserved.

#include "InkCompiler.h"

#include "Ink.h"

#include <mono/jit/jit.h>


////////////////////////////////////////////////////////
UInkCompiler::UInkCompiler()
{

}

////////////////////////////////////////////////////////
UInkCompiler* UInkCompiler::NewInkCompiler(FString inkFileContents, FString inkFileName)
{
	UInkCompiler* NewCompiler = NewObject<UInkCompiler>();

	MonoString* monoInkFileContents = mono_string_new(mono_domain_get(), TCHAR_TO_ANSI(*(inkFileContents)));
	MonoString* monoInkFileName = mono_string_new(mono_domain_get(), TCHAR_TO_ANSI(*(inkFileName)));
	void* args[2];
	args[0] = monoInkFileContents;
	args[1] = monoInkFileName;

	NewCompiler->MonoNew(args, 2);

	return NewCompiler;
}

////////////////////////////////////////////////////////
FString UInkCompiler::CompileToJson()
{
	return MonoInvoke<FString>("CompileToJson", NULL);
}

