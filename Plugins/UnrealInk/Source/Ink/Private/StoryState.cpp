// Copyright 2018 David Colson. All Rights Reserved.

#include "StoryState.h"

#include "Ink.h"

#include <mono/jit/jit.h>


////////////////////////////////////////////////////////
UStoryState::UStoryState()
{

}

////////////////////////////////////////////////////////
UStoryState* UStoryState::NewStoryState(MonoObject* MonoStoryState)
{
	UStoryState* NewStoryState = NewObject<UStoryState>();
	NewStoryState->NewFromInstance(MonoStoryState);
	return NewStoryState;
}

////////////////////////////////////////////////////////
int UStoryState::InkSaveStateVersion()
{
	return MonoInvoke<int>("InkSaveStateVersion", NULL);
}

////////////////////////////////////////////////////////
FString UStoryState::ToJson()
{
	return MonoInvoke<FString>("ToJson", NULL);
}

////////////////////////////////////////////////////////
void UStoryState::LoadJson(FString Json)
{
	MonoString* MonoJson = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*Json));
	void* args[1];
	args[0] = MonoJson;

	MonoInvoke<void>("LoadJson", args);
}

////////////////////////////////////////////////////////
int UStoryState::VisitCountAtPathString(FString PathString)
{
	MonoString* MonoPath = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*PathString));
	void* args[1];
	args[0] = MonoPath;

	return MonoInvoke<int>("VisitCountAtPathString", args);
}

////////////////////////////////////////////////////////
FString UStoryState::CurrentPathString()
{
	return MonoInvoke<FString>("CurrentPathString", NULL);
}

////////////////////////////////////////////////////////
void UStoryState::ForceEnd()
{
	MonoInvoke<void>("ForceEnd", NULL);
}
