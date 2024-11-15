// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "RangedWeaponBase.generated.h"

UENUM(BlueprintType)
enum class EShootingMode : uint8
{
	Single UMETA(DisplayName = "Single Shot"),
	ThreeRound UMETA(DisplayName = "Three-Round Burst"),
	Auto UMETA(DisplayName = "Full Auto"),
};

class URangedWeaponDataAsset;

/**
 * 
 */
UCLASS()
class MYGAME_API URangedWeaponBase : public UWeaponBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data") // Set in blueprint
	URangedWeaponDataAsset* DataAsset;

	// Shooting montage of the owned character
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation") // Set in blueprint
	TObjectPtr<UAnimMontage> CharacterFireMontage;

	// Shooting montage of this weapon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation") // Set in blueprint
	TObjectPtr<UAnimMontage> WeaponFireMontage;

	// Reloading montage of the owned character
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation") // Set in blueprint
	TObjectPtr<UAnimMontage> CharacterReloadMontage;

	// Reloading montage of this weapon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation") // Set in blueprint
	TObjectPtr<UAnimMontage> WeaponReloadMontage;

	//~Data asset start//
	int32 AmmoCount;
	int32 MaxAmmo;
	int32 FireRate;
	int32 BulletsPerCartridge;
	float BulletTraceSweepRadius;
	float MaxDamageRange;
	FRuntimeFloatCurve HeatToHeatPerShotCurve;
	FRuntimeFloatCurve HeatToCoolDownPerSecondCurve;
	float SpreadRecoveryCooldownDelay;
	FRuntimeFloatCurve HeatToSpreadCurve;
	float SpreadExponent;
	float MovingSpreadAngleMultiplier;
	float MovingSpeedThreshold;
	TSubclassOf<UCameraShakeBase> ShootingCameraShakeClass;
	TArray<EShootingMode> ShootingModes;
	//~End of data asset//

	// Current heat vaule
	float CurrentHeat = 0.0f;

	// Current spread vaule
	float CurrentSpreadAngle = 0.0f;

	// Current spread angle multiplier
	float CurrentSpreadAngleMultiplier = 1.0f;

	// Time since this weapon was last fired (relative to world time)
	double LastFiredTime = 0.0;

	// Set this to false during fire interval
	bool bCanFire = true;

private:
	// Interval between shots (in seconds)
	float FireInterval = 0.1f;

	// Timer handle for firing interval
	FTimerHandle FireIntervalTimer;

	// Index of shooting modes currently used
	int CurrentShootingModeIndex;

public:
	URangedWeaponBase();

	/** Perform initialization, like getting data from data asset */
	void Init();

	/** Reload ammo from ammo depot and play reload montage */
	void Reload(int &AmmoDepot);

	/** Decrease the ammo count and play fire montage */
	void Fire();

	/** Add spread according to heat and spread curves */
	void AddSpread();

	/** Switch to next shooting mode */
	void SwitchShootingMode();

	virtual void OnEquipped();
	virtual void OnUnequipped();

	/** Return current shooting mode */
	EShootingMode GetCurrentShootingMode() { return ShootingModes[CurrentShootingModeIndex]; }

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void ComputeHeatRange(float& MinHeat, float& MaxHeat);

	void ComputeSpreadRange(float& MinSpread, float& MaxSpread);

	inline float ClampHeat(float NewHeat)
	{
		float MinHeat;
		float MaxHeat;
		ComputeHeatRange(/*out*/ MinHeat, /*out*/ MaxHeat);

		return FMath::Clamp(NewHeat, MinHeat, MaxHeat);
	}

	// Update the spread and returns true if the spread is at minimum
	bool UpdateSpread(float DeltaSeconds);

	// Update the multipliers and returns true if they are at minimum
	bool UpdateMultipliers(float DeltaSeconds);

	// Update last fired time 
	void UpdateLastFiredTime(float DeltaSeconds);

};
