// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UMainMenuWidget::PlayLevel(FName LevelName)
{
	UGameplayStatics::OpenLevel(this, LevelName);
}

void UMainMenuWidget::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}

void UMainMenuWidget::CreateLevelButtons(const TArray<FName>& Levels)
{
    if (!LevelGrid) return;

    LevelGrid->ClearChildren();

    int32 Row = 0, Column = 0;
    const int32 MaxColumns = 3;

    for (const FName& LevelName : Levels)
    {
        UButton* NewButton = NewObject<UButton>(this);
        if (!NewButton) continue;

        UTextBlock* ButtonText = NewObject<UTextBlock>(NewButton);
        ButtonText->SetText(FText::FromName(LevelName));
        NewButton->AddChild(ButtonText);

        // Set the level name to a property
        ClickedLevelName = LevelName;
        NewButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnLevelButtonClicked);

        LevelGrid->AddChildToGrid(NewButton, Row, Column);
        Column++;
        if (Column >= MaxColumns)
        {
            Column = 0;
            Row++;
        }
    }
}

void UMainMenuWidget::OnLevelButtonClicked()
{
    UGameplayStatics::OpenLevel(this, ClickedLevelName);
}
