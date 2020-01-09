// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DarkMessiahGameMode.h"
#include "DarkMessiahHUD.h"
#include "DarkMessiahCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADarkMessiahGameMode::ADarkMessiahGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ADarkMessiahHUD::StaticClass();
}
