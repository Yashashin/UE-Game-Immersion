// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Enums.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ECameraState : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Far UMETA(DisplayName = "Far"),
	Close UMETA(DisplayName = "Close"),
	Indoor UMETA(DisplayName = "Indoor"),
	Aiming  UMETA(DisplayName = "Aiming"),
	Crouch_Normal UMETA(DisplayName = "Crouch Normal"),
	Crouch_Aiming  UMETA(DisplayName = "Crouch Aiming"),
	Death UMETA(DisplayName = "Death"),
};

UENUM(BlueprintType)
enum class EGait : uint8
{
	Walk UMETA(DisplayName = "Walk"),
	Run UMETA(DisplayName = "Run"),
	Sprint UMETA(DisplayName = "Sprint"),
};

UENUM(BlueprintType)
enum class EStance : uint8
{
	Stand UMETA(DisplayName = "Stand"),
	Crouch UMETA(DisplayName = "Crouch"),
	Prone UMETA(DisplyName = "Prone"),
};

UENUM(BlueprintType)
enum class EMyMovementMode : uint8
{
	OnGround UMETA(DisplayName = "OnGround"),
	InAir UMETA(DisplayName = "InAir"),
};

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Moving UMETA(DisplayName = "Moving"),
};

UENUM(BlueprintType)
enum class EMotionMode : uint8
{
	Locomotion UMETA(DisplayName = "Locomotion"),
	Aiming UMETA(DisplayName = "Aiming"),
	Attacking UMETA(DisplayName = "Attacking"),
	Vaulting UMETA(DisplayName = "Vaulting"),
	Rolling UMETA(DisplayName = "Rolling"),
	Climbing UMETA(DisplayName = "Climbing"),
	Covering UMETA(DisplayName = "Covering"),
	None UMETA(DisplayName = "None"),
};



UENUM(BlueprintType)
enum class EUpdateUI : uint8
{
	HUD_EquipmentIcon UMETA(DisplayName = "HUD Equipment Icon"),
	HUD_AmmoCount UMETA(DisplayName = "HUD Ammo Count"),
	HUD_HealthBar UMETA(DisplayName = "HUD Health Bar"),
	Crosshair UMETA(DisplayName = "Crosshair"),
};