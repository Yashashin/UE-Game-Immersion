
#include "ShootingSystem.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "MyGameCharacter.h"
#include "Camera/CameraComponent.h"

#include "Weapon/RangedWeaponBase.h"
#include "Equipment/EquipmentManager.h"

UShootingSystem::UShootingSystem()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UShootingSystem::BeginPlay()
{
	Super::BeginPlay();
	Init();
}

bool UShootingSystem::Fire()
{
	if (MyGameCharacter.IsValid() && CurrentRangedWeapon.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		URangedWeaponBase* CurrentWeaponPtr = CurrentRangedWeapon.Get();
		if (CurrentWeaponPtr->AmmoCount > 0 && CurrentWeaponPtr->bCanFire)
		{
			CurrentWeaponPtr->Fire();

			// Play camera shake
			APlayerController* PlayerController = Cast<APlayerController>(MyGameCharacterPtr->GetController());
			if (PlayerController)
			{
				PlayerController->ClientStartCameraShake(CurrentWeaponPtr->ShootingCameraShakeClass);
			}

			// Get spread angle
			SpreadAngle = CurrentWeaponPtr->CurrentSpreadAngle * CurrentWeaponPtr->CurrentSpreadAngleMultiplier * CurrentWeaponPtr->SpreadExponent;
			float RandomYaw = FMath::RandRange(-SpreadAngle, SpreadAngle);
			float RandomPitch = FMath::RandRange(-SpreadAngle, SpreadAngle);

			// Adjust shooting offset
			FRotator SpreadRotation = FRotator(RandomPitch, RandomYaw, 0.0f);
			FVector FinalShotDirection = SpreadRotation.RotateVector(MyGameCharacterPtr->FollowCamera->GetForwardVector());

			FVector Start = MyGameCharacterPtr->FollowCamera->GetComponentLocation();
			FVector End = Start + FinalShotDirection * (CurrentWeaponPtr->MaxDamageRange);
			FHitResult HitResult;
			if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::Type::ForDuration, HitResult, true))
			{
				// CurrentWeaponPtr->GetSocketLocation(TEXT("muzzle_Socket"));
				/*HitLocation = HitResult.Location;
				bIsHit = true;
				break;*/
			}

			switch (CurrentWeaponPtr->GetCurrentShootingMode())
			{
			case EShootingMode::Single:
				// Stop firing
				bShouldFire = false;
				break;
			case EShootingMode::ThreeRound:
				ShotCount++;
				if (ShotCount == 3)
				{
					// Stop firing when desired bullet shot
					bShouldFire = false;
					ShotCount = 0;
				}
				break;
			default:
				break;
			}

			CurrentWeaponPtr->AddSpread();
		}
	}
	return false;
}

void UShootingSystem::Reload()
{
	if (CurrentRangedWeapon.IsValid() && MyGameCharacter.IsValid())
	{
		URangedWeaponBase* CurrentWeaponPtr = CurrentRangedWeapon.Get();
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();

		// Reload based on weapon type
		switch (CurrentWeaponPtr->EquipmentType)
		{
		case EEquipmentType::Pistol:
			CurrentWeaponPtr->Reload(PistolAmmoDepot);
			break;
		case EEquipmentType::Rifle:
			CurrentWeaponPtr->Reload(RifleAmmoDepot);
			break;
		case EEquipmentType::Shotgun:
			CurrentWeaponPtr->Reload(ShotgunAmmoDepot);
			break;
		default:
			break;
		}
	}
}

FText UShootingSystem::GetAmmoText() const
{
	FString String;
	if (CurrentRangedWeapon.IsValid())
	{
		URangedWeaponBase* CurrentWeaponPtr = CurrentRangedWeapon.Get();
		switch (CurrentWeaponPtr->EquipmentType)
		{
		case EEquipmentType::Pistol:
			String = FString::Printf(TEXT("%d/%d"), CurrentWeaponPtr->AmmoCount, PistolAmmoDepot);
			break;
		case EEquipmentType::Rifle:
			String = FString::Printf(TEXT("%d/%d"), CurrentWeaponPtr->AmmoCount, RifleAmmoDepot);
			break;
		case EEquipmentType::Shotgun:
			String = FString::Printf(TEXT("%d/%d"), CurrentWeaponPtr->AmmoCount, ShotgunAmmoDepot);
			break;
		default:
			String = FString::Printf(TEXT(""));
			break;
		}
	}
	else
	{
		String = FString::Printf(TEXT(""));
	}
	return FText::FromString(String);
}

void UShootingSystem::Init()
{
	MyGameCharacter = Cast<AMyGameCharacter>(GetOwner());
	//SetComponentTickEnabled(false);
}


void UShootingSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentRangedWeapon.IsValid())
	{
		URangedWeaponBase* Weapon = CurrentRangedWeapon.Get();
		SpreadAngle = Weapon->CurrentSpreadAngle;
	}

	if (bShouldFire)
	{
		Fire();
	}
}

