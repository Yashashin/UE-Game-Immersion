// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Components/TimelineComponent.h"
#include "AssassinateInterface.h"
#include "MyGameCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UAttackSystem;
class UEquipmentSystem;
class UInputSystem;
class UClimbSystem;
class UInteractSystem;
class UCoverSystem;
class UMyGameInstance;
class UPlayerStats;
class UShootingSystem;
class UEquipmentManager;
class UMotionWarpingComponent;
struct FInputActionValue;

UENUM(BlueprintType)
enum class EMyMovementMode : uint8
{
	Locomotion UMETA(DisplayName = "Locomotion"),
	Aiming UMETA(DisplayName = "Aiming"),
	Attacking UMETA(DisplayName = "Attacking"),
	Vaulting UMETA(DisplayName = "Vaulting"),
	Rolling UMETA(DisplayName = "Rolling"),
	//PickingItem UMETA(DisplayName = "Picking Item"),
	Climbing UMETA(DisplayName = "Climbing"),
	Covering UMETA(DisplayName = "Covering"),
	None UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class ECameraState : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Far UMETA(DisplayName = "Far"),
	Close UMETA(DisplayName = "Close"),
	Indoor UMETA(DisplayName = "Indoor"),
	Aiming  UMETA(DisplayName = "Aiming"),
	Death UMETA(DisplayName = "Death"),
};

UENUM(BlueprintType)
enum class EUpdateUI : uint8
{
	HUD_EquipmentIcon UMETA(DisplayName = "HUD Equipment Icon"),
	HUD_AmmoCount UMETA(DisplayName = "HUD Ammo Count"),
	HUD_HealthBar UMETA(DisplayName = "HUD Health Bar"),
	Crosshair UMETA(DisplayName = "Crosshair"),
};

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class AMyGameCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Camera boom positioning the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	// Follow camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	ECameraState CameraState = ECameraState::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> AimingCameraShakeClass;

	UPROPERTY(EditAnywhere, Category = "Vault")  // Set in blueprint
	TObjectPtr<UAnimMontage> VaultMontage;

	UPROPERTY(VisibleAnywhere, Category = "Vault")
	FVector VaultStartPos = FVector::Zero();

	UPROPERTY(VisibleAnywhere, Category = "Vault")
	FVector VaultMidPos = FVector::Zero();

	UPROPERTY(VisibleAnywhere, Category = "Vault")
	FVector VaultLandPos = FVector::Zero();

	UPROPERTY(EditAnywhere, Category = "Assassinate") // Set in blueprint
	TObjectPtr<UAnimMontage> AssassinateMontage;

	UPROPERTY(EditAnywhere, Category = "Roll") // Set in blueprint
	TObjectPtr<UAnimMontage> RollForwardMontage;

	UPROPERTY(EditAnywhere, Category = "Roll") // Set in blueprint
	TObjectPtr<UAnimMontage> RollBackwardMontage;

	UPROPERTY(EditAnywhere, Category = "Death") // Set in blueprint
	TArray<TObjectPtr<UAnimMontage>> NormalDeathMontages;

	UPROPERTY(EditAnywhere, Category = "Death") // Set in blueprint
	TArray<TObjectPtr<UAnimMontage>> AimingDeathMontages;

	UPROPERTY(EditAnywhere, Category = "Death") // Set in blueprint
	TArray<TObjectPtr<UAnimMontage>> ProneDeathMontages;

	// My movement mode defined by custom states.
	// Since it's an enumeration, the character can't be in multiple states at the same time.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character State")
	EMyMovementMode MyMovementMode = EMyMovementMode::Locomotion;

	// Is character holding weapon?
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character State")
	bool bIsArmed = false;

	// Should character forced to walk?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State")
	bool bForecedToWalk = false;

	// Should enable turn in place?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State")
	bool bEnableTurnInPlace = false;

	// Is character dead?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State")
	bool isDead = false;

	// Right arm animation blending weight
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blending Weight")
	float RightArmBlendWeight = 0.0f;

	// Upper body animation blending weight
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blending Weight")
	float UpperBodyBlendWeight = 0.0f;

	// Actor component for motion warping
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Component")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	// Actor component for player stats
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Component")
	TObjectPtr<UPlayerStats> PlayerStats;

	// Actor component for attack system
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Component");
	TObjectPtr<UAttackSystem> AttackSystem;

	// Actor component for quipment system
	UPROPERTY(VisibleAnywhere, Category = "Player Component");
	TObjectPtr<UEquipmentSystem> EquipmentSystem;

	// Actor component for input system
	UPROPERTY(VisibleAnywhere, Category = "Player Component");
	TObjectPtr<UInputSystem> InputSystem;

	// Actor component for climb system
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Component");
	TObjectPtr<UClimbSystem> ClimbSystem;

	// Actor component for interact system
	UPROPERTY(VisibleAnywhere, Category = "Player Component");
	TObjectPtr<UInteractSystem> InteractSystem;

	// Actor component for cover system
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Component");
	TObjectPtr<UCoverSystem> CoverSystem;

	// Actor component for shooting system
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Component");
	TObjectPtr<UShootingSystem> ShootingSystem;

	// Actor component for weapon manager
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Component");
	TObjectPtr<UEquipmentManager> EquipmentManager;

	// This mesh is used for motion matching animation
	// The character mesh retarget the animation from this mesh
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> VirtualMesh;

	// Multiplier for sensitivity yaw angle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Settings", meta = (ClampMin = 0.1, ClampMax = 2)) // Set in blueprint
		float NormalSensitivityYaw = 1.0f;

	// Multiplier for sensitivity pitch angle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Settings", meta = (ClampMin = 0.1, ClampMax = 2)) // Set in blueprint
		float NormalSensitivityPitch = 1.0f;

	// Multiplier for sensitivity yaw angle while aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Settings", meta = (ClampMin = 0.1, ClampMax = 2)) // Set in blueprint
		float AimingSensitivityYaw = 1.0f;

	// Multiplier for sensitivity pitch angle while aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Settings", meta = (ClampMin = 0.1, ClampMax = 2)) // Set in blueprint
		float AimingSensitivityPitch = 1.0f;

private:
	TObjectPtr<ACharacter> LockedTarget;

	// Access my game instance
	TWeakObjectPtr<UMyGameInstance> MyGameInstance;

	// Timeline callback binding delegate
	FOnTimelineFloat CamDistananceTimelineDelegate;

	// Vault animation montage end binding delegate 
	FOnMontageEnded VaultMontageEndDelegate;

	// Dodge animation montage end biding delegate 
	FOnMontageEnded RollMontageEndDelegate;
public:
	AMyGameCharacter();

	/** Timeline callback function */
	UFUNCTION(BlueprintCallable)
	void CameDistanceTimelineReturn(float value);

	UFUNCTION(BlueprintCallable)
	void VaultMotionWarp();

	/** Vault animation montage callback function */
	UFUNCTION(BlueprintCallable)
	void OnVaultMontageEnd();

	/** Dodge animation montage callback function */
	UFUNCTION(BlueprintCallable)
	void OnRollMontageEnd();

	void Dead();

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void Interact();

	void Melee();

	void StartAiming();

	void StopAiming();

	void StartFiring();

	void StopFiring();

	void Reload();

	void SwitchEquipmentUp();

	void SwitchEquipmentDown();

	void SwitchEquipmentRight();

	void SwitchEquipmentLeft();

	void ToggleEquipment();

	virtual void Jump() override;

	virtual void Crouch(bool bClientSimulation = false) override;

	UFUNCTION(BlueprintCallable)
	void Ragdoll();

	UFUNCTION(BlueprintPure)
	bool IsAiming() const { return MyMovementMode == EMyMovementMode::Aiming; }

	UFUNCTION(BlueprintPure)
	bool ShouldDoLeftHandIK() const;

	/** This function is overridden in blueprint to notify a UI update event. */
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateUINotify(EUpdateUI UpdateType);

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	// Override TakeDamage to handle the damage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

private:
	/** Called for vault input */
	void Vault();

	/** Called for assassinate input */
	void Assassinate();

	/** Called for sprint start input */
	void SprintStart();

	/** Called for sprint end input */
	void SprintEnd();

	/** Called for lock input */
	void ToggleLock();

	/** Called for menu input*/
	void OpenInGameMenu();

	/** Called for climb input*/
	void Climb();

	/** Called for cover input*/
	void Cover();

	void BasicMovement(FVector2D MovementVector);

	bool IsMovingForward() const;

	void Roll();
};

