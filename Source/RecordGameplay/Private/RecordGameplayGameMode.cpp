// Copyright Epic Games, Inc. All Rights Reserved.

#include "RecordGameplayGameMode.h"
#include "RecordGameplayHUD.h"
#include "RecordGameplayCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARecordGameplayGameMode::ARecordGameplayGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ARecordGameplayHUD::StaticClass();
}
