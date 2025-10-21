// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSCharacter.h"
#include "Components/BoxComponent.h"
#include "Cannon.generated.h"

class UBoxComponent;

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
	float UpwardBoost = 300.f;

	UPROPERTY(EditAnywhere, Category = "Cannon")
	float ShootDuration = 1.f;

	UPROPERTY(EditAnywhere, Category = "Cannon")
	FVector ShootDirection;

	UPROPERTY(VisibleAnywhere, Category = "Cannon")
	UBoxComponent* StartLocation;

	UPROPERTY(VisibleAnywhere, Category = "Cannon")
	UBoxComponent* ShootLocation;

	UPROPERTY(VisibleAnywhere, Category = "Cannon")
	UStaticMeshComponent* ShootLocationMesh;

	UPROPERTY(VisibleAnywhere, Category = "Cannon")
	UStaticMeshComponent* StartLocationMesh;

};
