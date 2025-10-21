// Fill out your copyright notice in the Description page of Project Settings.

#include "Cannon.h"

// Sets default values
ACannon::ACannon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Spawn shoot location
	ShootLocation = CreateDefaultSubobject<UBoxComponent>(TEXT("ShootLocation"));
	ShootLocation->SetupAttachment(RootComponent);

	// Start location
	ShootLocationMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShootLocationMesh"));
	ShootLocationMesh->SetupAttachment(ShootLocation);

	// Spawn mesh for shoot location
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShootCubeMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (ShootCubeMeshAsset.Succeeded())
	{
		ShootLocationMesh->SetStaticMesh(ShootCubeMeshAsset.Object);
		ShootLocationMesh->SetRelativeScale3D(FVector(.5f));
	}

	// Start location
	StartLocationMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StartLocationMesh"));
	StartLocationMesh->SetupAttachment(StartLocation);
	StartLocationMesh->ComponentTags.Add(FName("Cannon"));

	// Spawn mesh for start location
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StartCubeMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (StartCubeMeshAsset.Succeeded())
	{
		StartLocationMesh->SetStaticMesh(StartCubeMeshAsset.Object);
		StartLocationMesh->SetRelativeScale3D(FVector(1.f));
	}
}

// Called when the game starts or when spawned
void ACannon::BeginPlay()
{
	Super::BeginPlay();
	
	if (ShootLocation)
	{
		ShootDirection = ShootLocation->GetComponentLocation();
	}
}

// Called every frame
void ACannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACannon::ShootPlayer(AFPSCharacter* Player)
{
	Player->StartShoot(ShootPower, UpwardBoost, ShootDuration, ShootDirection);
}

