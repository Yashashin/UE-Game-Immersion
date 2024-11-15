// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShootingSystem.generated.h"

class URangedWeaponBase;
class AMyGameCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYGAME_API UShootingSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo Depot")
	int PistolAmmoDepot = 500;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo Depot")
	int RifleAmmoDepot = 500;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo Depot")
	int ShotgunAmmoDepot = 500;

	// Set this value to true while fire button being pressed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bShouldFire = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float SpreadAngle = 0.0f;

private:
	// Access my game character
	TWeakObjectPtr<AMyGameCharacter> MyGameCharacter;

	// Equipped ranged weapon
	TWeakObjectPtr<URangedWeaponBase> CurrentRangedWeapon;

	// Shot counting for burst shooting mode
	int ShotCount = 0;

public:	
	UShootingSystem();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetCurrentRangedWeapon(URangedWeaponBase* Weapon) { CurrentRangedWeapon = Weapon; }

	UFUNCTION()
	bool Fire();

	UFUNCTION()
	void Reload();

	UFUNCTION(BlueprintCallable)
	FText GetAmmoText() const;

	
protected:
	virtual void BeginPlay() override;

private:

	// Init function called in BeginPlay
	void Init();
};
