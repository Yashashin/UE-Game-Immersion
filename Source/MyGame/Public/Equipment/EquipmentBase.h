// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "EquipmentBase.generated.h"

UENUM(BlueprintType)
enum class EEquipmentType :uint8
{
	Medkit UMETA(DisplayName = "Medical Kit"),
	Flashlight UMETA(DisplayName = "Flashlight"),
	Dagger UMETA(DisplayName = "Dagger"),
	Thrown UMETA(DisplayName = "Thrown"),
	Rifle UMETA(DisplayName = "Rifle"),
	Shotgun UMETA(DisplayName = "Shotgun"),
	Pistol UMETA(DisplayName = "Pistol"),
	Melee UMETA(DisplayName = "Melee"),
	None UMETA(DisplayName = "None"),
};

/**
 * 
 */
UCLASS()
class MYGAME_API UEquipmentBase : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	// Animation layer will be linked when this equipment is equipped,
	// and will be unlinked when unequipped.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation") // Set in blueprint
	TSubclassOf<UAnimInstance> EquippedAnimLayer;

	// Character montage plays when this equipment is being equipped.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation") // Set in blueprint
	TObjectPtr<UAnimMontage> EquipMontage;

	// Character montage plays when this equipment is being unequipped.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation") // Set in blueprint
	TObjectPtr<UAnimMontage> UnequipMontage;

	// Attach to the character's mesh using this socket name when equipped.
	UPROPERTY(EditAnywhere, Category = "Config") // Set in blueprint
	FName HoldingSocket;

	// Attach to the character's mesh using this socket name when unequipped.
	UPROPERTY(EditAnywhere, Category = "Config") // Set in blueprint
	FName HolsterSocket;

	// Type of this equipment
	UPROPERTY(EditAnywhere, Category = "Config") // Set in blueprint
	EEquipmentType EquipmentType;

public:
	virtual void OnEquipped();
	virtual void OnUnequipped();
};
