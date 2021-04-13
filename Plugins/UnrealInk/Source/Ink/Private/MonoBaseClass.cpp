// Copyright 2018 David Colson. All Rights Reserved.

#include "MonoBaseClass.h"

#include "Ink.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#include <UObject/UnrealType.h>
#include <mono/jit/jit.h>


////////////////////////////////////////////////////////
UMonoBaseClass::UMonoBaseClass()
{
}

////////////////////////////////////////////////////////
void UMonoBaseClass::MonoNew(void** params, int nParams)
{
	FInkModule& Ink = FModuleManager::GetModuleChecked<FInkModule>("Ink");
	FString GlueClassName = FString("Glue") + GetClass()->GetName();
	Class = mono_class_from_name(Ink.GetInkAssemblyImage(), "InkGlue", TCHAR_TO_UTF8(*GlueClassName));

	//Create a instance of the class
	Instance = mono_object_new(Ink.GetMonoDomain(), Class);

	// call the constructor
	MonoMethod* constructorMethod = mono_class_get_method_from_name(Class, ".ctor", nParams);
	MonoObject* exception{ nullptr };
	mono_runtime_invoke(constructorMethod, Instance, params, &exception);
	Ink.MaybeThrowMonoException(exception);

	FindMethods();
}

////////////////////////////////////////////////////////
void UMonoBaseClass::NewFromInstance(MonoObject* MonoInstance)
{
	FInkModule& Ink = FModuleManager::GetModuleChecked<FInkModule>("Ink");
	FString GlueClassName = FString("Glue") + GetClass()->GetName();
	Class = mono_class_from_name(Ink.GetInkAssemblyImage(), "InkGlue", TCHAR_TO_UTF8(*GlueClassName));
	Instance = MonoInstance;

	FindMethods();
}

////////////////////////////////////////////////////////
void UMonoBaseClass::FindMethods()
{
	for (TFieldIterator<UFunction> FunctionIt(GetClass(), EFieldIteratorFlags::IncludeSuper); FunctionIt; ++FunctionIt)
	{
		FString MethodName = FunctionIt->GetFName().ToString();

		int numParams = FunctionIt->NumParms;
		if (FunctionIt->GetReturnProperty())
			numParams--;

		MonoMethod* Method = mono_class_get_method_from_name(Class, TCHAR_TO_UTF8(*MethodName), numParams);

		if (Method)
			Methods.Add(MethodName, Method);
	}
}

void UMonoBaseClass::ManualMethodBind(FString MethodName, int numParams)
{
	MonoMethod* Method = mono_class_get_method_from_name(Class, TCHAR_TO_UTF8(*MethodName), numParams);
	if (Method)
		Methods.Add(MethodName, Method);
}
