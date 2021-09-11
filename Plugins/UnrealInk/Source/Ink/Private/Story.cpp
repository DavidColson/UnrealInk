// Copyright 2018 David Colson. All Rights Reserved.

#include "Story.h"

#include "StoryAsset.h"
#include "StoryState.h"
#include "Choice.h"
#include "Ink.h"
#include "VariablesState.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#include <mono/jit/jit.h>

int UStory::InstanceCounter = 0;
TMap<TPair<int, FString>, TArray<FVariableObserver>> UStory::VarObserverMap({});
TMap<TPair<int, FString>, FExternalFunctionHandler> UStory::ExternalFuncMap({});


////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) void ObserverCallback(int InstanceId, const char* VariableName, FInkVarInterop* NewValue)
{
	for (auto& _delegate : UStory::VarObserverMap[UStory::FDelegateMapKey(InstanceId, FString(VariableName))])
	{
		_delegate.ExecuteIfBound(FString(VariableName), FInkVar(*NewValue));
	}
}

////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) FInkVarInterop ExternalFunctionCallback(int32 InstanceId, const char* FunctionName, uint32 NumArgs, FInkVarInterop * pArgs)
{
	// Create argument array
	TArray<FInkVar> Arguments;
	for (uint32 i = 0; i < NumArgs; i++) {
		Arguments.Add(FInkVar(pArgs[i]));
	}

	// Get the handler for this function from the story map
	const FExternalFunctionHandler& handler = UStory::ExternalFuncMap[UStory::FDelegateMapKey(InstanceId, FString(FunctionName))];

	// Call handler
	FInkVar Result = handler.Execute(FString(FunctionName), Arguments);

	// Return result
	return Result.ToInterop();
}

////////////////////////////////////////////////////////
UStory::UStory()
{
}

////////////////////////////////////////////////////////
UStory::~UStory()
{
	TArray<FDelegateMapKey> KeysToRemove;
	for (auto& Element : VarObserverMap)
	{
		if (Element.Key.Key == InstanceId)
		{
			KeysToRemove.Add(Element.Key);
			break;
		}
	}

	for (auto& element : ExternalFuncMap)
	{
		if (element.Key.Key == InstanceId)
		{
			ExternalFuncMap.Remove(element.Key);
			break;
		}
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

	NewStory->InstanceId = InstanceCounter++;

	MonoString* JsonString = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*(StoryAsset->CompiledStory)));
	void* Args[2];
	Args[0] = JsonString;
	Args[1] = &(NewStory->InstanceId);

	NewStory->MonoNew(Args, 2);

	// Manually added methods (have unique parameters)
	NewStory->ManualMethodBind("ObserveVariable", 1);
	NewStory->ManualMethodBind("RemoveVariableObserver", 1);
	NewStory->ManualMethodBind("EvaluateFunction", 3);
	NewStory->ManualMethodBind("RegisterFunction", 1);

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
	TArray<UChoice*> ReturnChoices;

	MonoArray* Result = MonoInvoke<MonoArray*>("CurrentChoices", NULL);
	for (size_t i = 0; i < mono_array_length(Result); i++)
	{
		MonoObject* MonoChoiceInstance = mono_array_get(Result, MonoObject*, i);
		ReturnChoices.Add(UChoice::NewChoice(MonoChoiceInstance));
	}

	return ReturnChoices;
}

////////////////////////////////////////////////////////
void UStory::ChooseChoiceIndex(int Index)
{
	void* Args[1];
	Args[0] = &Index;
	MonoInvoke<void>("ChooseChoiceIndex", Args);
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
	TArray<FString>  ReturnTags;
	MonoArray* MonoTags = MonoInvoke<MonoArray*>("CurrentTags", NULL);
	for (size_t i = 0; i < mono_array_length(MonoTags); i++)
	{
		MonoString* String = mono_array_get(MonoTags, MonoString*, i);
		ReturnTags.Add(FString(mono_string_to_utf8(String)));
	}
	return ReturnTags;
}

////////////////////////////////////////////////////////
TArray<FString> UStory::GlobalTags()
{
	TArray<FString>  ReturnTags;
	MonoArray* MonoTags = MonoInvoke<MonoArray*>("GlobalTags", NULL);
	for (size_t i = 0; i < mono_array_length(MonoTags); i++)
	{
		MonoString* String = mono_array_get(MonoTags, MonoString*, i);
		ReturnTags.Add(FString(mono_string_to_utf8(String)));
	}
	return ReturnTags;
}

////////////////////////////////////////////////////////
TArray<FString> UStory::CurrentErrors()
{
	TArray<FString>  ReturnErrors;
	MonoArray* MonoErrors = MonoInvoke<MonoArray*>("CurrentErrors", NULL);
	for (size_t i = 0; i < mono_array_length(MonoErrors); i++)
	{
		MonoString* String = mono_array_get(MonoErrors, MonoString*, i);
		ReturnErrors.Add(FString(mono_string_to_utf8(String)));
	}
	return ReturnErrors;
}

////////////////////////////////////////////////////////
TArray<FString> UStory::CurrentWarnings()
{
	TArray<FString>  ReturnWarnings;
	MonoArray* MonoWarnings = MonoInvoke<MonoArray*>("CurrentWarnings", NULL);
	for (size_t i = 0; i < mono_array_length(MonoWarnings); i++)
	{
		MonoString* String = mono_array_get(MonoWarnings, MonoString*, i);
		ReturnWarnings.Add(FString(mono_string_to_utf8(String)));
	}
	return ReturnWarnings;
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
void UStory::ContinueAsync(float MillisecondLimitAsync)
{
	void* Args[1];
	Args[0] = &MillisecondLimitAsync;
	MonoInvoke<void>("ContinueAsync", Args);
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
	TArray<FString>  ReturnTags;

	MonoString* MonoPath = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*Path));
	void* Args[1];
	Args[0] = MonoPath;
	MonoArray* MonoTags = MonoInvoke<MonoArray*>("TagsForContentAtPath", Args);
	for (size_t i = 0; i < mono_array_length(MonoTags); i++)
	{
		MonoString* String = mono_array_get(MonoTags, MonoString*, i);
		ReturnTags.Add(FString(mono_string_to_utf8(String)));
	}
	return ReturnTags;
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
void UStory::ChoosePathString(FString Path, bool ResetCallstack, TArray<FInkVar> Vars)
{
	MonoArray* ArgsArray;
	ArgsArray = mono_array_new(mono_domain_get(), mono_get_object_class(), Vars.Num());

	for (int i = 0; i < Vars.Num(); i++)
	{
		FInkVar& Var = Vars[i];

		MonoObject* BoxedParam = nullptr;

		switch (Var.type)
		{
		case EInkVarType::Float:
			BoxedParam = mono_value_box(mono_domain_get(), mono_get_single_class(), &Var.floatVar);
			break;
		case EInkVarType::Int:
			BoxedParam = mono_value_box(mono_domain_get(), mono_get_int32_class(), &Var.intVar);
			break;
		case EInkVarType::String:
			BoxedParam = (MonoObject*)mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*Var.stringVar));
			//BoxedParam = mono_value_box(mono_domain_get(), mono_get_string_class(), monoString);
			break;
		}

		mono_array_set(ArgsArray, MonoObject*, i, BoxedParam);
	}

	void* Params[3];
	Params[0] = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*Path));
	Params[1] = ArgsArray;
	Params[2] = &ResetCallstack;

	MonoInvoke<void>("ChoosePathString", Params);
}

////////////////////////////////////////////////////////
class UVariablesState* UStory::VariablesState()
{
	MonoObject* MonoVariablesStateInstance = MonoInvoke<MonoObject*>("VariablesState", nullptr);
	return UVariablesState::NewVariablesState(MonoVariablesStateInstance);
}

////////////////////////////////////////////////////////
void UStory::ObserveVariable(FString VariableName, const FVariableObserver & Observer)
{
	FDelegateMapKey Key = FDelegateMapKey(InstanceId, VariableName);

	// If a delegate as already been bound to this variable, add it to the list of delegates for that key, and don't bother telling C# about it
	if (VarObserverMap.Contains(Key))
	{
		VarObserverMap[Key].Add(Observer);
	}
	else
	{
		TArray<FVariableObserver> Observers;
		Observers.Add(Observer);
		VarObserverMap.Add(Key, Observers);

		void* Params[1];
		Params[0] = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*VariableName));
		MonoInvoke<void>("ObserveVariable", Params);
	}
}

////////////////////////////////////////////////////////
void UStory::ObserveVariables(TArray<FString> VariableNames, const FVariableObserver & Observer)
{
	for (auto& VariableName : VariableNames)
	{
		ObserveVariable(VariableName, Observer);
	}
}

////////////////////////////////////////////////////////
void UStory::RemoveVariableObserver(const FVariableObserver& Observer, FString SpecificVariableName /*= ""*/)
{
	// Case 1, variable specified, delegate specified, Ubind this variable from this delegate
	if (SpecificVariableName != "")
	{
		FDelegateMapKey key = FDelegateMapKey(InstanceId, SpecificVariableName);
		TArray<FVariableObserver>& Delegates = VarObserverMap[key];
		Delegates.Remove(Observer);

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
		for (auto& Element : VarObserverMap)
		{
			TArray<FVariableObserver>& Delegates = Element.Value;
			Delegates.Remove(Observer);

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
bool UStory::HasFunction(FString FunctionName)
{
	void* Args[1];
	Args[0] = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*FunctionName));;
	return MonoInvoke<bool>("HasFunction", Args);
}

////////////////////////////////////////////////////////
FInkVar UStory::EvaluateFunction(FString FunctionName, TArray<FInkVar> Arguments)
{
	FString Discard;
	return EvaluateFunctionOutString(FunctionName, Discard, Arguments);
}

////////////////////////////////////////////////////////
FInkVar UStory::EvaluateFunctionOutString(FString FunctionName, FString& OutString, TArray<FInkVar> Arguments)
{
	MonoArray* ArgsArray;
	ArgsArray = mono_array_new(mono_domain_get(), mono_get_object_class(), Arguments.Num());

	for (int i = 0; i < Arguments.Num(); i++)
	{
		FInkVar& Argument = Arguments[i];
		MonoObject* BoxedParam = nullptr;
		switch (Argument.type)
		{
		case EInkVarType::Float:
			BoxedParam = mono_value_box(mono_domain_get(), mono_get_single_class(), &Argument.floatVar);
			break;
		case EInkVarType::Int:
			BoxedParam = mono_value_box(mono_domain_get(), mono_get_int32_class(), &Argument.intVar);
			break;
		case EInkVarType::String:
			BoxedParam = (MonoObject*)mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*Argument.stringVar));
			break;
		}
		mono_array_set(ArgsArray, MonoObject*, i, BoxedParam);
	}

	MonoString* OutTextString = mono_string_new(mono_domain_get(), "");

	void* Params[3];
	Params[0] = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*FunctionName));
	Params[1] = &OutTextString;
	Params[2] = ArgsArray;

	MonoObject* Result = MonoInvoke<MonoObject*>("EvaluateFunction", Params);

	OutString = FString(mono_string_to_utf8(OutTextString));

	MonoClass* pClass = mono_object_get_class(Result);
	if (pClass == mono_get_single_class())
		return FInkVar(*(float*)mono_object_unbox(Result));
	else if (pClass == mono_get_int32_class())
		return FInkVar(*(int*)mono_object_unbox(Result));
	else if (pClass == mono_get_string_class())
		return FInkVar(FString(mono_string_to_utf8((MonoString*)Result)));

	return FInkVar();
}

////////////////////////////////////////////////////////
void UStory::RegisterExternalFunction(FString FunctionName, const FExternalFunctionHandler& function)
{
	// Create Mono string
	MonoString* MonoName = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*FunctionName));
	void* Args[1];
	Args[0] = MonoName;

	// Invoke mono method
	MonoInvoke<void>("RegisterFunction", Args);

	// Register in map
	FDelegateMapKey key = FDelegateMapKey(InstanceId, FunctionName);
	ExternalFuncMap.Add(key, function);
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
