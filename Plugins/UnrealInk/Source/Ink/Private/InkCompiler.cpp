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

	MonoString* monoInkFileContents = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*(inkFileContents)));
	MonoString* monoInkFileName = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*(inkFileName)));
	void* args[2];
	args[0] = monoInkFileContents;
	args[1] = monoInkFileName;

	NewCompiler->MonoNew(args, 2);

	return NewCompiler;
}

////////////////////////////////////////////////////////
TArray<FString> UInkCompiler::GetErrors()
{
	TArray<FString> errors;

	MonoArray* result = MonoInvoke<MonoArray*>("GetErrors", nullptr);
	for (uintptr_t i = 0; i < mono_array_length(result); i++)
	{
		MonoString* variableName = mono_array_get(result, MonoString*, i);
		errors.Add(FString(mono_string_to_utf8(variableName)));
	}

	return errors;
}

////////////////////////////////////////////////////////
TArray<FString> UInkCompiler::GetWarnings()
{
	TArray<FString> warnings;

	MonoArray* result = MonoInvoke<MonoArray*>("GetWarnings", nullptr);
	for (uintptr_t i = 0; i < mono_array_length(result); i++)
	{
		MonoString* variableName = mono_array_get(result, MonoString*, i);
		warnings.Add(FString(mono_string_to_utf8(variableName)));
	}

	return warnings;
}

////////////////////////////////////////////////////////
TArray<FString> UInkCompiler::GetAuthorMessages()
{
	TArray<FString> messages;

	MonoArray* result = MonoInvoke<MonoArray*>("GetAuthorMessages", nullptr);
	for (uintptr_t i = 0; i < mono_array_length(result); i++)
	{
		MonoString* variableName = mono_array_get(result, MonoString*, i);
		messages.Add(FString(mono_string_to_utf8(variableName)));
	}

	return messages;
}

////////////////////////////////////////////////////////
FString UInkCompiler::CompileToJson()
{
	return MonoInvoke<FString>("CompileToJson", NULL);
}

