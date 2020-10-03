// Copyright 2018 David Colson. All Rights Reserved.

#include "Choice.h"

#include "Ink.h"

#include <mono/jit/jit.h>


////////////////////////////////////////////////////////
UChoice::UChoice()
{

}

////////////////////////////////////////////////////////
UChoice* UChoice::NewChoice(MonoObject* MonoChoice)
{
	UChoice* NewChoice = NewObject<UChoice>();
	NewChoice->NewFromInstance(MonoChoice);
	return NewChoice;
}

////////////////////////////////////////////////////////
FString UChoice::Text()
{
	return MonoInvoke<FString>("Text", NULL);
}

////////////////////////////////////////////////////////
FString UChoice::PathStringOnChoice()
{
	return MonoInvoke<FString>("PathStringOnChoice", NULL);
}

////////////////////////////////////////////////////////
FString UChoice::SourcePath()
{
	return MonoInvoke<FString>("SourcePath", NULL);
}

////////////////////////////////////////////////////////
int UChoice::Index()
{
	return MonoInvoke<int>("Index", NULL);
}