#include "InkVar.h"

#include "Misc/AssertionMacros.h"

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
		UE_LOG(LogInk, Warning, TEXT("Sending strings to C# via InkVar not currently supported!"));
	}
	return result;
}

void ThrowException()
{

	TSharedRef<FTokenizedMessage> Message = FTokenizedMessage::Create(EMessageSeverity::Error);
	Message->AddToken(FTextToken::Create(FText::Format(LOCTEXT("RuntimeErrorMessageFmt", "Blueprint Runtime Error: \"{0}\"."), FText::FromString("Casting incorrect ink var"))));
	FMessageLog("PIE").AddMessage(Message);
}

// TODO: Change this macros to errors that don't crash the editor. Give back errors to the user nicely
FString UInkVarLibrary::Conv_InkVarString(const FInkVar& InkVar)
{
	if (InkVar.type != EInkVarType::String)
	{
		ThrowException();
		return FString(TEXT(""));
	}
	return InkVar.stringVar;
}

int UInkVarLibrary::Conv_InkVarInt(const FInkVar& InkVar)
{
	if (InkVar.type != EInkVarType::Int)
	{
		ThrowException();
		return 0;
	}
	return InkVar.intVar;
}

float UInkVarLibrary::Conv_InkVarFloat(const FInkVar& InkVar)
{
	if (InkVar.type != EInkVarType::Float)
	{
		ThrowException();
		return 0.f;
	}
	return InkVar.floatVar;
}

FName UInkVarLibrary::Conv_InkVarName(const FInkVar& InkVar)
{
	if (InkVar.type != EInkVarType::String)
	{
		ThrowException();
		return NAME_None;
	}
	return FName(*InkVar.stringVar);
}

FText UInkVarLibrary::Conv_InkVarText(const FInkVar& InkVar)
{
	if (InkVar.type != EInkVarType::String)
	{
		ThrowException();
		return FText::GetEmpty();
	}
	return FText::FromString(InkVar.stringVar);
}

bool UInkVarLibrary::Conv_InkVarBool(const FInkVar& InkVar)
{
	if (InkVar.type != EInkVarType::String)
	{
		ThrowException();
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