// Copyright Epic Games, Inc. All Rights Reserved.

#include "LTsFPSGameMode.h"
#include "LTsFPSHUD.h"
#include "LTsFPSCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALTsFPSGameMode::ALTsFPSGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ALTsFPSHUD::StaticClass();
	
}
