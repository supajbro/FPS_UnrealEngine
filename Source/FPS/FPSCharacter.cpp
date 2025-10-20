// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Cannon.h"
#include "FPS.h"

AFPSCharacter::AFPSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
}

void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// When on ground check to make sure bools have been reset
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		bHasDoubleJumped = false;
		bHasDashed = false;
	}

	DebugFunc();
	KoyoteJump(DeltaTime);
	FallingGravity(DeltaTime);
	CheckForWall(DeltaTime);
	CheckForInteraction(DeltaTime);
}

void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AFPSCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFPSCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPSCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPSCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AFPSCharacter::LookInput);

		// Dash
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AFPSCharacter::StartDash);

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AFPSCharacter::InteractInput);
	}
	else
	{
		UE_LOG(LogFPS, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AFPSCharacter::Jump()
{
	UCharacterMovementComponent* Character = GetCharacterMovement();

	// Jump off of Wall Run
	if (bIsWallRunning)
	{
		FVector Forward = FirstPersonCameraComponent->GetForwardVector();
		Forward.Z = 0.0f;
		Forward.Normalize();

		// Make sure the jump is not into the wall
		float Dot = FVector::DotProduct(Forward, CurrentWallNormal);
		//UE_LOG(LogTemp, Warning, TEXT("Wall Run Dot: %f"), Dot);
		if (Dot < 0.f)
		{
			Forward = FVector::VectorPlaneProject(Forward, CurrentWallNormal * 15.f).GetSafeNormal();
		}

		FVector JumpVelocity = Forward * WallRunSpeed;
		JumpVelocity.Z = Character->JumpZVelocity;

		LaunchCharacter(JumpVelocity, true, true);
		PreviousDirection = GetActorForwardVector();
		bHasDoubleJumped = false;
		StopWallRun();
		//UE_LOG(LogTemp, Warning, TEXT("Wall Run Jump"));
		return;
	}

	bool bCanKoyoteJump = !Character->IsMovingOnGround() && TimeSinceLeftGround <= KoyoteTime;
	bool bCanNormalJump = Character->IsMovingOnGround();

	if (bCanNormalJump)
	{
		Super::Jump();
		PreviousDirection = GetActorForwardVector();
		bHasDoubleJumped = false;
		//UE_LOG(LogTemp, Warning, TEXT("Normal Jump"));
		return;
	}

	if (bCanKoyoteJump)
	{
		LaunchCharacter(FVector(0, 0, Character->JumpZVelocity), false, true);
		PreviousDirection = GetActorForwardVector();
		bHasDoubleJumped = false;
		//UE_LOG(LogTemp, Warning, TEXT("Koyote Jump"));
		return;
	}

	// Double jump
	if (!bHasDoubleJumped && Character->IsFalling())
	{
		FVector Forward = FirstPersonCameraComponent->GetForwardVector();
		Forward.Z = 0.0f;
		Forward.Normalize();

		FVector JumpVelocity = Forward * DoubleJumpForwardBoost + FVector(0, 0, Character->JumpZVelocity);
		LaunchCharacter(JumpVelocity, true, true);

		bHasDoubleJumped = true;
	}
}

void AFPSCharacter::DebugFunc()
{
	float DebugDistance = 500.f; // How far the line goes
	FVector Start = GetActorLocation();
	FVector End = Start + (GetActorForwardVector() * DebugDistance);

	DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, false, 0.f, 0, 1.0f);
}

void AFPSCharacter::FallingGravity(float DeltaTime)
{
	bool bIsFalling = GetCharacterMovement()->IsFalling();

	if (!bIsFalling)
	{
		FallGravityMultiplier = FallGravityMultiplierMin;
		return;
	}

	FallGravityMultiplier = (FallGravityMultiplier < FallGravityMultiplierMax) ? FallGravityMultiplier + DeltaTime * FallGravityScaler : FallGravityMultiplierMax;
	FVector tempVel = GetCharacterMovement()->Velocity;
	tempVel.Z -= FallGravityMultiplier * DeltaTime;
	GetCharacterMovement()->Velocity = tempVel;
}

void AFPSCharacter::KoyoteJump(float DeltaTime)
{
	bool bIsGrounded = GetCharacterMovement()->IsMovingOnGround();
	if (bIsGrounded) {
		TimeSinceLeftGround = 0.f;
	}
	else {
		TimeSinceLeftGround += DeltaTime;
	}
}

void AFPSCharacter::CheckForWall(float DeltaTime)
{
	WallConnectTimer += DeltaTime;

	if (WallConnectTimer < .1f)
	{
		return;
	}

	FVector Start = GetActorLocation();
	FVector RightVector = GetActorRightVector();
	FVector EndRight = Start + (RightVector * WallCheckDistance);
	FVector EndLeft = Start - (RightVector * WallCheckDistance);

	FHitResult RightHit;
	FHitResult LeftHit;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHitRight = GetWorld()->LineTraceSingleByChannel(
		RightHit,
		Start,
		EndRight,
		ECC_Visibility,
		Params
	);

	bool bHitLeft = GetWorld()->LineTraceSingleByChannel(
		LeftHit,
		Start,
		EndLeft,
		ECC_Visibility,
		Params
	);
	
	// Debug lines
	DrawDebugLine(GetWorld(), Start, EndRight, FColor::Blue, false, 0.f, 0, 1.0f);
	DrawDebugLine(GetWorld(), Start, EndLeft, FColor::Red, false, 0.f, 0, 1.0f);

	auto IsValidWall = [](AActor* HitActor)
		{
			return HitActor && HitActor->ActorHasTag(FName("WallRun"));
		};

	if ((bHitRight && IsValidWall(RightHit.GetActor())) || (bHitLeft && IsValidWall(LeftHit.GetActor())))
	{
		if (!GetCharacterMovement()->IsMovingOnGround())
		{
			CurrentWallNormal = (bHitRight && IsValidWall(RightHit.GetActor())) ? RightHit.ImpactNormal : LeftHit.ImpactNormal;
			StartWallRun(CurrentWallNormal);
		}
	}
	else
	{
		StopWallRun();
		CurrentWallNormal = FVector::ZeroVector;
	}

	// Disable players from falling when wall running
	if (bIsWallRunning)
	{
		FVector Velocity = GetCharacterMovement()->Velocity;
		Velocity.Z = 0.0f;
		GetCharacterMovement()->Velocity = Velocity;
	}
}

void AFPSCharacter::StartWallRun(const FVector& WallNormal)
{
	if (bIsWallRunning)
	{
		return;
	}
	bIsWallRunning = true;
	bHasDashed = false;

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->GravityScale = WallRunGravityScale;
	Movement->Velocity = FVector::VectorPlaneProject(Movement->Velocity, WallNormal).GetSafeNormal() * WallRunSpeed;

	// Lock rotation so player WONT move with rotation while wall running
	bUseControllerRotationYaw = false;
	Movement->bOrientRotationToMovement = false;

	//UE_LOG(LogTemp, Warning, TEXT("Wall run started"));
}

void AFPSCharacter::StopWallRun()
{
	if (!bIsWallRunning)
	{
		return;
	}
	bIsWallRunning = false;

	WallConnectTimer = 0.f;

	GetCharacterMovement()->GravityScale = 1.f;

	// Unlock rotation so player will move with camera
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	//UE_LOG(LogTemp, Warning, TEXT("Wall run stopped"));
}

void AFPSCharacter::StartDash()
{
	if (bHasDashed || !GetCharacterMovement()->IsFalling() || bIsWallRunning)
	{
		return;
	}

	bHasDashed = true;

	FVector Forward = GetCharacterMovement()->GetForwardVector();
	Forward.Normalize();

	FVector DashVelocity = Forward * DashPower;
	DashVelocity.Z += DashUpwardBoost;

	LaunchCharacter(DashVelocity, true, true);

	UE_LOG(LogTemp, Warning, TEXT("Dash Happened!"));
}

void AFPSCharacter::CheckForInteraction(float DeltaTime)
{
	FVector Start = GetActorLocation();
	FVector Forward = GetActorForwardVector();
	FVector EndForward = Start + (Forward * InteractCheckDistance);

	FHitResult Hit;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		EndForward,
		ECC_Visibility,
		Params
	);

	auto IsValidWall = [](AActor* HitActor)
		{
			return HitActor && HitActor->ActorHasTag(FName("Cannon"));
		};

	if ((bHit && IsValidWall(Hit.GetActor())))
	{
		bCanInteract = true;
		InteractableActor = Hit.GetActor();
		DrawDebugLine(GetWorld(), Start, EndForward, FColor::Green, false, 0.f, 0, 1.0f);
	}
	else
	{
		bCanInteract = false;
		InteractableActor = nullptr;
		DrawDebugLine(GetWorld(), Start, EndForward, FColor::Red, false, 0.f, 0, 1.0f);
	}
}

void AFPSCharacter::InteractInput()
{
	if (!bCanInteract)
	{
		return;
	}

	// Call the ShootPlayer function from Cannon.CPP
	ACannon* Cannon = Cast<ACannon>(InteractableActor);
	if (Cannon)
	{
		Cannon->ShootPlayer(this);
		UE_LOG(LogTemp, Warning, TEXT("Interacted with Cannon"));
	}
}

void AFPSCharacter::StartShoot(float Power, float Duration, FVector Direction)
{
	bStartedShoot = true;
	ShootPower = Power;
	ShootDuration = Duration;
	ShootDirection = Direction;

	FVector ShootVelocity = Direction.GetSafeNormal() * Power;
	LaunchCharacter(ShootVelocity, true, true);
	//GetCharacterMovement()->DisableMovement();

	UE_LOG(LogTemp, Warning, TEXT("Cannon shot started"));
}

void AFPSCharacter::ShootPlayer(float DeltaTime)
{
	if (!bStartedShoot)
	{
		return;
	}

	if (ShootDuration > 0.f)
	{
		ShootDuration -= DeltaTime;
	}
}


void AFPSCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void AFPSCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void AFPSCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AFPSCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void AFPSCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void AFPSCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}
