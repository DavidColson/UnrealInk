#include "InkVar.h"

#include "Misc/AssertionMacros.h"
#include "Logging/TokenizedMessage.h"
#include "Logging/MessageLog.h"

#define LOCTEXT_NAMESPACE "UnrealInk"

FInkVar::FInkVar(const FInkVarInterop& other)
	: type(other.type),
	floatVar(other.floatVal),
	intVar(other.intVal),
	stringVar(other.stringVal)
{
}

FInkVarInterop FInkVar::ToInterop() const
{
	FInkVarInterop result = { type, floatVar, intVar, nullptr };
	if (type == EInkVarType::String)
	{
		UE_LOG(LogInk, Warning, TEXT("Sending strings to C# via InkVar not currently supported! It requires special memory management that can easily cause memory leaks"));
	}
	return result;
}

FText InkVarTypeToText(EInkVarType Type)
{
	FText Result;
	switch (Type)
	{
	case EInkVarType::Float: return LOCTEXT("InkVarFloatType", "Float"); break;
	case EInkVarType::Int: return LOCTEXT("InkVarIntType", "Int"); break;
	case EInkVarType::String: return LOCTEXT("InkVarStringType", "String"); break;
	case EInkVarType::None:  return LOCTEXT("InkVarNoneType", "None"); break;
	}
	return FText();
}

void ThrowException(EInkVarType From, EInkVarType To)
{
	TSharedRef<FTokenizedMessage> Message = FTokenizedMessage::Create(EMessageSeverity::Error);
	Message->AddToken(FTextToken::Create(FText::Format(LOCTEXT("InkVarConversionMessageFmt", "Ink Var Conversion Error: Attempting to convert ink var of type \"{0}\" to type \"{1}\"."), InkVarTypeToText(From), InkVarTypeToText(To))));
	FMessageLog("PIE").AddMessage(Message);
}

FString UInkVarLibrary::Conv_InkVarString(const FInkVar& InkVar)
{
	if (InkVar.type != EInkVarType::String)
	{
		ThrowException(InkVar.type, EInkVarType::String);
		return FString(TEXT(""));
	}
	return InkVar.stringVar;
}

int UInkVarLibrary::Conv_InkVarInt(const FInkVar& InkVar)
{
	if (InkVar.type != EInkVarType::Int)
	{
		ThrowException(InkVar.type, EInkVarType::Int);
		return 0;
	}
	return InkVar.intVar;
}

float UInkVarLibrary::Conv_InkVarFloat(const FInkVar& InkVar)
{
	if (InkVar.type != EInkVarType::Float)
	{
		ThrowException(InkVar.type, EInkVarType::Float);
		return 0.f;
	}
	return InkVar.floatVar;
}

FName UInkVarLibrary::Conv_InkVarName(const FInkVar& InkVar)
{
	if (InkVar.type != EInkVarType::String)
	{
		ThrowException(InkVar.type, EInkVarType::String);
		return NAME_None;
	}
	return FName(*InkVar.stringVar);
}

FText UInkVarLibrary::Conv_InkVarText(const FInkVar& InkVar)
{
	if (InkVar.type != EInkVarType::String)
	{
		ThrowException(InkVar.type, EInkVarType::String);
		return FText::GetEmpty();
	}
	return FText::FromString(InkVar.stringVar);
}

bool UInkVarLibrary::Conv_InkVarBool(const FInkVar& InkVar)
{
	if (InkVar.type != EInkVarType::Int)
	{
		ThrowException(InkVar.type, EInkVarType::Int);
		return false;
	}
	return InkVar.intVar > 0;
}

FInkVar UInkVarLibrary::Conv_StringInkVar(const FString& String)
{
	return FInkVar(String);
}

FInkVar UInkVarLibrary::Conv_IntInkVar(int Number)
{
	return FInkVar(Number);
}

FInkVar UInkVarLibrary::Conv_FloatInkVar(float Number)
{
	return FInkVar(Number);
}

FInkVar UInkVarLibrary::Conv_TextInkVar(const FText& Text)
{
	return FInkVar(Text.ToString());
}

FInkVar UInkVarLibrary::Conv_BoolInkVar(bool Boolean)
{
	return FInkVar(Boolean ? 1 : 0);
}