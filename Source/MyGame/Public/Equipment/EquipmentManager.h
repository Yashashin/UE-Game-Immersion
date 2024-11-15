
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon/WeaponBase.h"
#include "Weapon/RangedWeaponBase.h"
#include "Equipment/EquipmentBase.h"
#include "EquipmentManager.generated.h"

UENUM()
enum class EEquipmentSocket :uint8
{
	Holster,
	Holding,
	Aiming,
};

class UWeaponBase;
class UEquipmentBase;
class URangedWeaponBase;
class AMyGameCharacter;
class USpotLightComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYGAME_API UEquipmentManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Weapon|Pistol") // Set in blueprint
	TSubclassOf<UWeaponBase> PistolClass;

	UPROPERTY(EditAnywhere, Category = "Weapon|Rifle") // Set in blueprint
	TSubclassOf<UWeaponBase> RifleClass;

	UPROPERTY(EditAnywhere, Category = "Weapon|Shotgun") // Set in blueprint
	TSubclassOf<UWeaponBase> ShotgunClass;

	UPROPERTY(EditAnywhere, Category = "Equipment|Flashlight") // Set in blueprint
	TSubclassOf<UEquipmentBase> FlashlightClass;

	// Currently equipped equipment
	// Character can only hold one equipment at a time.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment");
	TWeakObjectPtr<UEquipmentBase> CurrentEquipment;

	// All pistols the character owns
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Pistol")
	TArray<TObjectPtr<URangedWeaponBase>> PistolInventory;

	// All Rifles the character owns
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Rifle")
	TArray<TObjectPtr<URangedWeaponBase>> RifleInventory;

	// All shotguns the character owns
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Shotgun")
	TArray<TObjectPtr<URangedWeaponBase>> ShotgunInventory;

	// All equipments the character owns excepct weapons
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment|Flashlight")
	TObjectPtr<UEquipmentBase> Flashlight;

private:
	// Access my game character
	TWeakObjectPtr<AMyGameCharacter> MyGameCharacter;

	// Delegate binds OnEquippedMontageEnd() when montage ends.
	FOnMontageEnded OnEquippedMontageEndDelegate;

	//  Delegate binds OnUnequippedMontageEnd() when montage ends.
	FOnMontageEnded OnUnequippedMontageEndDelegate;

	USpotLightComponent* SpotLight = nullptr;

public:
	UEquipmentManager();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ToggleEquipment();

	/**
  	 * Equip equipment based on the equipment type and set it to current equipment. 
	 * This function is called when we want to equip the equipment.
	 */
	void Equip(EEquipmentType EquipmentType);

	/** 
	 * Unequip the current equipment and set it to null. 
	 * This function is called when we want to unequip the equipment.
	 */
	void Unequip();

	/**
	 * This function is called at the moment any equipment is equipped.
	 * It is essentially during the equip montage playback.
	 */
	UFUNCTION(BlueprintCallable)
	void OnEquipped();

	/**
	 * This function is called at the moment any equipment is unequipped.
	 * It is essentially during the unequip montage playback.
	 */
	UFUNCTION(BlueprintCallable)
	void OnUnequipped();

	/** Attach current equipment to socket */
	void AttachEquipmentToSocket(EEquipmentSocket Socket);

	/** Return the current equipment type */
	UFUNCTION(BlueprintPure)
	EEquipmentType GetCurrentEquipmentType() const;

	/** Turn on/off the spot light */
	UFUNCTION(BlueprintCallable)
	void ToggleFlashLight();


protected:
	virtual void BeginPlay() override;

private:
	/** This function is called in BeginPlay for initialization. */
	void Init();

	/** Callback function for OnEquipped montage. */
	UFUNCTION()
	void OnEquippedMontageEnd();

	/** Callback function for UnEquipped montage. */
	UFUNCTION()
	void OnUnequippedMontageEnd();
};
