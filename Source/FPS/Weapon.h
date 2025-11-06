// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bullet.h"
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

	void Shoot();

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<class ABullet> BulletClass;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	UStaticMeshComponent* SpawnLocation;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	FRotator SpawnRotation;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	int AmmoCount;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float FireRate;

	UPROPERTY(VisibleAnywhere)
	bool bReloading = false;

};
