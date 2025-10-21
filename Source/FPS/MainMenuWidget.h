// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/GridPanel.h"
#include "Components/Button.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Game")
		void PlayLevel(FName LevelName);

	UFUNCTION(BlueprintCallable, Category = "Game")
		void QuitGame();

	UPROPERTY(meta = (BindWidget))
	UGridPanel* LevelGrid;

	UFUNCTION(BlueprintCallable)
	void CreateLevelButtons(const TArray<FName>& Levels);

	UFUNCTION()
	void OnLevelButtonClicked();

	UPROPERTY()
	FName ClickedLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> LevelButtonClass;
	
};
