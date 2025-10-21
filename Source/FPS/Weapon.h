// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class FPS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	USkeletalMeshComponent* WeaponMesh;

	USkeletalMeshComponent* GetSkeletalMesh();

	UPROPERTY(EditAnywhere, Category = "Values")
	int AmmoCount;

	UPROPERTY(EditAnywhere, Category = "Values")
	float FireRate;

	UPROPERTY(VisibleAnywhere)
	bool bReloading = false;

};
