// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/EquipmentBase.h"
#include "WeaponBase.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYGAME_API UWeaponBase : public UEquipmentBase
{
	GENERATED_BODY()

public:
	// Attach to the character's mesh using this socket name when aiming.
	UPROPERTY(EditAnywhere, Category = "Config") // Set in blueprint
	FName AimingSocket;

public:
	UWeaponBase();

	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

virtual void OnEquipped();
virtual void OnUnequipped();
};
