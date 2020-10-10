#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "InkVar.generated.h"

// A wrapper for passing around ink vars to and fro ink itself
// Not templated so it can be used in blueprints
UENUM(BlueprintType)
enum class EInkVarType : uint8
{
	Float,
	Int,
	String,
	None
};

// Interop struct for passing between c#. Don't edit order!
struct FInkVarInterop
{
	EInkVarType type;
	float floatVal;
	int32 intVal;
	const char* stringVal;
};

USTRUCT(BlueprintType)
struct FInkVar
{
	GENERATED_BODY()

	FInkVar() { type = EInkVarType::None; }

	FInkVar(float val) { type = EInkVarType::Float; floatVar = val; }
	FInkVar(int val) { type = EInkVarType::Int; intVar = val; }
	FInkVar(FString val) { type = EInkVarType::String; stringVar = val; }
	FInkVar(const FInkVarInterop& other);

	FInkVarInterop ToInterop() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ink")
	EInkVarType type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ink")
	float floatVar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ink")
	int intVar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ink")
	FString stringVar;
};

UCLASS()
class INK_API UInkVarLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (DisplayName = "String (Ink Var)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Ink")
	static FString Conv_InkVarString(const FInkVar& InkVar);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Int (Ink Var)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Ink")
	static int Conv_InkVarInt(const FInkVar& InkVar);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Float (Ink Var)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Ink")
	static float Conv_InkVarFloat(const FInkVar& InkVar);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Name (Ink Var)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Ink")
	static FName Conv_InkVarName(const FInkVar& InkVar);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Text (Ink Var)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Ink")
	static FText Conv_InkVarText(const FInkVar& InkVar);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bool (Ink Var)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Ink")
	static bool Conv_InkVarBool(const FInkVar& InkVar);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Ink Var (String)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Ink")
	static FInkVar Conv_StringInkVar(const FString& String);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Ink Var (Int)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Ink")
	static FInkVar Conv_IntInkVar(int Number);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Ink Var (Float)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Ink")
	static FInkVar Conv_FloatInkVar(float Number);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Ink Var (Text)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Ink")
	static FInkVar Conv_TextInkVar(const FText& Text);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Ink Var (Bool)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Ink")
	static FInkVar Conv_BoolInkVar(bool Boolean);
};