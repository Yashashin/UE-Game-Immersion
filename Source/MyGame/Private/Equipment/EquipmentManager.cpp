 
#include "Equipment/EquipmentManager.h"
#include "Equipment/EquipmentBase.h"

#include "Weapon/WeaponBase.h"
#include "Weapon/RangedWeaponBase.h"
#include "MyGameCharacter.h"
#include "ShootingSystem.h"
#include "Components/SpotLightComponent.h"

UEquipmentManager::UEquipmentManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Set up delegate bindings
	OnEquippedMontageEndDelegate.BindUFunction(this, "OnEquippedMontageEnd");
	OnUnequippedMontageEndDelegate.BindUFunction(this, "OnUnequippedMontageEnd");
}


void UEquipmentManager::BeginPlay()
{
	Super::BeginPlay();
	Init();
}

void UEquipmentManager::Init()
{
	MyGameCharacter = Cast<AMyGameCharacter>(GetOwner());

	if (MyGameCharacter.IsValid())
	{
		// Create equipments and attach them to mesh
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		URangedWeaponBase* Pistol = NewObject<URangedWeaponBase>(GetOwner(), PistolClass);
		if (Pistol)
		{
			Pistol->AttachToComponent(MyGameCharacterPtr->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, Pistol->HolsterSocket);
			Pistol->RegisterComponent();
			Pistol->Init();
			PistolInventory.Add(Pistol);
		}
		URangedWeaponBase* Rifle = NewObject<URangedWeaponBase>(GetOwner(), RifleClass);
		if (Rifle)
		{
			Rifle->AttachToComponent(MyGameCharacterPtr->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, Rifle->HolsterSocket);
			Rifle->RegisterComponent();
			Rifle->Init();
			RifleInventory.Add(Rifle);
		}
		URangedWeaponBase* Shotgun = NewObject<URangedWeaponBase>(GetOwner(), ShotgunClass);
		if (Shotgun)
		{
			Shotgun->AttachToComponent(MyGameCharacterPtr->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, Shotgun->HolsterSocket);
			Shotgun->RegisterComponent();
			Shotgun->Init();
			ShotgunInventory.Add(Shotgun);
		}
		Flashlight = NewObject<UEquipmentBase>(GetOwner(), FlashlightClass);
		if (IsValid(Flashlight))
		{
			Flashlight->AttachToComponent(MyGameCharacterPtr->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, Flashlight->HolsterSocket);
			Flashlight->RegisterComponent();

			SpotLight = MyGameCharacterPtr->FindComponentByClass<USpotLightComponent>();
			if (SpotLight)
			{
				SpotLight->AttachToComponent(Flashlight, FAttachmentTransformRules::KeepRelativeTransform, TEXT("spotlight_Socket"));
				SpotLight->RegisterComponent();
				SpotLight->SetVisibility(false);
			}
		}
	}
}

void UEquipmentManager::OnEquippedMontageEnd()
{

}

void UEquipmentManager::OnUnequippedMontageEnd()
{

}


void UEquipmentManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEquipmentManager::ToggleEquipment()
{
	Unequip();
}

void UEquipmentManager::Equip(EEquipmentType EquipmentType)
{
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();

		// Equip specific type of equipments based on input equipment type
		switch (EquipmentType)
		{
		case EEquipmentType::Pistol:
			CurrentEquipment = PistolInventory[0];
			MyGameCharacterPtr->ShootingSystem->SetCurrentRangedWeapon(PistolInventory[0]);
			MyGameCharacterPtr->UpperBodyBlendWeight = 0.0f;
			MyGameCharacterPtr->RightArmBlendWeight = 0.0f;
			MyGameCharacterPtr->bEnableTurnInPlace = false;
			MyGameCharacterPtr->bForecedToWalk = false;
			break;
		case EEquipmentType::Rifle:
			CurrentEquipment = RifleInventory[0];
			MyGameCharacterPtr->ShootingSystem->SetCurrentRangedWeapon(RifleInventory[0]);
			MyGameCharacterPtr->UpperBodyBlendWeight = 1.0f;
			MyGameCharacterPtr->RightArmBlendWeight = 0.0f;
			MyGameCharacterPtr->bEnableTurnInPlace = true;
			MyGameCharacterPtr->bForecedToWalk = false;
			break;
		case EEquipmentType::Shotgun:
			CurrentEquipment = ShotgunInventory[0];
			MyGameCharacterPtr->ShootingSystem->SetCurrentRangedWeapon(ShotgunInventory[0]);
			MyGameCharacterPtr->UpperBodyBlendWeight = 1.0f;
			MyGameCharacterPtr->RightArmBlendWeight = 0.0f;
			MyGameCharacterPtr->bEnableTurnInPlace = true;
			MyGameCharacterPtr->bForecedToWalk = false;
			break;
		case EEquipmentType::Melee:
			break;
		case EEquipmentType::Dagger:
			break;
		case EEquipmentType::Thrown:
			break;
		case EEquipmentType::Flashlight:
			CurrentEquipment = Flashlight;
			MyGameCharacterPtr->ShootingSystem->SetCurrentRangedWeapon(nullptr);
			MyGameCharacterPtr->UpperBodyBlendWeight = 0.0f;
			MyGameCharacterPtr->RightArmBlendWeight = 1.0f;
			MyGameCharacterPtr->bEnableTurnInPlace = true;
			MyGameCharacterPtr->bForecedToWalk = true;
			break;
		default:
			break;
		}

		if (CurrentEquipment.IsValid())
		{
			UEquipmentBase* EquipmentPtr = CurrentEquipment.Get();

			// Link the animation layer relevant to current equipment
			MyGameCharacter->GetMesh()->LinkAnimClassLayers(EquipmentPtr->EquippedAnimLayer);

			// Play equip montage
			UAnimInstance* AnimInstance = MyGameCharacterPtr->GetMesh()->GetAnimInstance();
			AnimInstance->Montage_Play(EquipmentPtr->EquipMontage);
			AnimInstance->Montage_SetEndDelegate(OnEquippedMontageEndDelegate, EquipmentPtr->EquipMontage);
		}
	}
}

void UEquipmentManager::OnEquipped()
{
	if (MyGameCharacter.IsValid() && CurrentEquipment.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		UEquipmentBase* EquipmentPtr = CurrentEquipment.Get();

		// Attach to holding socket
		AttachEquipmentToSocket(EEquipmentSocket::Holding);

		// Trigger OnEquipped event for equipment
		EquipmentPtr->OnEquipped();

		MyGameCharacterPtr->UpdateUINotify(EUpdateUI::HUD_EquipmentIcon);
	}
}

void UEquipmentManager::Unequip()
{
	if (MyGameCharacter.IsValid() && CurrentEquipment.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		UEquipmentBase* EquipmentPtr = CurrentEquipment.Get();

		// Unlink the animation layer related to unequipped equipment
		MyGameCharacter->GetMesh()->UnlinkAnimClassLayers(EquipmentPtr->EquippedAnimLayer);

		// Play unequip montage
		UAnimInstance* AnimInstance = MyGameCharacterPtr->GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(EquipmentPtr->UnequipMontage);
		AnimInstance->Montage_SetEndDelegate(OnUnequippedMontageEndDelegate, EquipmentPtr->UnequipMontage);

	}
}


void UEquipmentManager::OnUnequipped()
{
	if (MyGameCharacter.IsValid() && CurrentEquipment.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		UEquipmentBase* EquipmentPtr = CurrentEquipment.Get();

		// Trigger OnUnequipped event for equipment
		EquipmentPtr->OnUnequipped();

		// Attach to holster socket
		AttachEquipmentToSocket(EEquipmentSocket::Holster);

		// Set current equipment to null
		MyGameCharacterPtr->ShootingSystem->SetCurrentRangedWeapon(nullptr);
		CurrentEquipment.Reset();

		// Set revelant values to default.
		MyGameCharacterPtr->UpperBodyBlendWeight = 0.0f;
		MyGameCharacterPtr->RightArmBlendWeight = 0.0f;
		MyGameCharacterPtr->bEnableTurnInPlace = false;
		MyGameCharacterPtr->bForecedToWalk = false;

		MyGameCharacterPtr->UpdateUINotify(EUpdateUI::HUD_EquipmentIcon);
	}
	
}


void UEquipmentManager::AttachEquipmentToSocket(EEquipmentSocket Socket)
{
	if (CurrentEquipment.IsValid() && MyGameCharacter.IsValid())
	{
		UEquipmentBase* EquipmentPtr = CurrentEquipment.Get();
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		UWeaponBase* WeaponPtr=nullptr;

		if (EquipmentPtr)
		{
			switch (Socket)
			{
			case EEquipmentSocket::Holster:
				EquipmentPtr->AttachToComponent(MyGameCharacterPtr->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, EquipmentPtr->HolsterSocket);
				break;
			case EEquipmentSocket::Holding:
				EquipmentPtr->AttachToComponent(MyGameCharacterPtr->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, EquipmentPtr->HoldingSocket);
				break;
			case EEquipmentSocket::Aiming:
				WeaponPtr = Cast<UWeaponBase>(EquipmentPtr);
				if (WeaponPtr)
				{
					WeaponPtr->AttachToComponent(MyGameCharacterPtr->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponPtr->AimingSocket);
				}
				break;
			default:
				break;
			}
		}
	}
}

EEquipmentType UEquipmentManager::GetCurrentEquipmentType() const
{
	if (CurrentEquipment.IsValid())
	{
		UEquipmentBase* EquipmentPtr = CurrentEquipment.Get();
		return EquipmentPtr->EquipmentType;
	}
	else
	{
		return EEquipmentType::None;
	}
}

void UEquipmentManager::ToggleFlashLight()
{
	SpotLight->ToggleVisibility();
}

