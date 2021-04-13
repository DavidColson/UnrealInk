// Copyright 2018 David Colson. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "UObject/Object.h"
#include <mono/jit/jit.h>

#include "Ink.h"

#include "MonoBaseClass.generated.h"


typedef struct _MonoArray MonoArray;
typedef struct _MonoObject MonoObject;
typedef struct _MonoClass MonoClass;
typedef struct _MonoMethod MonoMethod;

class UObject;

UCLASS()
class INK_API UMonoBaseClass : public UObject
{
	GENERATED_BODY()

public:
	UMonoBaseClass();

protected:
	// Creates a new instance of this class in the mono domain ready for use
	void MonoNew(void** params, int nParams);

	// Creates the C++ representation of an object that's already been created in the mono domain
	void NewFromInstance(MonoObject* MonoInstance);

	// TODO Convert to use Variadics
	// Function invocation methods
	template<typename type>
	inline type MonoInvoke(FString MethodName, void** params)
	{
		FInkModule& Ink = FModuleManager::GetModuleChecked<FInkModule>("Ink");

		MonoObject* Exception;
		MonoObject* ret = mono_runtime_invoke(Methods[MethodName], Instance, params, &Exception);
		Ink.MaybeThrowMonoException(Exception);

		return *(type*)mono_object_unbox(ret);
	}

	template<>
	inline void MonoInvoke<void>(FString MethodName, void** params)
	{
		FInkModule& Ink = FModuleManager::GetModuleChecked<FInkModule>("Ink");

		MonoObject* Exception;
		mono_runtime_invoke(Methods[MethodName], Instance, params, &Exception);
		Ink.MaybeThrowMonoException(Exception);
	}

	template <>
	inline MonoObject* MonoInvoke<MonoObject*>(FString MethodName, void** params)
	{
		FInkModule& Ink = FModuleManager::GetModuleChecked<FInkModule>("Ink");

		MonoObject* Exception;
		MonoObject* ret = mono_runtime_invoke(Methods[MethodName], Instance, params, &Exception);
		Ink.MaybeThrowMonoException(Exception);
		return ret;
	}

	template <>
	inline MonoArray* MonoInvoke<MonoArray*>(FString MethodName, void** params)
	{
		FInkModule& Ink = FModuleManager::GetModuleChecked<FInkModule>("Ink");

		MonoObject* Exception;
		MonoArray* ret = (MonoArray*)mono_runtime_invoke(Methods[MethodName], Instance, params, &Exception);
		Ink.MaybeThrowMonoException(Exception);
		return ret;
	}

	template <>
	inline FString MonoInvoke<FString>(FString MethodName, void** params)
	{
		FInkModule& Ink = FModuleManager::GetModuleChecked<FInkModule>("Ink");

		MonoObject* Exception;
		MonoString* ret = (MonoString*)mono_runtime_invoke(Methods[MethodName], Instance, params, &Exception);
		Ink.MaybeThrowMonoException(Exception);

		return FString(mono_string_to_utf16(ret));
	}


protected:
	void FindMethods();
	void ManualMethodBind(FString MethodName, int numParams);

	TMap<FString, MonoMethod*> Methods;

	MonoClass* Class{ nullptr };
	MonoObject* Instance{ nullptr };
};
