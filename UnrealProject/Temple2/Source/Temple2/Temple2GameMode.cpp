// Copyright Epic Games, Inc. All Rights Reserved.

#include "Temple2GameMode.h"
#include "Temple2PlayerController.h"
#include "Temple2Character.h"
#include "UObject/ConstructorHelpers.h"

ATemple2GameMode::ATemple2GameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ATemple2PlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}