#include "InputSystem.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MyGameCharacter.h"

UInputSystem::UInputSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInputSystem::Init()
{
	MyGameCharacter = Cast<AMyGameCharacter>(GetOwner());

	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		//Add Input Mapping Context
		if (APlayerController* PlayerController = Cast<APlayerController>(MyGameCharacterPtr->Controller))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void UInputSystem::LStickInput(const FInputActionValue& Value)
{
	if(MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		MyGameCharacter->Move(Value);
	}
}

void UInputSystem::RStickInput(const FInputActionValue& Value)
{
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		MyGameCharacter->Look(Value);
	}
}

void UInputSystem::OnUpButtonPressed()
{
	// Medkit/Flashlight
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		MyGameCharacter->SwitchEquipmentUp();
	}
}

void UInputSystem::OnDownButtonPressed()
{
	// Dagger/Thrown
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		MyGameCharacter->SwitchEquipmentDown();
	}
}

void UInputSystem::OnLeftButtonPressed()
{
	// Rifle/Shotgun
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		MyGameCharacter->SwitchEquipmentLeft();
	}
}

void UInputSystem::OnRightButtonPressed()
{
	// Pistol/Melee
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		MyGameCharacter->SwitchEquipmentRight();
	}
}

void UInputSystem::BeginPlay()
{
	Super::BeginPlay();
	Init();
}

void UInputSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInputSystem::SetupInputBinding(UInputComponent* PlayerInputComponent)
{
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// L stick (Move)
		EnhancedInputComponent->BindAction(LStickAction, ETriggerEvent::Triggered, this, &UInputSystem::LStickInput);

		// R stick (Look)
		EnhancedInputComponent->BindAction(RStickAction, ETriggerEvent::Triggered, this, &UInputSystem::RStickInput);

		// Circle button pressed (Crouch)
		EnhancedInputComponent->BindAction(CircleButtonAction, ETriggerEvent::Started, this, &UInputSystem::OnCircleButtonPressed);

		// Triangle button pressed (Interact)
		EnhancedInputComponent->BindAction(TriangleButtonAction, ETriggerEvent::Started, this, &UInputSystem::OnTriangleButtonPressed);

		// Cross button pressed (Jump)
		EnhancedInputComponent->BindAction(CrossButtonAction, ETriggerEvent::Started, this, &UInputSystem::OnCrossButtonPressed);

		// Square button pressed (Melee)
		EnhancedInputComponent->BindAction(SquareButtonAction, ETriggerEvent::Started, this, &UInputSystem::OnSquareButtonPressed);

		// Up button pressed (Medkit/Flashlight)
		EnhancedInputComponent->BindAction(UpButtonAction, ETriggerEvent::Started, this, &UInputSystem::OnUpButtonPressed);

		// Down button pressed (Dagger/Thrown)
		EnhancedInputComponent->BindAction(DownButtonAction, ETriggerEvent::Started, this, &UInputSystem::OnDownButtonPressed);

		// Left button pressed (Rifle/Shotgun)
		EnhancedInputComponent->BindAction(LeftButtonAction, ETriggerEvent::Started, this, &UInputSystem::OnLeftButtonPressed);

		// Right button pressed (Pistol/Melee)
		EnhancedInputComponent->BindAction(RightButtonAction, ETriggerEvent::Started, this, &UInputSystem::OnRightButtonPressed);

		// L1 button pressed (Start sprinting)
		EnhancedInputComponent->BindAction(L1ButtonAction, ETriggerEvent::Started, this, &UInputSystem::OnL1ButtonPressed);

		// L1 button released (Stop sprinting)
		EnhancedInputComponent->BindAction(L1ButtonAction, ETriggerEvent::Completed, this, &UInputSystem::OnL1ButtonReleased);

		// R1 button released (Equipment swap)
		EnhancedInputComponent->BindAction(R1ButtonAction, ETriggerEvent::Completed, this, &UInputSystem::OnR1ButtonReleased);

		// L2 button pressed/released (Aiming)
		EnhancedInputComponent->BindAction(L2ButtonAction, ETriggerEvent::Started, this, &UInputSystem::OnL2ButtonPressed);
		EnhancedInputComponent->BindAction(L2ButtonAction, ETriggerEvent::Completed, this, &UInputSystem::OnL2ButtonReleased);

		// R2 button pressed (Reload/Start firing)
		EnhancedInputComponent->BindAction(R2ButtonAction, ETriggerEvent::Started, this, &UInputSystem::OnR2ButtonPressed);

		// R2 button released (End firing)
		EnhancedInputComponent->BindAction(R2ButtonAction, ETriggerEvent::Completed, this, &UInputSystem::OnR2ButtonReleased);

		// Vault
		//EnhancedInputComponent->BindAction(InputSystem->VaultAction, ETriggerEvent::Started, this, &AMyGameCharacter::Vault);

		// Assassinate
		//EnhancedInputComponent->BindAction(InputSystem->AssassinateAction, ETriggerEvent::Started, this, &AMyGameCharacter::Assassinate);

		// Sprint
		//EnhancedInputComponent->BindAction(InputSystem->SprintAction, ETriggerEvent::Started, this, &AMyGameCharacter::SprintStart);
		//EnhancedInputComponent->BindAction(InputSystem->SprintAction, ETriggerEvent::Completed, this, &AMyGameCharacter::SprintEnd);

		// Lock
		//EnhancedInputComponent->BindAction(InputSystem->LockAction, ETriggerEvent::Started, this, &AMyGameCharacter::ToggleLock);

		// Menu
		//EnhancedInputComponent->BindAction(InputSystem->MenuAction, ETriggerEvent::Started, this, &AMyGameCharacter::OpenInGameMenu);

		// Climb
		//EnhancedInputComponent->BindAction(InputSystem->ClimbAction, ETriggerEvent::Started, this, &AMyGameCharacter::Climb);

		// Cover
		//EnhancedInputComponent->BindAction(InputSystem->CoverAction, ETriggerEvent::Started, this, &AMyGameCharacter::Cover);


		LStickActionBinding = &EnhancedInputComponent->BindActionValue(LStickAction);
	}
}

FInputActionValue UInputSystem::GetLStickActionValue()
{
	return LStickActionBinding->GetValue();
}

void UInputSystem::OnSquareButtonPressed()
{
	// Melee
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		MyGameCharacter->Melee();
	}
}

void UInputSystem::OnTriangleButtonPressed()
{
	// Interact
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		MyGameCharacter->Interact();
	}
}

void UInputSystem::OnCircleButtonPressed()
{
	// Crouch
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		MyGameCharacter->ToggleCrouched();
	}
}

void UInputSystem::OnCrossButtonPressed()
{
	// Jump
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		
		/* Currently written in blueprint for traversal checking */
		//MyGameCharacter->Jump();
	}
}

void UInputSystem::OnL1ButtonPressed()
{
	// Start sprinting
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();

		MyGameCharacter->StartSprinting();
	}
}

void UInputSystem::OnL1ButtonReleased()
{
	// Stop sprinting
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();

		MyGameCharacter->StopSprinting();
	}
}

void UInputSystem::OnL2ButtonPressed()
{
	// Start aiming
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		MyGameCharacter->StartAiming();
	}
}

void UInputSystem::OnL2ButtonReleased()
{
	// End aiming
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		MyGameCharacter->StopAiming();
	}
}

void UInputSystem::OnR1ButtonReleased()
{
	// Equipment Swap
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		MyGameCharacter->ToggleEquipment();
	}
}

void UInputSystem::OnR2ButtonPressed()
{
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		if (MyGameCharacterPtr->MotionMode == EMotionMode::Aiming)
		{
			// Start firing
			MyGameCharacterPtr->StartFiring();
		}
		else
		{
			// Reload
			MyGameCharacterPtr->Reload();
		}
	}
}

void UInputSystem::OnR2ButtonReleased()
{
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		if (MyGameCharacterPtr->MotionMode == EMotionMode::Aiming)
		{
			// End firing
			MyGameCharacterPtr->StopFiring();
		}
	}
}
