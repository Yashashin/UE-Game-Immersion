// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MovementStruct.h"
#include "AssassinateInterface.h"
#include "Components/TimelineComponent.h"
#include "Enums.h"
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
//struct FTimeline;
//struct FOnTimelineFloat;


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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	TObjectPtr<UCurveFloat> TimelineDefaultCurve;

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

	// My motion mode defined by custom actions.
	// Since it's an enumeration, the character can't be in multiple modes at the same time.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State")
	EMotionMode MotionMode = EMotionMode::Locomotion;

	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State")
	EMyMovementMode MovementMode = EMyMovementMode::OnGround;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State")
	EMovementState MovementState = EMovementState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State")
	EStance Stance = EStance::Stand;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State")
	EGait Gait = EGait::Walk;

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
	bool bIsDead = false;

	// Does character want to sprint?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State")
	bool bWantsToSprint = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curve") // Set in blueprint
		TObjectPtr<UCurveFloat> StrafeSpeedMapCurve;

	// Right arm animation blending weight
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blending Weight")
	float RightArmBlendWeight = 0.0f;

	// Upper body animation blending weight
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blending Weight")
	float UpperBodyBlendWeight = 0.0f;

	// Actor component for motion warping
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Component")
	TObjectPtr<UMotionWarpingComponent> MotionWarping;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Custom", meta = (ClampMin = 0.1, ClampMax = 2)) // Set in blueprint
		float NormalSensitivityYaw = 1.0f;

	// Multiplier for sensitivity pitch angle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Custom", meta = (ClampMin = 0.1, ClampMax = 2)) // Set in blueprint
		float NormalSensitivityPitch = 1.0f;

	// Multiplier for sensitivity yaw angle while aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Custom", meta = (ClampMin = 0.1, ClampMax = 2)) // Set in blueprint
		float AimingSensitivityYaw = 1.0f;

	// Multiplier for sensitivity pitch angle while aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Custom", meta = (ClampMin = 0.1, ClampMax = 2)) // Set in blueprint
		float AimingSensitivityPitch = 1.0f;

	// Threshold for walk and run. Stick analog below this value is walking and above this is running. 
	// Sprinting does not take this value into account.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings | Developer", meta = (ClampMin = 0.1, ClampMax = 0.9)) // Set in blueprint
		float AnalogInputWalkRunThreshold = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings | Developer") // Set in blueprint
		FMovementSpeedsSet MovementSpeedsSet;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Developer") // Set in blueprint
	//	TUniquePtr <FMovementSpeeds> ArmedSpeeds;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Developer") // Set in blueprint
	//	TUniquePtr <FMovementSpeeds> AimingSpeeds;

private:
	TObjectPtr<ACharacter> LockedTarget;

	// Access my game instance
	TWeakObjectPtr<UMyGameInstance> MyGameInstance;

	FTimeline CrouchTimeline;

	// Timeline callback binding delegate
	FOnTimelineFloat CrouchTimelineDelegate;

	// Vault animation montage end binding delegate 
	FOnMontageEnded VaultMontageEndDelegate;

	// Dodge animation montage end biding delegate 
	FOnMontageEnded RollMontageEndDelegate;
public:
	AMyGameCharacter();

	/** Crouch timeline callback function */
	UFUNCTION()
	void CrouchTimelineReturn(float value);

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

	void StartSprinting();

	void StopSprinting();

	void Reload();

	void SwitchEquipmentUp();

	void SwitchEquipmentDown();

	void SwitchEquipmentRight();

	void SwitchEquipmentLeft();

	void ToggleEquipment();

	virtual void Jump() override;

	void ToggleCrouched();

	UFUNCTION(BlueprintCallable)
	void Ragdoll();

	UFUNCTION(BlueprintPure)
	bool IsAiming() const { return MotionMode == EMotionMode::Aiming; }

	UFUNCTION(BlueprintPure)
	bool ShouldDoLeftHandIK() const;

	/** This function is overridden in blueprint that used to notify a UI update event. */
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

	/** This function is used to update the Gait value and use it to set the max walk speeds of the character movement component.*/
	void UpdateGait();

	/** Return desired gait.*/
	EGait GetDesiredGait() const;

	float CalculateMaxAcceleration() const;

	float CalculateBrakingDeceleration() const;

	float CalculateGroundFriction() const;

	/** This function is used to set the max speed for the character¡¦s movement.
		Because the forwards, strafes, and backwards animations move at different speeds, we need to change the max speed of the character based on its movement direction.
		We use a simple curve to map different speed values to the different directions. 0 = forward, 1 = strafe L or R, 2 = Backwards.
		Return X for walk speed and Y for crouch speed.
	*/
	FVector2D CalculateMaxSpeed() const;

	bool CanSprint() const;

};

