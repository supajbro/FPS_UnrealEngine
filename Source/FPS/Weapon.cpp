// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SpawnLocation = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spawn Location"));
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

USkeletalMeshComponent* AWeapon::GetSkeletalMesh()
{
	return WeaponMesh;
}

void AWeapon::Shoot()
{
    if (!BulletClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("BulletClass is null!"));
        return;
    }

    if (!SpawnLocation)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnLocation is null!"));
        return;
    }

    FVector SpawnLoc = SpawnLocation->GetComponentLocation();
    FRotator SpawnRot = SpawnLocation->GetComponentRotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    ABullet* SpawnedBullet = GetWorld()->SpawnActor<ABullet>(BulletClass, SpawnLoc, SpawnRot, SpawnParams);
    if (!SpawnedBullet)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnActor failed!"));
    }
}

