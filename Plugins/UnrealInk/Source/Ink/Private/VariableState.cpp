#include "VariablesState.h"

#include "Story.h"
#include "StoryState.h"
#include "MonoBaseClass.h"

#include <mono/jit/jit.h>
#include <mono/metadata/metadata.h>

UVariablesState::UVariablesState()
{

}

TArray<FString> UVariablesState::GetVariables()
{
	TArray<FString> variableNames;

	// Grab variable names
	MonoArray* result = MonoInvoke<MonoArray*>("GetVariables", nullptr);
	for (uintptr_t i = 0; i < mono_array_length(result); i++)
	{
		MonoString* variableName = mono_array_get(result, MonoString*, i);
		variableNames.Add(FString(mono_string_to_utf8(variableName)));
	}

	return variableNames;
}

FInkVar UVariablesState::GetVariable(const FString& variableName)
{
	// Single parameter: variable name
	void* params[1];
	params[0] = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*variableName));

	// Get result as a mono object
	MonoObject* result = MonoInvoke<MonoObject*>("GetVariable", params);
	MonoClass* pClass = mono_object_get_class(result);

	if (pClass == mono_get_single_class())
		return FInkVar(*(float*)mono_object_unbox(result));
	else if (pClass == mono_get_int32_class())
		return FInkVar(*(int*)mono_object_unbox(result));
	else if (pClass == mono_get_string_class())
		return FInkVar(FString(mono_string_to_utf8((MonoString*)result)));

	checkf(false, TEXT("Expected either float, int, or string returned from VariablesState::GetVariable"));
	return FInkVar(0);
}

void UVariablesState::SetVariableFloat(const FString& variableName, float value)
{
	void* params[2];
	params[0] = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*variableName));
	params[1] = mono_value_box(mono_domain_get(), mono_get_single_class(), &value);

	MonoInvoke<void>("SetVariable", params);
}

void UVariablesState::SetVariableInt(const FString& variableName, int value)
{
	void* params[2];
	params[0] = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*variableName));
	params[1] = mono_value_box(mono_domain_get(), mono_get_int32_class(), &value);

	MonoInvoke<void>("SetVariable", params);
}

void UVariablesState::SetVariableString(const FString& variableName, const FString& value)
{
	void* params[2];
	params[0] = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*variableName));
	params[1] = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*value));

	MonoInvoke<void>("SetVariable", params);
}

void UVariablesState::SetVariable(const FString& variableName, const FInkVar& value)
{
	if (!ensure(value.type != EInkVarType::None))
		return;

	switch (value.type)
	{
	case EInkVarType::Float:
		SetVariableFloat(variableName, value.floatVar);
		break;
	case EInkVarType::Int:
		SetVariableInt(variableName, value.intVar);
		break;
	case EInkVarType::String:
		SetVariableString(variableName, value.stringVar);
		break;
	}
}

UVariablesState* UVariablesState::NewVariablesState(MonoObject* MonoVariableState)
{
	UVariablesState* NewVariableState = NewObject<UVariablesState>();
	NewVariableState->NewFromInstance(MonoVariableState);
	NewVariableState->ManualMethodBind("GetVariable", 1);
	return NewVariableState;
}