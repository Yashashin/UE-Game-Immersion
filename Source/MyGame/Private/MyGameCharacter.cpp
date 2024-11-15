
#include "MyGameCharacter.h"

#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "InputActionValue.h"
#include "MotionWarpingComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

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
#define CROUCH_MAX_SPEED 350.0f
#define CROUCH_CAMERA_DISTANCE 550.0f
#define UNCROUCH_TRACE_DISTANCE 115.0f
#define UNCROUCH_TRACE_SIZE 10.0f
#define WALK_CAPSULE_HEIGHT 90.0f
#define CROUCH_CAPSULE_HEIGHT 60.0f

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

	//bUseControllerRotationPitch = false;
	//bUseControllerRotationRoll = false;
	//bUseControllerRotationYaw = false;

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

	CamDistananceTimelineDelegate.BindUFunction(this, "CameDistanceTimelineReturn");

	// Bind delegates
	VaultMontageEndDelegate.BindUFunction(this, "OnVaultMontageEnd");
	RollMontageEndDelegate.BindUFunction(this, "OnRollMontageEnd");

	// Create player components
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("Motion Warping"));
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


	// Game instance and create widgets
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
}

float AMyGameCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	bool bIsDead = PlayerStats->DecreaseCurrentHealth(DamageAmount);
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
	if (MyMovementMode == EMyMovementMode::Rolling)
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
	MyMovementMode = EMyMovementMode::Rolling;
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
//
//FText AMyGameCharacter::GetAmmoText() const
//{
//	return ShootingSystem->GetAmmoText();
//}

void AMyGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	InputSystem->SetupInputBinding(PlayerInputComponent);
}


void AMyGameCharacter::CameDistanceTimelineReturn(float value)
{
	//CameraBoom->TargetArmLength = FMath::Lerp(WALK_CAMERA_DISTANCE, CROUCH_CAMERA_DISTANCE, value);
	//GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::Lerp(WALK_CAPSULE_HEIGHT, CROUCH_CAPSULE_HEIGHT, value));
	//GetMesh()->SetRelativeLocation(FVector(0, 0, FMath::Lerp(-90.0f, -60.0f, value)));
}

void AMyGameCharacter::Vault()
{
	bool bIsHit = false;
	FVector HitLocation;

	// Horizontal forward trace
	for (int i = 0; i < 3; i++)
	{
		FVector Start = GetActorLocation() + FVector(0, 0, 30.0f * i) + GetActorForwardVector()*30.0f;
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
			if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, 10, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::Type::ForDuration, HitResult, true,FLinearColor::Green,FLinearColor::Red))
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
				if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::Type::ForDuration, HitResult, true,FLinearColor::Blue,FLinearColor::Red))
				{
					VaultLandPos = HitResult.Location;
					FVector MeshLocation= GetMesh()->K2_GetComponentLocation();
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
		MotionWarpingComponent->AddOrUpdateWarpTarget(*Target);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(AssassinateMontage);
	}
}

void AMyGameCharacter::SprintStart()
{
	//UCharacterMovementComponent* Movement = GetCharacterMovement();
	//if (MyMovementMode==EMyMovementMode::Crouching)
	//{
	//	ToggleCrouch(); //Cancel crouch
	//}
	//Movement->MaxWalkSpeed = SPRINT_MAX_WALK_SPEED;
}

void AMyGameCharacter::SprintEnd()
{
	/*UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->MaxWalkSpeed = MAX_WALK_SPEED;*/
}

void AMyGameCharacter::StartFiring()
{
	if (MyMovementMode == EMyMovementMode::Aiming)
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
		MyMovementMode = EMyMovementMode::Locomotion;
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
				MyMovementMode = EMyMovementMode::Attacking;
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
	MyMovementMode = EMyMovementMode::Vaulting;

	// Set vault start
	FMotionWarpingTarget* StartTarget = new FMotionWarpingTarget();
	StartTarget->Name = FName("VaultStart");
	StartTarget->Location = VaultStartPos;
	StartTarget->Rotation = GetActorRotation();
	MotionWarpingComponent->AddOrUpdateWarpTarget(*StartTarget);
	// Set vault middle
	FMotionWarpingTarget* MiddleTarget = new FMotionWarpingTarget();
	MiddleTarget->Name = FName("VaultMiddle");
	MiddleTarget->Location = VaultMidPos;
	MiddleTarget->Rotation = GetActorRotation();
	MotionWarpingComponent->AddOrUpdateWarpTarget(*MiddleTarget);
	// Set vault land
	FMotionWarpingTarget* LandTarget = new FMotionWarpingTarget();
	LandTarget->Name = FName("VaultLand");
	LandTarget->Location = VaultLandPos;
	LandTarget->Rotation = GetActorRotation();
	MotionWarpingComponent->AddOrUpdateWarpTarget(*LandTarget);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(VaultMontage);
	AnimInstance->Montage_SetEndDelegate(VaultMontageEndDelegate, VaultMontage);
}

void AMyGameCharacter::OnVaultMontageEnd()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	SetActorEnableCollision(true);
	MyMovementMode = EMyMovementMode::Locomotion;
}

void AMyGameCharacter::OnRollMontageEnd()
{
//	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	MyMovementMode = EMyMovementMode::Locomotion;
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
	if (MyMovementMode == EMyMovementMode::Aiming)
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
		Crouch(); // Cancel crouch
	}
	Super::Jump();
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

	if (MyMovementMode == EMyMovementMode::Aiming)
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
	MyMovementMode = EMyMovementMode::Aiming;
	bForecedToWalk = true;
	//CameraBoom->bEnableCameraRotationLag = false;
	CameraState = ECameraState::Aiming;

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
}

void AMyGameCharacter::StopAiming()
{
	MyMovementMode = EMyMovementMode::Locomotion;
	bForecedToWalk = false;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraState = ECameraState::Normal;
	
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

	// No shooting if not aiming
	StopFiring();
}

void AMyGameCharacter::Crouch(bool bClientSimulation)
{
	bIsCrouched = ~bIsCrouched;
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (bIsCrouched)
	{
		//Movement->MaxWalkSpeed = CROUCH_MAX_SPEED;
	}
	else
	{
		// If there's obstacle above the character, remain crouching
		FVector Start = GetActorLocation();
		FVector End = GetActorLocation() + GetActorUpVector() * UNCROUCH_TRACE_DISTANCE;
		FHitResult HitResult;
		GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh")); // Avoid ray being blocked on charactermesh
		if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, UNCROUCH_TRACE_SIZE, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::Type::ForDuration, HitResult, true))
		{
			bIsCrouched = true;
			GetMesh()->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
			return;
		}
		GetMesh()->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
	}

	Super::Crouch(bClientSimulation);
}

void AMyGameCharacter::Ragdoll()
{
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));
	GetMesh()->SetSimulatePhysics(true);
	isDead = true;
}
