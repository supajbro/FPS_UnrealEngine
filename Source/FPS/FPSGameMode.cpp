// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"

AFPSGameMode::AFPSGameMode()
{
	// stub
}

void AFPSGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PC->bShowMouseCursor = false;
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}
}
