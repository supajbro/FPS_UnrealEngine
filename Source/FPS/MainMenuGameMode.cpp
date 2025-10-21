// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuGameMode.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	DefaultPawnClass = nullptr; // Do not spawn any pawn
}

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (MainMenuWidgetClass)
	{
		//UUserWidget* Menu = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
		UMainMenuWidget* Menu = Cast<UMainMenuWidget>(CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass));
		if (Menu)
		{
			Menu->AddToViewport();

			Menu->CreateLevelButtons(Levels);

			APlayerController* PC = GetWorld()->GetFirstPlayerController();
			if (PC)
			{
				PC->bShowMouseCursor = true;
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(Menu->TakeWidget());
				PC->SetInputMode(InputMode);
			}
		}
	}
}
