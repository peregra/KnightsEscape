// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "KnightsEscapeGameMode.h"
#include "KnightsEscapeCharacter.h"
#include "UObject/ConstructorHelpers.h"

AKnightsEscapeGameMode::AKnightsEscapeGameMode()
{
	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}
}
