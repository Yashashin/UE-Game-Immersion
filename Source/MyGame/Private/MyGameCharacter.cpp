
#include "MyGameCharacter.h"

#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Curves/CurveFloat.h"
#include "Components/CapsuleComponent.h"


#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "InputActionValue.h"
#include "MotionWarpingComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"

#include "AttackSystem.h"
#include "InputSystem.h"
#include "ClimbSystem.h"
#include "CoverSystem.h"
#include "PlayerStats.h"
#include "ShootingSystem.h"
#include "MyGameInstance.h"
#include "MyGameAI.h"

#include "Interact/InteractSystem.h"

#include "Equipment/EquipmentManager.h"
#include "Equipment/EquipmentSystem.h"

#include "Weapon/RangedWeaponBase.h"




DEFINE_LOG_CATEGORY(LogTemplateCharacter);

#ifndef MY_GAME_CHARACTER_MACRO

// Walk
#define SPRINT_MAX_WALK_SPEED 800.0f

// Crouch
#define UNCROUCH_TRACE_DISTANCE 100.0f
#define UNCROUCH_TRACE_SIZE 10.0f

// Lock
#define LOCK_TRACE_DISTANCE 1500.0f
#define LOCK_TRACE_SIZE 200.0f
#define LOCK_CAMERA_UP 100.0f

// Vault
#define VAULT_DETECT_DISTANCE 180.0f
#define VAULT_LAND_DISTANCE 80.0f

#endif // !MY_GAME_CHARACTER_MACRO




//////////////////////////////////////////////////////////////////////////
// AMyGameCharacter

AMyGameCharacter::AMyGameCharacter()
{
	// Configure tags
	Tags.Add(TEXT("Character"));
	Tags.Add(TEXT("Player"));

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(45.0f, 90.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character doesn't move in the direction of input
	GetCharacterMovement()->bUseControllerDesiredRotation = true; // Character moves in the direction of controller rotation	
	GetCharacterMovement()->NavAgentProps.bCanFly = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	// Crouch timeline
	CrouchTimelineDelegate.BindUFunction(this, "CrouchTimelineReturn");

	// Bind delegates
	VaultMontageEndDelegate.BindUFunction(this, "OnVaultMontageEnd");
	RollMontageEndDelegate.BindUFunction(this, "OnRollMontageEnd");

	// Create player components
	MotionWarping = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));
	PlayerStats = CreateDefaultSubobject<UPlayerStats>(TEXT("Player Stats"));
	AttackSystem = CreateDefaultSubobject<UAttackSystem>(TEXT("Attack System"));
	EquipmentSystem = CreateDefaultSubobject<UEquipmentSystem>(TEXT("Equipment System"));
	InputSystem = CreateDefaultSubobject<UInputSystem>(TEXT("Input System"));
	ClimbSystem = CreateDefaultSubobject<UClimbSystem>(TEXT("Climb System"));
	InteractSystem = CreateDefaultSubobject<UInteractSystem>(TEXT("Interact System"));
	CoverSystem = CreateDefaultSubobject<UCoverSystem>(TEXT("Cover System"));
	ShootingSystem = CreateDefaultSubobject<UShootingSystem>(TEXT("Shooting System"));
	EquipmentManager = CreateDefaultSubobject<UEquipmentManager>(TEXT("Equipment Manager"));

	// Create the virtual skeletal mesh and attach it to the character mesh.
	// This mesh is for motion matching animation.
	VirtualMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Virtual Mesh"));
	VirtualMesh->SetupAttachment(GetMesh());
}

void AMyGameCharacter::BeginPlay()
{
	Super::BeginPlay();


	// Game instance and create widget
	MyGameInstance = Cast<UMyGameInstance>(GetGameInstance());
	if (MyGameInstance.IsValid())
	{
		UMyGameInstance* GameInstancePtr = MyGameInstance.Get();

		GameInstancePtr->CreateHUDWidget();
		GameInstancePtr->CreateInGameMenuWidget();
		GameInstancePtr->CreateShootingWidget();
	}

	// Init components
	EquipmentSystem->Init();
	ClimbSystem->Init();
	CoverSystem->Init();

	CrouchTimeline.AddInterpFloat(TimelineDefaultCurve, CrouchTimelineDelegate);
	CrouchTimeline.SetLooping(false);
}

void AMyGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Lock enemy
	if (IsValid(LockedTarget))
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation() + FVector(0, 0, LOCK_CAMERA_UP), LockedTarget->GetActorLocation());

		Controller->SetControlRotation(LookAtRotation);
	}

	// Timeline
	if (CrouchTimeline.IsPlaying())
	{
		CrouchTimeline.TickTimeline(DeltaTime);
	}
	FVector2D InputValue = InputSystem->GetLStickActionValue().Get<FVector2D>();
	UE_LOG(LogTemp, Warning, TEXT("%f"), InputValue.Length());
	UpdateGait();
}

float AMyGameCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	bIsDead = PlayerStats->DecreaseCurrentHealth(DamageAmount);
	if (bIsDead)
	{
		Dead();
	}
	return ActualDamage;
}

bool AMyGameCharacter::IsMovingForward() const
{
	FVector Velocity = GetVelocity();
	FVector ForwardVector = GetActorForwardVector();
	float DotProduct = FVector::DotProduct(Velocity.GetSafeNormal(), ForwardVector);
	return DotProduct > 0;
}

void AMyGameCharacter::Roll()
{
	if (MotionMode == EMotionMode::Rolling)
	{
		return;
	}

	if (AttackSystem->GetIsAttacking())
	{
		if (AttackSystem->AttackMode == EAttackMode::Melee)
		{
			switch (AttackSystem->MeleeMode)
			{
			case EMeleeMode::H2H:
				AttackSystem->H2HKickComboStop();
				break;
			default:
				break;
			}
		}

	}
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	MotionMode = EMotionMode::Rolling;
	//SetActorEnableCollision(false);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	//if (IsMovingForward())
	//{
	//	AnimInstance->Montage_Play(RollForwardMontage);
	//	AnimInstance->Montage_SetEndDelegate(RollMontageEndDelegate, RollForwardMontage);
	//}
	//else
	//{
	//	AnimInstance->Montage_Play(RollBackwardMontage);
	//	AnimInstance->Montage_SetEndDelegate(RollMontageEndDelegate, RollBackwardMontage);
	//}

	AnimInstance->Montage_Play(RollForwardMontage);
	AnimInstance->Montage_SetEndDelegate(RollMontageEndDelegate, RollForwardMontage);
}

void AMyGameCharacter::UpdateGait()
{
	Gait = GetDesiredGait();

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent->MaxAcceleration = CalculateMaxAcceleration();
		MovementComponent->BrakingDecelerationWalking = CalculateBrakingDeceleration();
		MovementComponent->GroundFriction = CalculateGroundFriction();
		FVector2D MaxSpeeds = CalculateMaxSpeed();
		MovementComponent->MaxWalkSpeed = MaxSpeeds.X;
		MovementComponent->MaxWalkSpeedCrouched = MaxSpeeds.Y;
	}
}

bool AMyGameCharacter::ShouldDoLeftHandIK() const
{

	bool bWeapon = (EquipmentManager->GetCurrentEquipmentType() == EEquipmentType::Rifle) || (EquipmentManager->GetCurrentEquipmentType() == EEquipmentType::Shotgun);
	if ((IsAiming() || bWeapon) /*&& !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying()*/)
	{
		return true;
	}
	else
	{
		return false;
	}
}


EGait AMyGameCharacter::GetDesiredGait() const
{
	// If character is sprinting, keep sprinting.
	if (bWantsToSprint && CanSprint())
	{
		return EGait::Sprint;
	}

	EGait DesiredGait = EGait::Walk;
	if (bForecedToWalk)
	{
		DesiredGait = EGait::Walk;
	}
	else
	{
		// Decide gait based on threshold
		FVector2D InputValue = InputSystem->GetLStickActionValue().Get<FVector2D>();
		UE_LOG(LogTemp, Warning, TEXT("%f"), InputValue.Length());
		if (InputValue.Length() >= AnalogInputWalkRunThreshold)
		{
			DesiredGait = EGait::Run;
		}
		else
		{
			DesiredGait = EGait::Walk;
		}
	}
	return DesiredGait;
}

float AMyGameCharacter::CalculateMaxAcceleration() const
{
	float Value = InputSystem->GetLStickActionValue().Get<FVector2D>().Length();
	float MaxAcceleration = 600.0f;
	if (Gait == EGait::Sprint)
	{
		FVector2D InRange = FVector2D(300.0f, 700.0f);
		FVector2D OutRange = FVector2D(600.0f, 300.0f);
		MaxAcceleration = FMath::GetMappedRangeValueClamped(InRange, OutRange, Value);
	}
	return MaxAcceleration;
}

float AMyGameCharacter::CalculateBrakingDeceleration() const
{
	float BrakingDeceleration = 2000.0f;
	if (GetPendingMovementInputVector().Length() != 0)
	{
		BrakingDeceleration = 500.0f;
	}
	return BrakingDeceleration;
}

float AMyGameCharacter::CalculateGroundFriction() const
{
	float GroundFriction = 5.0f;
	if (Gait == EGait::Sprint)
	{
		float Value = GetCharacterMovement()->Velocity.Length();
		GroundFriction = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 500.0f), FVector2D(5.0f, 3.0f), Value);
	}
	return GroundFriction;
}

FVector2D AMyGameCharacter::CalculateMaxSpeed() const
{
	FVector Velocity = GetCharacterMovement()->Velocity;
	FRotator BaseRotation = GetActorRotation();
	float Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, BaseRotation);
	float StrafeSpeedMap = StrafeSpeedMapCurve->GetFloatValue(FMath::Abs(Direction));

	FMovementSpeeds MovementSpeeds;
	if (MotionMode == EMotionMode::Locomotion && !bIsArmed)
	{
		MovementSpeeds = MovementSpeedsSet.NormalSpeeds;
	}
	else if (MotionMode == EMotionMode::Locomotion)
	{
		MovementSpeeds = MovementSpeedsSet.ArmedSpeeds;
	}
	else if (MotionMode == EMotionMode::Aiming)
	{
		MovementSpeeds = MovementSpeedsSet.AimingSpeeds;
	}

	FVector MaxSpeeds = FVector::Zero();
	FVector MaxCrouchSpeeds = FVector::Zero();

	switch (Gait)
	{
	case EGait::Walk:
		MaxSpeeds = MovementSpeeds.WalkSpeeds;
		break;
	case EGait::Run:
		MaxSpeeds = MovementSpeeds.RunSpeeds;
		break;
	case EGait::Sprint:
		MaxSpeeds = MovementSpeeds.SprintSpeeds;
		break;
	default:
		break;
	}

	MaxCrouchSpeeds = MovementSpeeds.CrouchSpeeds;

	float ReturnMaxSpeed = 0.0f;
	float ReturnMaxCrouchSpeed = 0.0f;
	if (StrafeSpeedMap < 1)
	{
		ReturnMaxSpeed = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(MaxSpeeds.X, MaxSpeeds.Y), StrafeSpeedMap);
		ReturnMaxCrouchSpeed = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(MaxCrouchSpeeds.X, MaxCrouchSpeeds.Y), StrafeSpeedMap);
	}
	else
	{
		ReturnMaxSpeed = FMath::GetMappedRangeValueClamped(FVector2D(1.0f, 2.0f), FVector2D(MaxSpeeds.Y, MaxSpeeds.Z), StrafeSpeedMap);
		ReturnMaxCrouchSpeed = FMath::GetMappedRangeValueClamped(FVector2D(1.0f, 2.0f), FVector2D(MaxCrouchSpeeds.Y, MaxCrouchSpeeds.Z), StrafeSpeedMap);
	}

	return FVector2D(ReturnMaxSpeed, ReturnMaxCrouchSpeed);
}

bool AMyGameCharacter::CanSprint() const
{
	FVector Direction =GetCharacterMovement()->GetCurrentAcceleration();
	if (!Direction.IsNearlyZero())
	{
		Direction = Direction.GetSafeNormal();
		FRotator Rotator = UKismetMathLibrary::MakeRotFromX(Direction);
		FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetActorRotation(), Rotator);

		return FMath::Abs(DeltaRotation.Yaw) < 50.0f;
	}

	return false;
}



void AMyGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	InputSystem->SetupInputBinding(PlayerInputComponent);
}


void AMyGameCharacter::CrouchTimelineReturn(float value)
{
	//CameraBoom->TargetArmLength = FMath::Lerp(WALK_CAMERA_DISTANCE, CROUCH_CAMERA_DISTANCE, value);
	//GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::Lerp(60.0f, 90.0f, value));
	//GetMesh()->SetRelativeLocation(FVector(0, 0, FMath::Lerp(-60.0f, -90.0f, value)));
}

void AMyGameCharacter::Vault()
{
	bool bIsHit = false;
	FVector HitLocation;

	// Horizontal forward trace
	for (int i = 0; i < 3; i++)
	{
		FVector Start = GetActorLocation() + FVector(0, 0, 30.0f * i) + GetActorForwardVector() * 30.0f;
		FVector End = Start + GetActorForwardVector() * VAULT_DETECT_DISTANCE;
		FHitResult HitResult;
		if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, 5, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::Type::ForDuration, HitResult, true))
		{
			HitLocation = HitResult.Location;
			bIsHit = true;
			break;
		}
	}

	// Vertical trace on top of obstacle
	if (bIsHit)
	{
		bool bHasStartPos = false;
		bool bHasMidPos = false;
		bool bHasLandPos = false;
		for (int i = 0; i < 5; i++)
		{
			FVector Start = HitLocation + FVector(0, 0, 100) + GetActorForwardVector() * 50 * i;
			FVector End = Start + FVector(0, 0, -100);
			FHitResult HitResult;
			if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, 10, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::Type::ForDuration, HitResult, true, FLinearColor::Green, FLinearColor::Red))
			{
				if (i == 0) // Valut start point
				{
					VaultStartPos = HitResult.Location;
					bHasStartPos = true;
					//UKismetSystemLibrary::DrawDebugSphere(GetWorld(), VaultStartPos, 10, 12, FLinearColor::White, 10);
				}
				else // Valut middle point
				{
					VaultMidPos = HitResult.Location;
					bHasMidPos = true;
					//UKismetSystemLibrary::DrawDebugSphere(GetWorld(), VaultMiddlePos, 10, 12, FLinearColor::Blue, 10);
				}
			}
			else // Vault land point
			{

				Start = GetActorForwardVector() * VAULT_LAND_DISTANCE + HitResult.TraceEnd;
				End = Start + FVector(0, 0, -1000);
				if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::Type::ForDuration, HitResult, true, FLinearColor::Blue, FLinearColor::Red))
				{
					VaultLandPos = HitResult.Location;
					FVector MeshLocation = GetMesh()->K2_GetComponentLocation();
					if (VaultLandPos.Z < MeshLocation.Z + 50 && VaultLandPos.Z > MeshLocation.Z - 50) // Avoid landing position too high or too low
					{
						bHasLandPos = true;
					}
					break;
				}
			}
		}
		if (bHasStartPos && bHasMidPos && bHasLandPos)
		{
			VaultMotionWarp();
		}
	}
}

void AMyGameCharacter::Assassinate()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);
	for (AActor* Actor : OverlappingActors)
	{
		FVector RefLocation = FVector::ZeroVector;
		FRotator AssassinateRotation = FRotator::ZeroRotator;
		AMyGameAI* Enemy = Cast<AMyGameAI>(Actor);
		Enemy->StealthBackAssassinate(RefLocation, AssassinateRotation);

		FMotionWarpingTarget* Target = new FMotionWarpingTarget();
		Target->Name = FName("AssassinateWarp");
		Target->Location = RefLocation;
		Target->Rotation = AssassinateRotation;
		MotionWarping->AddOrUpdateWarpTarget(*Target);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(AssassinateMontage);
	}
}

void AMyGameCharacter::StartFiring()
{
	if (MotionMode == EMotionMode::Aiming)
	{
		// Enable firing
		ShootingSystem->bShouldFire = true;
		//ShootingSystem->SetComponentTickEnabled(true);
	}
}

void AMyGameCharacter::StopFiring()
{
	// Disable firing
	//ShootingSystem->SetComponentTickEnabled(false);
	ShootingSystem->bShouldFire = false;
}

void AMyGameCharacter::StartSprinting()
{
	bWantsToSprint = true;
}

void AMyGameCharacter::StopSprinting()
{
	bWantsToSprint = false;
}

void AMyGameCharacter::Reload()
{
	ShootingSystem->Reload();
}

void AMyGameCharacter::SwitchEquipmentUp()
{
	EquipmentManager->Equip(EEquipmentType::Flashlight);
}

void AMyGameCharacter::SwitchEquipmentDown()
{
	EquipmentManager->Equip(EEquipmentType::Shotgun);
}

void AMyGameCharacter::SwitchEquipmentRight()
{
	EquipmentManager->Equip(EEquipmentType::Pistol);
}

void AMyGameCharacter::SwitchEquipmentLeft()
{
	EquipmentManager->Equip(EEquipmentType::Rifle);
}

void AMyGameCharacter::ToggleEquipment()
{
	EquipmentManager->ToggleEquipment();
}

void AMyGameCharacter::ToggleLock()
{
	if (IsValid(LockedTarget))
	{
		LockedTarget = nullptr;
		MotionMode = EMotionMode::Locomotion;
		return;
	}

	FVector Start = FollowCamera->GetComponentLocation();// GetMesh()->GetSocketLocation(TEXT("head"));
	FVector End = FollowCamera->GetComponentLocation() + FollowCamera->GetForwardVector() * LOCK_TRACE_DISTANCE;
	TArray<FHitResult> HitResults;
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	if (UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), Start, End, LOCK_TRACE_SIZE, ObjectTypes, false, IgnoredActors, EDrawDebugTrace::Type::ForDuration, HitResults, true, FColor::Blue, FColor::Red))
	{
		for (FHitResult& HitResult : HitResults)
		{
			if (HitResult.GetActor()->ActorHasTag(TEXT("Enemy")))
			{
				LockedTarget = Cast<ACharacter>(HitResult.GetActor());
				MotionMode = EMotionMode::Attacking;
				return;
			}
		}
	}

}

void AMyGameCharacter::Interact()
{
	InteractSystem->DoInteraction();
}

void AMyGameCharacter::OpenInGameMenu()
{
	/*CloseInGameMenu is written in bluerpint*/

	if (MyGameInstance.IsValid())
	{
		UMyGameInstance* GameInstancePtr = MyGameInstance.Get();
		GameInstancePtr->ToggleHUDWidget(); // Close HUD widget
		GameInstancePtr->ToggleInGameMenuWidget(); // Open in-game menu widget
	}
}

void AMyGameCharacter::Climb()
{
	if (GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Flying)
	{
		FHitResult HitResult;
		if (ClimbSystem->DetectWall(HitResult))
		{
			ClimbSystem->AttachToSurface(HitResult);
		}
	}
	else
	{
		ClimbSystem->StopClimbing();
	}
}

void AMyGameCharacter::Cover()
{
	if (CoverSystem->bIsCovering)
	{
		CoverSystem->StopCovering();
	}
	else
	{
		FHitResult HitResult;
		if (CoverSystem->DetectFrontCover(HitResult))
		{
			CoverSystem->AttachToCover(HitResult);
		}
	}
}


void AMyGameCharacter::VaultMotionWarp()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	SetActorEnableCollision(false);
	MotionMode = EMotionMode::Vaulting;

	// Set vault start
	FMotionWarpingTarget* StartTarget = new FMotionWarpingTarget();
	StartTarget->Name = FName("VaultStart");
	StartTarget->Location = VaultStartPos;
	StartTarget->Rotation = GetActorRotation();
	MotionWarping->AddOrUpdateWarpTarget(*StartTarget);
	// Set vault middle
	FMotionWarpingTarget* MiddleTarget = new FMotionWarpingTarget();
	MiddleTarget->Name = FName("VaultMiddle");
	MiddleTarget->Location = VaultMidPos;
	MiddleTarget->Rotation = GetActorRotation();
	MotionWarping->AddOrUpdateWarpTarget(*MiddleTarget);
	// Set vault land
	FMotionWarpingTarget* LandTarget = new FMotionWarpingTarget();
	LandTarget->Name = FName("VaultLand");
	LandTarget->Location = VaultLandPos;
	LandTarget->Rotation = GetActorRotation();
	MotionWarping->AddOrUpdateWarpTarget(*LandTarget);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(VaultMontage);
	AnimInstance->Montage_SetEndDelegate(VaultMontageEndDelegate, VaultMontage);
}

void AMyGameCharacter::OnVaultMontageEnd()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	SetActorEnableCollision(true);
	MotionMode = EMotionMode::Locomotion;
}

void AMyGameCharacter::OnRollMontageEnd()
{
	//	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	MotionMode = EMotionMode::Locomotion;
	//SetActorEnableCollision(true);
}

void AMyGameCharacter::Dead()
{
	// Disable input
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		//UnPossessed();
		//PlayerController->UnPossess();
		//DisableInput(PlayerController);
	}

	// Decide which montage to play
	UAnimMontage* Montage = nullptr;
	if (MotionMode == EMotionMode::Aiming)
	{
		// Stop aiming before death
		StopAiming();

		int Rand = FMath::RandRange(0, AimingDeathMontages.Num() - 1);
		Montage = AimingDeathMontages[Rand];
	}
	else
	{
		int Rand = FMath::RandRange(0, NormalDeathMontages.Num() - 1);
		Montage = NormalDeathMontages[Rand];
	}

	// Play death montage
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(Montage);
	}

	CameraState = ECameraState::Death;


}

void AMyGameCharacter::BasicMovement(FVector2D MovementVector)
{
	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// add movement 
	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AMyGameCharacter::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void AMyGameCharacter::Move(const FInputActionValue& Value)
{
	// Input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		BasicMovement(MovementVector);
		/*switch (GetCharacterMovement()->MovementMode)
		{
		case EMovementMode::MOVE_Falling:
		case EMovementMode::MOVE_Walking:
			BasicMovement(MovementVector);
			break;
		case EMovementMode::MOVE_Flying:
			if (ClimbSystem->bIsClimbing)
			{
				ClimbSystem->ClimbMovement(GetActorRightVector(), MovementVector.X);
				ClimbSystem->ClimbLeftAndRightValue = MovementVector.X;
				ClimbSystem->ClimbMovement(GetActorUpVector(), MovementVector.Y);
				ClimbSystem->ClimbUpAndDownValue = MovementVector.Y;
			}
			else if (CoverSystem->bIsCovering)
			{
				CoverSystem->CoverSneakMovement(GetActorRightVector() * -1.0f, MovementVector.X);
				CoverSystem->CoverSneakLeftAndRightValue = MovementVector.X;
				if (MovementVector.X < 0)
				{
					CoverSystem->bIsCoverFacingLeft = true;
				}
				else if(MovementVector.X==0)
				{

				}
				else
				{
					CoverSystem->bIsCoverFacingLeft = false;
				}
			}
			break;
		default:
			break;
		}*/
	}
}

void AMyGameCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (MotionMode == EMotionMode::Aiming)
	{
		LookAxisVector.X *= AimingSensitivityYaw;
		LookAxisVector.Y *= AimingSensitivityPitch;
	}
	else
	{
		LookAxisVector.X *= NormalSensitivityYaw;
		LookAxisVector.Y *= NormalSensitivityPitch;
	}

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMyGameCharacter::Melee()
{
	AttackSystem->Attack();
}


void AMyGameCharacter::StartAiming()
{
	MotionMode = EMotionMode::Aiming;
	bForecedToWalk = true;
	if (bIsCrouched)
	{
		CameraState = ECameraState::Crouch_Aiming;
	}
	else
	{
		CameraState = ECameraState::Aiming;
	}


	// Play idle camera shake
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->ClientStartCameraShake(AimingCameraShakeClass);
	}

	if (MyGameInstance.IsValid())
	{
		UMyGameInstance* GameInstancePtr = MyGameInstance.Get();
		GameInstancePtr->ToggleShootingWidget();
		EquipmentManager->AttachEquipmentToSocket(EEquipmentSocket::Aiming);
	}



	// Rotate character a little bit to be more nature
	if (bIsCrouched)
	{
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -86.0f, -2.0f));
	}
	else
	{
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -86.0f, 0.0f));
	}
}

void AMyGameCharacter::StopAiming()
{
	MotionMode = EMotionMode::Locomotion;
	bForecedToWalk = false;
	CameraBoom->bEnableCameraRotationLag = true;
	if (bIsCrouched)
	{
		CameraState = ECameraState::Crouch_Normal;
	}
	else
	{
		CameraState = ECameraState::Normal;
	}

	// Stop idle camera shake
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->ClientStopCameraShake(AimingCameraShakeClass);
	}

	if (MyGameInstance.IsValid())
	{
		UMyGameInstance* GameInstancePtr = MyGameInstance.Get();
		GameInstancePtr->ToggleShootingWidget();
		EquipmentManager->AttachEquipmentToSocket(EEquipmentSocket::Holding);
	}

	// Reset rotation
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	// No shooting if not aiming
	StopFiring();
}

void AMyGameCharacter::ToggleCrouched()
{
	if (bIsCrouched)
	{
		// If there's obstacle above the character, remain crouched
		FVector Start = GetActorLocation() + GetActorUpVector() * UNCROUCH_TRACE_DISTANCE;
		FVector End = Start;
		FHitResult HitResult;
		if (!UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, UNCROUCH_TRACE_SIZE, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::Type::ForDuration, HitResult, true, FLinearColor::Red, FLinearColor::Green, -1.0f))
		{
			UnCrouch();
			CrouchTimeline.PlayFromStart();

			if (IsAiming())
			{
				CameraState = ECameraState::Aiming;
				GetMesh()->SetRelativeRotation(FRotator(0.0f, -86.0f, 0.0f));
			}
			else
			{
				CameraState = ECameraState::Normal;
				GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
			}
		}
	}
	else
	{
		Crouch();
		CrouchTimeline.ReverseFromEnd();

		if (IsAiming())
		{
			CameraState = ECameraState::Crouch_Aiming;
			GetMesh()->SetRelativeRotation(FRotator(0.0f, -86.0f, -2.0f));
		}
		else
		{
			CameraState = ECameraState::Crouch_Normal;
			GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		}
	}
}

void AMyGameCharacter::Ragdoll()
{
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));
	GetMesh()->SetSimulatePhysics(true);
	bIsDead = true;
}
