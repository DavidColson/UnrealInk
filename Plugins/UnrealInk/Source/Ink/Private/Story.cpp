// Copyright 2018 David Colson. All Rights Reserved.

#include "Story.h"

#include "StoryAsset.h"
#include "StoryState.h"
#include "Choice.h"
#include "Ink.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#include <mono/jit/jit.h>


int UStory::instanceCounter = 0;
TMap<TPair<int, FString>, TArray<FVariableObserver>> UStory::delegateMap({});

////////////////////////////////////////////////////////
UStory::UStory()
{

}

extern "C" __declspec(dllexport) void ObserverCallback(int instanceId, const char* variableName, FInkVarInterop* newValue)
{
	for (auto& _delegate : UStory::delegateMap[UStory::FDelegateMapKey(instanceId, FString(variableName))])
	{
		_delegate.ExecuteIfBound(FString(variableName), FInkVar(*newValue));
	}
}

UStory::~UStory()
{
	TArray<FDelegateMapKey> keysToRemove;
	for (auto& element : delegateMap)
	{
		if (element.Key.Key == instanceId)
		{
			keysToRemove.Add(element.Key);
		}
	}
	for (auto& key : keysToRemove)
	{
		delegateMap.Remove(key);
	}
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

	NewStory->instanceId = instanceCounter++;

	MonoString* jsonString = mono_string_new(mono_domain_get(), TCHAR_TO_ANSI(*(StoryAsset->CompiledStory)));
	void* args[2];
	args[0] = jsonString;
	args[1] = &(NewStory->instanceId);

	NewStory->MonoNew(args, 2);

	// Manually added methods (have unique parameters)
	NewStory->ManualMethodBind("ObserveVariable", 1);
	NewStory->ManualMethodBind("RemoveVariableObserver", 1);

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
	for (size_t i = 0; i < mono_array_length(Result); i++)
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
bool UStory::HasWarning()
{
	return MonoInvoke<bool>("HasWarning", NULL);
}

////////////////////////////////////////////////////////
TArray<FString> UStory::CurrentTags()
{
	TArray<FString>  returnTags;
	MonoArray* MonoTags = MonoInvoke<MonoArray*>("CurrentTags", NULL);
	for (size_t i = 0; i < mono_array_length(MonoTags); i++)
	{
		MonoString* String = mono_array_get(MonoTags, MonoString*, i);
		returnTags.Add(FString(mono_string_to_utf8(String)));
	}
	return returnTags;
}

////////////////////////////////////////////////////////
TArray<FString> UStory::GlobalTags()
{
	TArray<FString>  returnTags;
	MonoArray* MonoTags = MonoInvoke<MonoArray*>("GlobalTags", NULL);
	for (size_t i = 0; i < mono_array_length(MonoTags); i++)
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
	for (size_t i = 0; i < mono_array_length(MonoErrors); i++)
	{
		MonoString* String = mono_array_get(MonoErrors, MonoString*, i);
		returnErrors.Add(FString(mono_string_to_utf8(String)));
	}
	return returnErrors;
}

////////////////////////////////////////////////////////
TArray<FString> UStory::CurrentWarnings()
{
	TArray<FString>  returnWarnings;
	MonoArray* MonoWarnings = MonoInvoke<MonoArray*>("CurrentWarnings", NULL);
	for (size_t i = 0; i < mono_array_length(MonoWarnings); i++)
	{
		MonoString* String = mono_array_get(MonoWarnings, MonoString*, i);
		returnWarnings.Add(FString(mono_string_to_utf8(String)));
	}
	return returnWarnings;
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
	for (size_t i = 0; i < mono_array_length(MonoTags); i++)
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

////////////////////////////////////////////////////////
void UStory::ObserveVariable(FString variableName, const FVariableObserver & observer)
{
	FDelegateMapKey key = FDelegateMapKey(instanceId, variableName);

	// If a delegate as already been bound to this variable, add it to the list of delegates for that key, and don't bother telling C# about it
	if (delegateMap.Contains(key))
	{
		delegateMap[key].Add(observer);
	}
	else
	{
		TArray<FVariableObserver> observers;
		observers.Add(observer);
		delegateMap.Add(key, observers);

		void* Params[1];
		Params[0] = mono_string_new(mono_domain_get(), TCHAR_TO_ANSI(*variableName));
		MonoInvoke<void>("ObserveVariable", Params);
	}
}

////////////////////////////////////////////////////////
void UStory::ObserveVariables(TArray<FString> variableNames, const FVariableObserver & observer)
{
	for (auto& variableName : variableNames)
	{
		ObserveVariable(variableName, observer);
	}
}

////////////////////////////////////////////////////////
void UStory::RemoveVariableObserver(const FVariableObserver& observer, FString specificVariableName /*= ""*/)
{
	// Case 1, variable specified, delegate specified, Ubind this variable from this delegate
	if (specificVariableName != "")
	{
		FDelegateMapKey key = FDelegateMapKey(instanceId, specificVariableName);
		TArray<FVariableObserver>& delegates = delegateMap[key];
		delegates.Remove(observer);

		// -------HACK----------
		// Due to a bug in ink runtime, we won't actually call RenoveVariableObserver as it results in a null reference exception.
		// When ink is updated next we can undo this hack
		
		// if there are no more delegates bound to this specific variable you're free to unbind InternalObserve
		/*if (delegates.Num() == 0)
		{
			void* Params[1];
			Params[0] = mono_string_new(mono_domain_get(), TCHAR_TO_ANSI(*specificVariableName));
			MonoInvoke<void>("RemoveVariableObserver", Params);
		}*/
	}
	// Case 2, variable not specified, delegate specified Unbind all variables bound to this delegate
	else
	{
		for (auto& element : delegateMap)
		{
			TArray<FVariableObserver>& delegates = element.Value;
			delegates.Remove(observer);

			// -------HACK----------
			// Due to a bug in ink runtime, we won't actually call RenoveVariableObserver as it results in a null reference exception.

			// Likewise, if there are 0 delegates left bound to this variable, unbind InternalObserve
			/*if (delegates.Num() == 0)
			{
				FString varName = element.Key.Value;

				void* Params[1];
				Params[0] = mono_string_new(mono_domain_get(), TCHAR_TO_ANSI(*element.Key.Value));
				MonoInvoke<void>("RemoveVariableObserver", Params);
			}*/
		}
	}
}

////////////////////////////////////////////////////////
FString UStory::BuildStringOfHeirarchy()
{
	return MonoInvoke<FString>("BuildStringOfHeirarchy", NULL);
}

////////////////////////////////////////////////////////
UStoryState* UStory::CopyStateForBackgroundThreadSave()
{
	MonoObject* MonoStateInstance = MonoInvoke<MonoObject*>("CopyStateForBackgroundThreadSave", NULL);
	return UStoryState::NewStoryState(MonoStateInstance);
}

////////////////////////////////////////////////////////
void UStory::BackgroundSaveComplete()
{
	MonoInvoke<void>("BackgroundSaveComplete", NULL);
}
