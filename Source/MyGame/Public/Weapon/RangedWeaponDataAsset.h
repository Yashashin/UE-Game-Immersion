// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RangedWeaponDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API URangedWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Current ammo count
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 AmmoCount = 30;

	// Maximum ammo count per magazine
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxAmmo = 30;

	// How many bullets are shot per minute (Rounds Per Minute)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ForceUnits = RPM))
	int32 FireRate = 120;

	// Number of bullets to fire in a single cartridge (typically 1, but may be more for shotguns)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 BulletsPerCartridge = 1;

	// The radius for bullet traces sweep spheres (0.0 will result in a line trace)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ForceUnits = cm))
	float BulletTraceSweepRadius = 0.0f;

	// The maximum distance at which this weapon can damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ForceUnits = cm))
	float MaxDamageRange = 25000.0f;

	// Camera shake during firing
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UCameraShakeBase> ShootingCameraShakeClass;

	// A curve that maps the current heat to the amount a single shot will further 'heat up'
    // This is typically a flat curve with a single data point indicating how much heat a shot adds,
	// but can be other shapes to do things like punish overheating by adding progressively more heat.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heat")
	FRuntimeFloatCurve HeatToHeatPerShotCurve;

	// A curve that maps the current heat to the heat cooldown rate per second
	// This is typically a flat curve with a single data point indicating how fast the heat
	// wears off, but can be other shapes to do things like punish overheating by slowing down
	// recovery at high heat.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heat")
	FRuntimeFloatCurve HeatToCoolDownPerSecondCurve;

	// Time since firing before cooldown recovery begins (in seconds)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ForceUnits = s), Category = "Heat")
	float SpreadRecoveryCooldownDelay = 0.0f;

	// A curve that maps the heat to the spread angle
	// The X range of this curve typically sets the min/max heat range of the weapon
	// The Y range of this curve is used to define the minimun and maximum spread angle
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread")
	FRuntimeFloatCurve HeatToSpreadCurve;

	// Affects how tightly shots will cluster around the center line
	// when the weapon has spread (non-perfect accuracy). Higher values will cause shots
	// to be closer to the center (default is 1.0 which means uniformly within the spread range)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0.1), Category = "Spread")
	float SpreadExponent = 1.0f;

	// Spread angle multiplier when character is moving
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread", meta = (ForceUnits = x))
	float MovingSpreadAngleMultiplier = 1.2f;

	// Speeds at or above this are considered moving
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread", meta = (ForceUnits = "cm/s"))
	float MovingSpeedThreshold = 50.0f;

	// Shooting modes available for this weapon
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<EShootingMode> ShootingModes;

};
