// Fill out your copyright notice in the Description page of Project Settings.
#include "Weapon/RangedWeaponBase.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon/RangedWeaponDataAsset.h"


URangedWeaponBase::URangedWeaponBase()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URangedWeaponBase::Init()
{
	// Get data from data asset
	AmmoCount = DataAsset->AmmoCount;
	MaxAmmo = DataAsset->MaxAmmo;
	FireRate = DataAsset->FireRate;
	BulletsPerCartridge = DataAsset->BulletsPerCartridge;
	BulletTraceSweepRadius = DataAsset->BulletTraceSweepRadius;
	MaxDamageRange = DataAsset->MaxDamageRange;
	HeatToHeatPerShotCurve = DataAsset->HeatToHeatPerShotCurve;
	HeatToCoolDownPerSecondCurve = DataAsset->HeatToCoolDownPerSecondCurve;
	SpreadRecoveryCooldownDelay = DataAsset->SpreadRecoveryCooldownDelay;
	HeatToSpreadCurve = DataAsset->HeatToSpreadCurve;
	SpreadExponent = DataAsset->SpreadExponent;
	MovingSpreadAngleMultiplier = DataAsset->MovingSpreadAngleMultiplier;
	MovingSpeedThreshold = DataAsset->MovingSpeedThreshold;
	ShootingCameraShakeClass = DataAsset->ShootingCameraShakeClass;
	ShootingModes = DataAsset->ShootingModes;

	FireInterval = 1.0f/(FireRate / 60.0f);
	CurrentShootingModeIndex = 0;
	//HeatToHeatPerShotCurve.EditorCurveData.AddKey(0.0f, 1.0f);
	//HeatToCoolDownPerSecondCurve.EditorCurveData.AddKey(0.0f, 2.0f);
}

void URangedWeaponBase::Reload(int &AmmoDepot)
{
	if (AmmoCount != MaxAmmo)
	{
		// Reload ammo from ammo depot but not get over the max ammo number.
		if (AmmoDepot > MaxAmmo - AmmoCount)
		{
			AmmoDepot -= (MaxAmmo - AmmoCount);
			AmmoCount = MaxAmmo;
		}
		else
		{
			AmmoCount += AmmoDepot;
			AmmoDepot = 0;
		}

		// Play character reload montage
		ACharacter* Owner = Cast<ACharacter>(GetOwner());
		if (IsValid(Owner))
		{
			UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(CharacterReloadMontage);
			}
		}

		// Play weapon reload montage

	}
}

void URangedWeaponBase::Fire()
{
		// Play character fire montage
		ACharacter* Owner = Cast<ACharacter>(GetOwner());
		if (IsValid(Owner))
		{
			UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(CharacterFireMontage);
			}
		}

		// Play weapon fire montage

		// Decrease ammo count
		AmmoCount -= BulletsPerCartridge;
		AmmoCount = (AmmoCount < 0) ? 0 : AmmoCount;

		// Reset last fired time
		LastFiredTime = 0.0f;

		// Set fire interval timer
		bCanFire = false;
		GetWorld()->GetTimerManager().SetTimer(FireIntervalTimer, [this]()
		{
			bCanFire = true;
		}, FireInterval, true);
}

void URangedWeaponBase::SwitchShootingMode()
{
	CurrentShootingModeIndex++;
	if (CurrentShootingModeIndex >= ShootingModes.Num())
	{
		CurrentShootingModeIndex = 0;
	}
}

void URangedWeaponBase::OnEquipped()
{
	Super::OnEquipped();
}

void URangedWeaponBase::OnUnequipped()
{
	Super::OnUnequipped();
}

void URangedWeaponBase::AddSpread()
{
	// Sample the heat up curve
	const float HeatPerShot = HeatToHeatPerShotCurve.GetRichCurveConst()->Eval(CurrentHeat);
	CurrentHeat = ClampHeat(CurrentHeat + HeatPerShot);

	// Map the heat to the spread angle
	CurrentSpreadAngle = HeatToSpreadCurve.GetRichCurveConst()->Eval(CurrentHeat);
}

void URangedWeaponBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateLastFiredTime(DeltaTime);
	bool bMinSpread = UpdateSpread(DeltaTime);
	bool bMinMultiplier = UpdateMultipliers(DeltaTime);
	
}

void URangedWeaponBase::ComputeHeatRange(float& MinHeat, float& MaxHeat)
{
	float Min1;
	float Max1;
	HeatToHeatPerShotCurve.GetRichCurveConst()->GetTimeRange(/*out*/ Min1, /*out*/ Max1);

	float Min2;
	float Max2;
	HeatToCoolDownPerSecondCurve.GetRichCurveConst()->GetTimeRange(/*out*/ Min2, /*out*/ Max2);

	float Min3;
	float Max3;
	HeatToSpreadCurve.GetRichCurveConst()->GetTimeRange(/*out*/ Min3, /*out*/ Max3);

	MinHeat = FMath::Min(FMath::Min(Min1, Min2), Min3);
	MaxHeat = FMath::Max(FMath::Max(Max1, Max2), Max3);
}

void URangedWeaponBase::ComputeSpreadRange(float& MinSpread, float& MaxSpread)
{
	HeatToSpreadCurve.GetRichCurveConst()->GetValueRange(/*out*/ MinSpread, /*out*/ MaxSpread);
}

bool URangedWeaponBase::UpdateSpread(float DeltaSeconds)
{
	const float TimeSinceFired = GetWorld()->TimeSince(LastFiredTime);

	if (TimeSinceFired > SpreadRecoveryCooldownDelay)
	{
		const float CooldownRate = HeatToCoolDownPerSecondCurve.GetRichCurveConst()->Eval(CurrentHeat);
		CurrentHeat = ClampHeat(CurrentHeat - (CooldownRate * DeltaSeconds));
		CurrentSpreadAngle = HeatToSpreadCurve.GetRichCurveConst()->Eval(CurrentHeat);
	}

	float MinSpread;
	float MaxSpread;
	ComputeSpreadRange(/*out*/ MinSpread, /*out*/ MaxSpread);

	return FMath::IsNearlyEqual(CurrentSpreadAngle, MinSpread, KINDA_SMALL_NUMBER);
}

bool URangedWeaponBase::UpdateMultipliers(float DeltaSeconds)
{
	const float MultiplierNearlyEqualThreshold = 0.05f;

	CurrentSpreadAngleMultiplier = 1.0f;

	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	UCharacterMovementComponent* CharacterMovementComponent = Cast<UCharacterMovementComponent>(Owner->GetMovementComponent());
	FVector Velocity = CharacterMovementComponent->GetLastUpdateVelocity();
	if (Velocity.Length() >= MovingSpeedThreshold)
	{
		CurrentSpreadAngleMultiplier *= MovingSpreadAngleMultiplier;
	}

	return  FMath::IsNearlyEqual(CurrentSpreadAngleMultiplier, 1.0f, MultiplierNearlyEqualThreshold);
}

void URangedWeaponBase::UpdateLastFiredTime(float DeltaSeconds)
{
	LastFiredTime += DeltaSeconds;

	// Limit last fired time to a max number
	if (LastFiredTime > 500.0f)
	{
		LastFiredTime = 500.0f;
	}
}


