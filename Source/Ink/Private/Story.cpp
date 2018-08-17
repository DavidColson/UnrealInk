// Copyright 2018 David Colson. All Rights Reserved.

#include "Story.h"

#include "StoryAsset.h"
#include "StoryState.h"
#include "Choice.h"
#include "Ink.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#include <mono/jit/jit.h>


////////////////////////////////////////////////////////
UStory::UStory()
{

}

////////////////////////////////////////////////////////
UStory* UStory::NewStory(UStoryAsset* StoryAsset)
{
	UStory* NewStory = NewObject<UStory>();

	if (StoryAsset == nullptr)
	{
		UE_LOG(LogInk, Error, TEXT("Invalid Story Asset"));
		return nullptr;
	}

	if (StoryAsset->CompiledStory.IsEmpty())
	{
		UE_LOG(LogInk, Error, TEXT("Invalid Story Asset"));
		return nullptr;
	}
	
	MonoString* jsonString = mono_string_new(mono_domain_get(), TCHAR_TO_ANSI(*(StoryAsset->CompiledStory)));
	void* args[1];
	args[0] = jsonString;

	NewStory->MonoNew(args, 1);

	return NewStory;
}

////////////////////////////////////////////////////////
FString UStory::Continue()
{
	return MonoInvoke<FString>("Continue", NULL);
}

////////////////////////////////////////////////////////
bool UStory::CanContinue()
{
	return MonoInvoke<bool>("CanContinue", NULL);
}

////////////////////////////////////////////////////////
TArray<UChoice*> UStory::CurrentChoices()
{
	// TODO FString here should be replaced with our analog of the choice type
	TArray<UChoice*> returnChoices;

	MonoArray* Result = MonoInvoke<MonoArray*>("CurrentChoices", NULL);
	for (int i = 0; i < mono_array_length(Result); i++)
	{
		MonoObject* MonoChoiceInstance = mono_array_get(Result, MonoObject*, i);
		returnChoices.Add(UChoice::NewChoice(MonoChoiceInstance));
	}

	return returnChoices;
}

////////////////////////////////////////////////////////
void UStory::ChooseChoiceIndex(int index)
{
	void* args[1];
	args[0] = &index;
	MonoInvoke<void>("ChooseChoiceIndex", args);
}

////////////////////////////////////////////////////////
FString UStory::CurrentText()
{
	return MonoInvoke<FString>("CurrentText", NULL);
}

////////////////////////////////////////////////////////
UStoryState* UStory::State()
{
	MonoObject* MonoStateInstance = MonoInvoke<MonoObject*>("State", NULL);
	return UStoryState::NewStoryState(MonoStateInstance);
}

////////////////////////////////////////////////////////
bool UStory::HasError()
{
	return MonoInvoke<bool>("HasError", NULL);
}

////////////////////////////////////////////////////////
TArray<FString> UStory::CurrentTags()
{
	TArray<FString>  returnTags;
	MonoArray* MonoTags = MonoInvoke<MonoArray*>("CurrentTags", NULL);
	for (int i = 0; i < mono_array_length(MonoTags); i++)
	{
		MonoString* String = mono_array_get(MonoTags, MonoString*, i);
		returnTags.Add(FString(mono_string_to_utf8(String)));
	}
	return returnTags;
}

////////////////////////////////////////////////////////
TArray<FString> UStory::CurrentErrors()
{
	TArray<FString>  returnErrors;
	MonoArray* MonoErrors = MonoInvoke<MonoArray*>("CurrentErrors", NULL);
	for (int i = 0; i < mono_array_length(MonoErrors); i++)
	{
		MonoString* String = mono_array_get(MonoErrors, MonoString*, i);
		returnErrors.Add(FString(mono_string_to_utf8(String)));
	}
	return returnErrors;
}

////////////////////////////////////////////////////////
void UStory::ResetState()
{
	MonoInvoke<void>("ResetState", NULL);
}

////////////////////////////////////////////////////////
void UStory::ResetErrors()
{
	MonoInvoke<void>("ResetErrors", NULL);
}

////////////////////////////////////////////////////////
void UStory::ResetCallstack()
{
	MonoInvoke<void>("ResetCallstack", NULL);
}

////////////////////////////////////////////////////////
void UStory::ContinueAsync(float millisecondLimitAsync)
{
	void* args[1];
	args[0] = &millisecondLimitAsync;
	MonoInvoke<void>("ContinueAsync", args);
}

////////////////////////////////////////////////////////
FString UStory::ContinueMaximally()
{
	return MonoInvoke<FString>("ContinueMaximally", NULL);
}

////////////////////////////////////////////////////////
bool UStory::AsyncContinueComplete()
{
	return MonoInvoke<bool>("AsyncContinueComplete", NULL);
}

////////////////////////////////////////////////////////
TArray<FString> UStory::TagsForContentAtPath(FString Path)
{
	TArray<FString>  returnTags;

	MonoString* MonoPath = mono_string_new(mono_domain_get(), TCHAR_TO_ANSI(*Path));
	void* args[1];
	args[0] = MonoPath;
	MonoArray* MonoTags = MonoInvoke<MonoArray*>("TagsForContentAtPath", args);
	for (int i = 0; i < mono_array_length(MonoTags); i++)
	{
		MonoString* String = mono_array_get(MonoTags, MonoString*, i);
		returnTags.Add(FString(mono_string_to_utf8(String)));
	}
	return returnTags;
}

// Useful variadic template
//////////////////////////////////////////////////////////
//template <int Size>
//struct Params {
//	void* data[Size];
//
//	template <typename... Args>
//	constexpr Params(Args... args) : data{args ...} {}
//};
//
//template<typename... Args>
//void UStory::ChoosePathString(FString Path, bool ResetCallstack, Args... args)
//{
//	Params<sizeof...(args)> ParamsPack(&args...);
//

//////////////////////////////////////////////////////////

void UStory::ChoosePathString(FString Path, bool ResetCallstack, TArray<FInkVar> vars)
{
	MonoArray* argsArray;
	argsArray = mono_array_new(mono_domain_get(), mono_get_object_class(), vars.Num());

	for (int i = 0; i < vars.Num(); i++)
	{
		FInkVar& var = vars[i];

		MonoObject* BoxedParam = nullptr;

		switch (var.type)
		{
		case EInkVarType::Float:
			BoxedParam = mono_value_box(mono_domain_get(), mono_get_single_class(), &var.floatVar);
			break;
		case EInkVarType::Int:
			BoxedParam = mono_value_box(mono_domain_get(), mono_get_int32_class(), &var.intVar);
			break;
		case EInkVarType::String:
			BoxedParam = (MonoObject*)mono_string_new(mono_domain_get(), TCHAR_TO_ANSI(*var.stringVar));
			//BoxedParam = mono_value_box(mono_domain_get(), mono_get_string_class(), monoString);
			break;
		}

		mono_array_set(argsArray, MonoObject*, i, BoxedParam);
	}

	void* Params[3];
	Params[0] = mono_string_new(mono_domain_get(), TCHAR_TO_ANSI(*Path));
	Params[1] = argsArray;
	Params[2] = &ResetCallstack;

	MonoInvoke<void>("ChoosePathString", Params);
}