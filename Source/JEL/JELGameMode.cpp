// Copyright Epic Games, Inc. All Rights Reserved.

#include "JELGameMode.h"
#include "JELCharacter.h"
#include "UObject/ConstructorHelpers.h"

AJELGameMode::AJELGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/SideScrollerCPP/Blueprints/SideScrollerCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		//DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
