// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Weapon.h"
#include "PlayerAnimInstance.h"
#include "FPSCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY()
	TSubclassOf<UAnimInstance> UnarmedAnimClass;

	UPROPERTY()
	TSubclassOf<UAnimInstance> PistolAnimClass;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DashAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InteractAction;

	/** Shoot Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ShootAction;
	
public:
	AFPSCharacter();

protected:

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	
	virtual void Jump() override;

public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UPROPERTY()
	UPlayerAnimInstance* PlayerAnimInstance;

private:
	void DebugFunc();

	// Handles additional gravity when player is falling
	void FallingGravity(float DeltaTime);

	float FallGravityMultiplier;

	UPROPERTY(EditAnywhere, Category = "Falling")
	float FallGravityScaler = 5.f;

	UPROPERTY(EditAnywhere, Category = "Falling")
	float FallGravityMultiplierMin = 10.f;

	UPROPERTY(EditAnywhere, Category = "Falling")
	float FallGravityMultiplierMax = 100.f;

	// Koyote Time
	void KoyoteJump(float DeltaTime);

	UPROPERTY(VisibleAnywhere, Category = "Jumping")
	float TimeSinceLeftGround = 0.f;

	UPROPERTY(EditAnywhere, Category = "Jumping")
	float KoyoteTime = .2f;

	// Double Jump
	bool bHasDoubleJumped = false;
	FVector PreviousDirection = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Double Jump")
	float DoubleJumpForwardBoost = 600.f;

protected:
	void CheckForWall(float DeltaTime);
	void StartWallRun(const FVector& WallNormal);
	void StopWallRun();

	bool bIsWallRunning = false;
	FVector CurrentWallNormal = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Category = "Wall Run")
	float WallConnectTimer = 0.f;

	UPROPERTY(EditAnywhere, Category = "Wall Run")
	float WallCheckDistance = 100.f;

	UPROPERTY(EditAnywhere, Category = "Wall Run")
	float WallRunGravityScale = .2f;

	UPROPERTY(EditAnywhere, Category = "Wall Run")
	float WallRunSpeed = 600.f;

protected:
	void StartDash();

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashPower = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashUpwardBoost = 300.f;

	FVector DashDirection;

	bool bHasDashed = false;

protected:
	void CheckForInteraction(float DeltaTime);
	void InteractInput();

	bool bCanInteract = false;

	UPROPERTY(EditAnywhere, Category = "Interact")
	float InteractCheckDistance = 100.f;

	UPROPERTY()
	AActor* InteractableActor;

	void ShootPlayer(float DeltaTime);
	bool bStartedShoot = false;
	float ShootPower;
	float ShootDuration;
	FVector ShootDirection;

public:
	void StartShoot(float Power, float UpwardBoost, float Duration, FVector Direction);

protected:
	void ShootWeapon();
	void PickupWeapon(AWeapon* Weapon);
	void SwitchWeapon();

	// Current chosen weapon
	AWeapon* SelectedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	TArray<AWeapon*> OwnedWeapons;

};

