// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSCharacter.h"
#include "Cannon.generated.h"

UCLASS()
class FPS_API ACannon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACannon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void ShootPlayer(AFPSCharacter* Player);

protected:
	UPROPERTY(EditAnywhere, Category = "Cannon")
	float ShootPower = 100.f;

	UPROPERTY(EditAnywhere, Category = "Cannon")
	float ShootDuration = 1.f;

	UPROPERTY(EditAnywhere, Category = "Cannon")
	FVector ShootDirection;

};
