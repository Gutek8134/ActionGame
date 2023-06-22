// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActionGameGameMode.h"
#include "ActionGameCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "PlayerControllers/ActionGamePlayerController.h"

AActionGameGameMode::AActionGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PlayerControllerClass = AActionGamePlayerController::StaticClass();
}

void AActionGameGameMode::NotifyPlayerDied(AActionGamePlayerController* PlayerController)
{
	if (PlayerController) {
		PlayerController->RestartPlayerIn(2.f);
	}
}
